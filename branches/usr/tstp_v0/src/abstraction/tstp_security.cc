// EPOS Trustful SpaceTime Security Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <tstp_mac.h>

__BEGIN_SYS

// Class attributes
TSTP_Security * TSTP_Security::_instance;

// Methods
bool TSTP_Security::do_bootstrap() 
{
    _is_sink = TSTP::here() == Coordinates(0,0,0);

    db<TSTP>(TRC) << "TSTP_Security::bootstrap()" << endl;

    if(_is_sink) {
        Buffer * buf = TSTPNIC::alloc(sizeof(TSTP::DH_Request));
        new (buf->frame()->data<char>()) TSTP::DH_Request(Coordinates(10,10,10), _dh.public_key());
        TSTPNIC::send(buf);
    } else {
        _sem.p();
        Buffer * buf = TSTPNIC::alloc(sizeof(TSTP::Auth_Request));
        new (buf->frame()->data<char>()) TSTP::Auth_Request(_auth, otp());
        TSTPNIC::send(buf);
        _sem.p();
    }

    return true;
}

void TSTP_Security::update(TSTPNIC::Observed * obs, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP_Security::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(buf->is_rx() && buf->destined_to_me()) {
        TSTP::Header * hdr = buf->frame()->data<Header>();
        db<TSTP>(TRC) << hdr->type() << endl;
        if(hdr->type() == CONTROL) {
            TSTP::Control * cnt = buf->frame()->data<TSTP::Control>();
            db<TSTP>(TRC) << "TSTP_Security::processing: " << *cnt << endl;
            switch(cnt->subtype()) {
                case DH_REQUEST: {
                    if(!_is_sink) {
                        Buffer * resp = TSTPNIC::alloc(sizeof(TSTP::DH_Response));
                        new (resp->frame()->data<char>()) TSTP::DH_Response(_dh.public_key());
                        TSTPNIC::send(resp);

                        TSTP::DH_Request * dhr = buf->frame()->data<TSTP::DH_Request>();
                        _master_secret = _dh.shared_key(dhr->key());
                        _sem.v();
                    }
                }
                break;
                case DH_RESPONSE: {
                    if(_is_sink) {
                        TSTP::DH_Response * dhr = buf->frame()->data<TSTP::DH_Response>();
                        _master_secret = _dh.shared_key(dhr->key());
                    }
                }
                break;
                case AUTH_REQUEST: {
                    if(_is_sink) {
                        Buffer * resp = TSTPNIC::alloc(sizeof(TSTP::Auth_Granted));
                        new (resp->frame()->data<char>()) TSTP::Auth_Granted(hdr->origin(), Auth()); // TODO
                        TSTPNIC::send(resp);
                    }
                }
                break;
                case AUTH_GRANTED: {
                    if(!_is_sink) {
                        _sem.v();
                    }
                }
                break;
                default:
                break;
            }
        }
    }
}

__END_SYS

#endif
