// EPOS 1-step static hierarchy PTPv2 Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <ptp.h>

__BEGIN_SYS

PTP::Timestamp * PTP::now(unsigned int id) {
       // TODO: this is a dummy
    return new (SYSTEM) Timestamp(id, id + 1);
}

bool PTP::synchronize_slaves(unsigned char domain_number) {
    db<PTP>(TRC) << "PTP::synchronize(this=" << this << ")" << endl;

    if (!_master) {
        db<PTP>(WRN) << "PTP::synchronize:only masters can synchronize slaves" << endl;

        return false;
    }

    Buffer * buf = _nic.alloc(&_nic, Address::BROADCAST, NIC::PTP, 0, 0, sizeof(Event_Message)); // FIXME

    if (buf == 0) {
        db<PTP>(WRN) << "PTP::synchronize:couldn't allocate any buffer" << endl;

        return false;
    }

    Event_Message * sync = buf->frame()->data<Event_Message>();

    Header h(domain_number, SYNC, 1, 2, 1, _current_sync_id++);

    memcpy(sync->header(), &h, sizeof(Header));
    kout << "size of header: " << sizeof(Header) << endl;

    t1 = now();

    sync->origin_seconds(t1->seconds());
    sync->origin_nanoseconds(t1->nanoseconds());

    _nic.send(buf);

    _c.wait();

    return true;
}

void PTP::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf) {
    db<PTP>(TRC) << "PTP::update(obs=" << obs << ",prot=" << prot << ",buf=" << buf << ")" << endl;

    Timestamp* arrival_time = now(2);

    Header* h = buf->frame()->data<Header>();

    db<PTP>(TRC) << "PTP::update: received a ";

    if (!_master) {
        if (h->type() == SYNC) {
            db<PTP>(TRC) << "SYNC" << endl;

            _current_sync_id = h->sequence_id();

            if (h->flags() & Header::TWO_STEPS) { // TODO
                db<PTP>(TRC) << "PTP::update: TWO_STEPS == true" << endl;

                return;
            }

            Event_Message * m = buf->frame()->data<Event_Message>();
            t1 = new (SYSTEM) Timestamp(m->origin_seconds(), m->origin_nanoseconds());
            t2 = arrival_time;

            Buffer * buf = _nic.alloc(&_nic, Address::BROADCAST, NIC::PTP, 0, 0, sizeof(Event_Message)); // FIXME

            if (buf == 0) {
                db<PTP>(WRN) << "PTP::synchronize:couldn't allocate any buffer" << endl;

                return;
            }

            Event_Message * req = buf->frame()->data<Event_Message>();

            Header header(h->domain_number(), DELAY_REQ, 1, 2, 1, _current_delay_req_id++);

            t3 = now(3);

            memcpy(req->header(), &header, sizeof(Header));

            req->origin_timestamp(t3);

            _nic.send(buf);

            return;
        }

        if (h->type() == DELAY_RESP) {
            db<PTP>(TRC) << "DELAY_RESP" << endl;

            Delay_Resp_Message * resp = buf->frame()->data<Delay_Resp_Message>();

            if (resp->header()->sequence_id() != (_current_delay_req_id - 1)) {
                _nic.free(buf);

                return;
            }

            t4 = resp->receive_timestamp();

            // Timestamp * clock_offset = (*(*(*t2 - *t1) + *(*t4 - *t3))) / 2;

            // TODO calibrate

            return;
        }

        return;
    }

    if (_master) {
        if (h->type() == DELAY_REQ) {
            db<PTP>(TRC) << "DELAY_REQ" << endl;

            Buffer * buf = _nic.alloc(&_nic, Address::BROADCAST, NIC::PTP, 0, 0, sizeof(Delay_Resp_Message)); // FIXME

            if (buf == 0) {
                db<PTP>(WRN) << "PTP::synchronize:couldn't allocate any buffer" << endl;

                return;
            }

            Delay_Resp_Message * resp = buf->frame()->data<Delay_Resp_Message>();

            t4 = arrival_time;

            Header header(h->domain_number(), DELAY_RESP, 1, 2, 1, h->sequence_id());

            memcpy(resp->header(), &header, sizeof(Header));

            resp->receive_timestamp(t4);

            // TODO set source port identity

            _nic.send(buf);

            _c.signal();

            return;
        }
    }

    db<PTP>(TRC) << "unknown/untreated message type" << endl;
}

__END_SYS

#endif
