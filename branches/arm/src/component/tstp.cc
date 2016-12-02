// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <utility/math.h>
#include <utility/string.h>

__BEGIN_SYS

// TSTP::Locator
// Class attributes
TSTP::Coordinates TSTP::Locator::_here;

// Methods
void TSTP::Locator::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe) {
        if(!buf->downlink)
            buf->my_distance = here() - TSTP::sink();
    } else {
        Coordinates dst = TSTP::destination(buf).center;
        buf->my_distance = here() - dst;
        buf->downlink = dst != TSTP::sink(); // This would fit better in the Router, but Timekeeper uses this info
    }
}

void TSTP::Locator::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::marshal(buf=" << buf << ")" << endl;
    Coordinates dst = TSTP::destination(buf).center;
    buf->my_distance = here() - dst;
    buf->downlink = dst != TSTP::sink(); // This would fit better in the Router, but Timekeeper uses this info
}

TSTP::Locator::~Locator()
{
    db<TSTP>(TRC) << "TSTP::~Locator()" << endl;
    TSTP::_nic->detach(this, 0);
}

// TSTP::Timekeeper
// Class attributes
TSTP::Timekeeper::Time_Stamp TSTP::Timekeeper::_t0;
TSTP::Timekeeper::Time_Stamp TSTP::Timekeeper::_t1;
TSTP::Timekeeper::Time_Stamp TSTP::Timekeeper::_t2;
TSTP::Timekeeper::Time_Stamp TSTP::Timekeeper::_t3;

