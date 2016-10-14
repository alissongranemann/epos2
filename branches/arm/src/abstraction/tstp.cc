// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <utility/math.h>

__BEGIN_SYS

// TSTP_Router
// Class attributes

// Methods
void TSTP_Router::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
//    db<TSTP_Router>(TRC) << "TSTP_Router::update(obs=" << obs << ",buf=" << buf << ")" << endl;
//    if(buf->is_microframe && !buf->relevant)
//        buf->relevant = TSTP::here() - TSTP::sink_address() < buf->frame()->data<Microframe>()->hint();
//    if(!buf->is_microframe)
//        if(buf->my_distance < buf->sender_distance)
//            if(Buffer * send_buf = TSTP::_nic->alloc(TSTP::_nic->broadcast(), buf->frame()->data<Header>()->type(), 0, 0, buf->size()))
//                TSTP::_nic->send(send_buf);
}

TSTP_Router::~TSTP_Router()
{
    db<TSTP_Router>(TRC) << "TSTP_Router::~TSTP_Router()" << endl;
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
