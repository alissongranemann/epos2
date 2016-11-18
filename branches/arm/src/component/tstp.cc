// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <utility/math.h>
#include <utility/string.h>

__BEGIN_SYS

// TSTP::Locator
// Class attributes

// Methods
void TSTP::Locator::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe) {
        if(!buf->downlink)
            buf->my_distance = here() - TSTP::sink();
    } else
        buf->my_distance = here() - TSTP::destination(buf).center;
}

void TSTP::Locator::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::marshal(buf=" << buf << ")" << endl;
    buf->my_distance = here() - TSTP::destination(buf).center;
    buf->sender_distance = buf->my_distance;
}

TSTP::Locator::~Locator()
{
    db<TSTP>(TRC) << "TSTP::~Locator()" << endl;
    TSTP::_nic->detach(this, 0);
}

// TODO: we need a better way to define static locations
TSTP::Coordinates TSTP::Locator::here()
{
    if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x84\x0d\x06", 8))
        return TSTP::sink();
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xec\x82\x0d\x06", 8))
        return Coordinates( 0,10, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xae\x82\x0d\x06", 8))
        return Coordinates(10,10, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x67\x83\x0d\x06", 8))
        return Coordinates(10, 0, 0);

    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xca\x0e\x16\x06", 8))
        return TSTP::sink();
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xee\x0e\x16\x06", 8))
        return Coordinates(50,50,50);

    else
        return Coordinates(-1, -1, -1);
}

// TSTP::Timekeeper
// Class attributes

// Methods
void TSTP::Timekeeper::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(!buf->is_microframe) {
        buf->deadline = TSTP::destination(buf).t1;

        bool peer_closer_to_sink = buf->downlink ?
            (buf->my_distance > buf->sender_distance) :
            (TSTP::here() - TSTP::sink() > buf->frame()->data<Header>()->last_hop() - TSTP::sink());

        if(peer_closer_to_sink) {
            NIC::Timer::Offset adj = buf->frame()->data<Header>()->last_hop_time() - (NIC::Timer::sfd() + NIC::Timer::us2count(IEEE802_15_4::SHR_SIZE * 1000000 / IEEE802_15_4::BYTE_RATE));

            db<TSTP>(INF) << "TSTP::Timekeeper::update: adjusting timer by " << adj << endl;

            NIC::Timer::adjust(adj);
        }
    }
}

void TSTP::Timekeeper::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::marshal(buf=" << buf << ")" << endl;
    buf->deadline = TSTP::destination(buf).t1;
}

TSTP::Timekeeper::~Timekeeper()
{
    db<TSTP>(TRC) << "TSTP::~Timekeeper()" << endl;
    TSTP::_nic->detach(this, 0);
}

// TSTP::Router
// Class attributes

// Methods
void TSTP::Router::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Router::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe && !buf->relevant) {
        buf->relevant = buf->my_distance < buf->sender_distance;
    } else if(!buf->is_microframe) {
        Region dst = TSTP::destination(buf);
        buf->destined_to_me = dst.contains(TSTP::here(), dst.t0);
        if(buf->my_distance < buf->sender_distance) {
            // Forward the message

            Buffer * send_buf = TSTP::alloc(buf->size());

            // Copy frame contents
            memcpy(send_buf->frame(), buf->frame(), buf->size());

            // Copy Buffer Metainformation
            send_buf->id = buf->id;
            send_buf->destined_to_me = buf->destined_to_me;
            send_buf->downlink = buf->downlink;
            send_buf->deadline = buf->deadline;
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

void TSTP::Router::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Router::marshal(buf=" << buf << ")" << endl;
    TSTP::Region dest = TSTP::destination(buf);
    buf->downlink = dest.center != TSTP::sink();
    buf->destined_to_me = dest.contains(TSTP::here(), TSTP::now());

    offset(buf);
}

TSTP::Router::~Router()
{
    db<TSTP>(TRC) << "TSTP::~Router()" << endl;
    TSTP::_nic->detach(this, 0);
}

// TSTP::Security
// Class attributes

// Methods
void TSTP::Security::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::update(obs=" << obs << ",buf=" << buf << ")" << endl;
}

void TSTP::Security::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::marshal(buf=" << buf << ")" << endl;
}

TSTP::Security::~Security()
{
    db<TSTP>(TRC) << "TSTP::~Security()" << endl;
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
