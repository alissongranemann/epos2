// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <tstp_mac.h>

__BEGIN_SYS

// Class attributes
TSTP::Interests TSTP::_interested;
TSTP::Responsives TSTP::_responsives;

TSTP::Observed TSTP::_observed;

// Methods
void Greedy_Geographic_Router::update(TSTPNIC::Observed * obs, Buffer * buf) {
    db<TSTP>(TRC) << "Greedy_Geographic_Router::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    Coordinates destination;
    if(buf->is_frame()) {
        Header * hdr = buf->frame()->data<Header>();
        bool for_me = false;
        Distance my_distance(0);

        switch (hdr->type()) {
            case INTEREST: {
                Region dst = buf->frame()->data<TSTP::Interest>()->region();
                for_me = dst.contains(TSTP::here(), TSTP::now());
                new (&destination) Coordinates(dst.center.x, dst.center.y, dst.center.z);
                my_distance = destination - TSTP::here();
                }
                break;
            case RESPONSE: {
                new (&destination) Coordinates(0,0,0);
                for_me = TSTP::here() == destination;
                my_distance = destination - TSTP::here();
                }
                break;
            default:
                break;
        }
        buf->destined_to_me(for_me);
        buf->my_distance(my_distance);
    }

    if(buf->is_tx()) {
        if(buf->is_frame()) {
            buf->offset((Random::random() % (TSTP_MAC::PERIOD - 4096)) + 4096); // TODO
        }
    } else {
        if(buf->is_microframe()) {
            TSTP_MAC::Microframe * mf = buf->frame()->data<TSTP_MAC::Microframe>();
            buf->relevant(mf->all_listen() or (Coordinates(0,0,0) - TSTP::here() < mf->hint()));
        } else {
            if(buf->destined_to_me() or (buf->my_distance() < (destination - buf->frame()->data<Header>()->last_hop()))) {
                Buffer * forwarding = TSTPNIC::alloc(buf->size());
                memcpy(forwarding->frame(), buf->frame(), buf->size());
                //forwarding->deadline(buf->deadline());
                forwarding->deadline(TSTP::now() + 1000000); // TODO
                forwarding->origin_time(buf->origin_time());
                forwarding->id(buf->id());
                TSTPNIC::send(forwarding);
            }
        }
    }
}

__END_SYS

#endif