// Methods
void TSTP::Timekeeper::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(!buf->is_microframe) {
        buf->deadline = TSTP::destination(buf).t1;

        bool peer_closer_to_sink = buf->downlink ?
            (TSTP::here() - TSTP::sink() > buf->frame()->data<Header>()->last_hop() - TSTP::sink()) :
            (buf->my_distance > buf->sender_distance);

        if(peer_closer_to_sink) {
            _t0 = buf->sfd_time_stamp;
            _t1 = buf->frame()->data<Header>()->last_hop_time();

            NIC::Timer::Offset adj = buf->frame()->data<Header>()->last_hop_time() - (buf->sfd_time_stamp + NIC::Timer::us2count(IEEE802_15_4::SHR_SIZE * 1000000 / IEEE802_15_4::BYTE_RATE));

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
        if(buf->destined_to_me || (buf->my_distance < buf->sender_distance)) {

            // Forward or ACK the message

            Buffer * send_buf = TSTP::alloc(buf->size());

            // Copy frame contents
            memcpy(send_buf->frame(), buf->frame(), buf->size());

            // Copy Buffer Metainformation
            send_buf->size(buf->size());
            send_buf->id = buf->id;
            send_buf->destined_to_me = buf->destined_to_me;
            send_buf->downlink = buf->downlink;
            send_buf->deadline = buf->deadline;
            send_buf->my_distance = buf->my_distance;
            send_buf->sender_distance = buf->sender_distance;
            send_buf->is_new = false;
            send_buf->is_microframe = false;

            // Calculate offset
            offset(send_buf);

            // Adjust Last Hop location
            send_buf->frame()->data<Header>()->last_hop(TSTP::here());
            send_buf->sender_distance = send_buf->my_distance;

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
Cipher TSTP::Security::_cipher;
TSTP::Node_ID TSTP::Security::_id;
TSTP::Auth TSTP::Security::_auth;
Diffie_Hellman TSTP::Security::_dh;
TSTP::Security::Pending_Keys TSTP::Security::_pending_keys;
TSTP::Security::Peers TSTP::Security::_pending_peers;
TSTP::Security::Peers TSTP::Security::_trusted_peers;
volatile bool TSTP::Security::_peers_lock;
Thread * TSTP::Security::_key_manager;
unsigned int TSTP::Security::_dh_requests_open;

// Methods
void TSTP::Security::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(!buf->is_microframe && buf->destined_to_me) {

        switch(buf->frame()->data<Header>()->type()) {

            case CONTROL: {
                db<TSTP>(TRC) << "TSTP::Security::update(): Control message received" << endl;
                switch(buf->frame()->data<Control>()->subtype()) {

                    case DH_REQUEST: {
                        if(TSTP::here() != TSTP::sink()) {
                            DH_Request * dh_req = buf->frame()->data<DH_Request>();
                            db<TSTP>(INF) << "TSTP::Security::update(): DH_Request message received: " << *dh_req << endl;

                            //while(CPU::tsl(_peers_lock));
                            //CPU::int_disable();
                            bool valid_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next())
                                if(el->object()->valid_deploy(dh_req->origin(), TSTP::now())) {
                                    valid_peer = true;
                                    break;
                                }
                            //_peers_lock = false;
                            //CPU::int_enable();

                            if(valid_peer) {
                                db<TSTP>(TRC) << "TSTP::Security::update(): Sending DH_Response" << endl;
                                // Respond to Diffie-Hellman request
                                Buffer * resp = TSTP::alloc(sizeof(DH_Response));
                                new (resp->frame()) DH_Response(_dh.public_key());
                                TSTP::marshal(resp);
                                TSTP::_nic->send(resp);

                                // Calculate Master Secret
                                Pending_Key * pk = new (SYSTEM) Pending_Key(buf->frame()->data<DH_Request>()->key());
                                Master_Secret ms = pk->master_secret();
                                //while(CPU::tsl(_peers_lock));
                                //CPU::int_disable();
                                _pending_keys.insert(pk->link());
                                //_peers_lock = false;
                                //CPU::int_enable();

                                db<TSTP>(TRC) << "TSTP::Security::update(): Sending Auth_Request" << endl;
                                // Send Authentication Request
                                resp = TSTP::alloc(sizeof(Auth_Request));
                                new (resp->frame()) Auth_Request(_auth, otp(ms, _id));
                                TSTP::marshal(resp);
                                TSTP::_nic->send(resp);
                                db<TSTP>(TRC) << "Sent" << endl;
                            }
                        }
                    } break;

                    case DH_RESPONSE: {
                        if(_dh_requests_open) {
                            DH_Response * dh_resp = buf->frame()->data<DH_Response>();
                            db<TSTP>(INF) << "TSTP::Security::update(): DH_Response message received: " << *dh_resp << endl;

                            //while(CPU::tsl(_peers_lock));
                            //CPU::int_disable();
                            bool valid_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next())
                                if(el->object()->valid_deploy(dh_resp->origin(), TSTP::now())) {
                                    valid_peer = true;
                                    db<TSTP>(TRC) << "Valid peer found: " << *el->object() << endl;
                                    break;
                                }
                            //_peers_lock = false;
                            //CPU::int_enable();

                            if(valid_peer) {
                                _dh_requests_open--;
                                Pending_Key * pk = new (SYSTEM) Pending_Key(buf->frame()->data<DH_Response>()->key());
                                //while(CPU::tsl(_peers_lock));
                                //CPU::int_disable();
                                _pending_keys.insert(pk->link());
                                //_peers_lock = false;
                                //CPU::int_enable();
                                db<TSTP>(INF) << "TSTP::Security::update(): Inserting new Pending Key: " << *pk << endl;
                            }
                        }
                    } break;

                    case AUTH_REQUEST: {

                        Auth_Request * auth_req = buf->frame()->data<Auth_Request>();
                        db<TSTP>(INF) << "TSTP::Security::update(): Auth_Request message received: " << *auth_req << endl;

                        //while(CPU::tsl(_peers_lock));
                        //CPU::int_disable();
                        Peer * auth_peer = 0;
                        for(Peers::Element * el = _pending_peers.head(); el; el = el->next()) {
                            Peer * peer = el->object();

                            if(peer->valid_request(auth_req->auth(), auth_req->origin(), TSTP::now())) {
                                for(Pending_Keys::Element * pk_el = _pending_keys.head(); pk_el; pk_el = pk_el->next()) {
                                    Pending_Key * pk = pk_el->object();
                                    if(otp(pk->master_secret(), peer->id()) == auth_req->otp()) {
                                        peer->master_secret(pk->master_secret());
                                        _pending_peers.remove(el);
                                        _trusted_peers.insert(el);
                                        auth_peer = peer;

                                        _pending_keys.remove(pk_el);
                                        delete pk_el->object();

                                        break;
                                    }
                                }
                                if(auth_peer)
                                    break;
                            }
                        }
                        //_peers_lock = false;
                        //CPU::int_enable();

                        if(auth_peer) {
                            Auth encrypted_auth;
                            encrypt(auth_peer->auth(), auth_peer, encrypted_auth);

                            Buffer * resp = TSTP::alloc(sizeof(Auth_Granted));
                            new (resp->frame()) Auth_Granted(auth_req->origin(), encrypted_auth);
                            TSTP::marshal(resp);
                            db<TSTP>(INF) << "TSTP::Security: Sending Auth_Granted message " << resp->frame()->data<Auth_Granted>() << endl;
                            TSTP::_nic->send(resp);
                        } else
                            db<TSTP>(WRN) << "TSTP::Security::update(): No peer found" << endl;
                    } break;

                    case AUTH_GRANTED: {

                        if(TSTP::here() != TSTP::sink()) {
                            Auth_Granted * auth_grant = buf->frame()->data<Auth_Granted>();
                            db<TSTP>(INF) << "TSTP::Security::update(): Auth_Granted message received: " << *auth_grant << endl;
                            //while(CPU::tsl(_peers_lock));
                            //CPU::int_disable();
                            bool auth_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next()) {
                                Peer * peer = el->object();
                                for(Pending_Keys::Element * pk_el = _pending_keys.head(); pk_el; pk_el = pk_el->next()) {
                                    Pending_Key * pk = pk_el->object();
                                    Auth decrypted_auth;
                                    OTP key = otp(pk->master_secret(), peer->id());
                                    _cipher.decrypt(auth_grant->auth(), key, decrypted_auth);
                                    if(decrypted_auth == _auth) {
                                        _pending_peers.remove(el);
                                        _trusted_peers.insert(el);
                                        auth_peer = true;

                                        _pending_keys.remove(pk_el);
                                        delete pk_el->object();

                                        break;
                                    }
                                }
                                if(auth_peer)
                                    break;
                            }
                            //_peers_lock = false;
                            //CPU::int_enable();
                        }
                    } break;
                }
            }
        }
    }
}

