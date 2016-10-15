// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <utility/math.h>

__BEGIN_SYS

// TSTP_Locator
// Class attributes

// Methods
void TSTP_Locator::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP_Locator>(TRC) << "TSTP_Locator::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe)
        buf->sender_distance = buf->frame()->data<Microframe>()->hint();
    else
        buf->my_distance = here() - TSTP::destination(buf).center;
}

void TSTP_Locator::marshal(Buffer * buf)
{
    db<TSTP_Locator>(TRC) << "TSTP_Locator::marshal(buf=" << buf << ")" << endl;
    buf->my_distance = here() - TSTP::destination(buf).center;
    buf->sender_distance = buf->my_distance;
}

TSTP_Locator::~TSTP_Locator()
{
    db<TSTP_Locator>(TRC) << "TSTP_Locator::~TSTP_Locator()" << endl;
    TSTP::_nic->detach(this, 0);
}

TSTP_Locator::Coordinates TSTP_Locator::here() { return Coordinates(5,5,5); } // TODO

// TSTP_Time_Manager
// Class attributes

// Methods
void TSTP_Time_Manager::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP_Time_Manager>(TRC) << "TSTP_Time_Manager::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    buf->expiry = TSTP::destination(buf).t1;
}

void TSTP_Time_Manager::marshal(Buffer * buf)
{
    db<TSTP_Time_Manager>(TRC) << "TSTP_Time_Manager::marshal(buf=" << buf << ")" << endl;
    buf->expiry = TSTP::destination(buf).t1;
}

TSTP_Time_Manager::~TSTP_Time_Manager()
{
    db<TSTP_Time_Manager>(TRC) << "TSTP_Time_Manager::~TSTP_Time_Manager()" << endl;
    TSTP::_nic->detach(this, 0);
}

TSTP_Time_Manager::Time TSTP_Time_Manager::now()
{
    // FIXME: this is not a pretty way to access the MAC timer...
#ifdef __mmod_emote3__
    return TSTP_MAC<CC2538RF>::Timer::count2us(TSTP_MAC<CC2538RF>::Timer::now());
#else
    return RTC::seconds_since_epoch();
#endif
}

// TSTP_Router
// Class attributes

// Methods
void TSTP_Router::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP_Router>(TRC) << "TSTP_Router::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe && !buf->relevant)
        buf->relevant = TSTP::here() - TSTP::sink() < buf->sender_distance;
    if(!buf->is_microframe) {
        buf->destined_to_me = TSTP::destination(buf).contains(TSTP::here(), TSTP::now());
        if(buf->my_distance < buf->sender_distance) {
            // Forward the message

            Buffer * send_buf = TSTP::_nic->alloc(TSTP::_nic->broadcast(), NIC::TSTP, 0, 0, buf->size());

            // Copy frame contents
            memcpy(send_buf->frame(), buf->frame(), buf->size());

            // Copy Buffer Metainformation
            send_buf->id = buf->id;
            send_buf->destined_to_me = buf->destined_to_me;
            send_buf->downlink = buf->downlink;
            send_buf->expiry = buf->expiry;
            send_buf->origin_time = buf->origin_time;
            send_buf->my_distance = buf->my_distance;
            send_buf->sender_distance = buf->sender_distance;
            send_buf->is_new = false;
            send_buf->is_microframe = false;

            // Calculate offset
            offset(send_buf);

            TSTP::_nic->send(send_buf);
        }
    }
}

void TSTP_Router::marshal(Buffer * buf)
{
    db<TSTP_Router>(TRC) << "TSTP_Router::marshal(buf=" << buf << ")" << endl;
    TSTP::Region dest = TSTP::destination(buf);
    buf->downlink = dest.center == TSTP::sink();
    buf->destined_to_me = dest.contains(TSTP::here(), TSTP::now());

    offset(buf);
}

TSTP_Router::~TSTP_Router()
{
    db<TSTP_Router>(TRC) << "TSTP_Router::~TSTP_Router()" << endl;
    TSTP::_nic->detach(this, 0);
}

// TSTP_Security_Manager
// Class attributes

// Methods
void TSTP_Security_Manager::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP_Security_Manager>(TRC) << "TSTP_Security_Manager::update(obs=" << obs << ",buf=" << buf << ")" << endl;
}

void TSTP_Security_Manager::marshal(Buffer * buf)
{
    db<TSTP_Security_Manager>(TRC) << "TSTP_Security_Manager::marshal(buf=" << buf << ")" << endl;
}

TSTP_Security_Manager::~TSTP_Security_Manager()
{
    db<TSTP_Security_Manager>(TRC) << "TSTP_Security_Manager::~TSTP_Security_Manager()" << endl;
    TSTP::_nic->detach(this, 0);
}


// TSTP
// Class attributes
NIC * TSTP::_nic;
TSTP::Interests TSTP::_interested;
TSTP::Responsives TSTP::_responsives;
TSTP::Observed TSTP::_observed;

// Methods
TSTP::~TSTP()
{
    db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
    _nic->detach(this, 0);
}

void TSTP::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(buf->is_microframe)
        return;

    Packet * packet = buf->frame()->data<Packet>();
    switch(packet->type()) {
    case INTEREST: {
        Interest * interest = reinterpret_cast<Interest *>(packet);
        db<TSTP>(INF) << "TSTP::update:interest=" << interest << " => " << *interest << endl;
        // Check for local capability to respond and notify interested observers
        Responsives::List * list = _responsives[interest->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
        if(list)
            for(Responsives::Element * el = list->head(); el; el = el->next()) {
                Responsive * responsive = el->object();
                if(interest->region().contains(responsive->origin(), now())) {
                    notify(responsive, buf);
                }
            }
    } break;
    case RESPONSE: {
        Response * response = reinterpret_cast<Response *>(packet);
        db<TSTP>(INF) << "TSTP::update:response=" << response << " => " << *response << endl;
        // Check region inclusion and notify interested observers
        Interests::List * list = _interested[response->unit()];
        if(list)
            for(Interests::Element * el = list->head(); el; el = el->next()) {
                Interested * interested = el->object();
                if(interested->region().contains(response->origin(), response->time()))
                    notify(interested, buf);
            }
    } break;
    case COMMAND: {
        Command * command = reinterpret_cast<Command *>(packet);
        db<TSTP>(INF) << "TSTP::update:command=" << command << " => " << *command << endl;
        // Check for local capability to respond and notify interested observers
        Responsives::List * list = _responsives[command->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
        if(list)
            for(Responsives::Element * el = list->head(); el; el = el->next()) {
                Responsive * responsive = el->object();
                if(command->region().contains(responsive->origin(), now()))
                    notify(responsive, buf);
            }
    } break;
    case CONTROL: break;
    }

    _nic->free(buf);
}

__END_SYS

#endif