void TSTP::Security::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::marshal(buf=" << buf << ")" << endl;
}

TSTP::Security::~Security()
{
    db<TSTP>(TRC) << "TSTP::~Security()" << endl;
    TSTP::_nic->detach(this, 0);
    if(_key_manager)
        delete _key_manager;
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

    if(!buf->destined_to_me) {
        //buf->freed = true; // TODO
        //_nic->free(buf);
        return;
    }

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
                if(interest->region().contains(responsive->origin(), now()))
                    notify(responsive, buf);
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
    case CONTROL:
        switch(buf->frame()->data<Control>()->subtype()) {
            case DH_REQUEST:
                db<TSTP>(INF) << "TSTP::update: DH_Request: " << *buf->frame()->data<DH_Request>() << endl;
                break;
            case DH_RESPONSE:
                db<TSTP>(INF) << "TSTP::update: DH_Response: " << *buf->frame()->data<DH_Response>() << endl;
                break;
            case AUTH_REQUEST:
                db<TSTP>(INF) << "TSTP::update: Auth_Request: " << *buf->frame()->data<Auth_Request>() << endl;
                break;
            case AUTH_GRANTED:
                db<TSTP>(INF) << "TSTP::update: Auth_Granted: " << *buf->frame()->data<Auth_Granted>() << endl;
                break;
        }
    }

    //buf->freed = true; // TODO
    //_nic->free(buf);
}

__END_SYS

#endif
