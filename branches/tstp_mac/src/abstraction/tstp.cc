#include <utility/malloc.h>
#include <tstp.h>

__BEGIN_SYS

void TSTP::process(TSTP_Common::Interest * i, Header * h, Message_ID interest_id)
{
    db<TSTP>(TRC) << "TSTP::process: Interest " << *i << endl;
    for(auto el = sensors.search_key(i->unit()); el; el = el->next()) {
        auto sensor = el->object();
        db<TSTP>(TRC) << "Found sensor " << (*sensor) << endl;
        if((sensor->period() <= i->period()) and (sensor->precision() <= i->precision())) {
            subscribe(sensor, i, interest_id);
        }
    }
}

void TSTP::process(TSTP_Common::Labeled_Data * d, Header * h)
{
    db<TSTP>(TRC) << "TSTP::process: Data " << *d << " Header " << *h << endl;
    for(auto el = interests.search_key(d->unit); el; el = el->next()) {
        auto interest = el->object();
        if((interest->last_reading() < h->origin_time()) and (interest->region().contains(h->origin_address()))) {
            interest->handle(d->data, h);
            interest->last_reading(h->origin_time());
            db<TSTP>(TRC) << "Found interest " << endl;
            db<TSTP>(TRC) << "interest: " << *interest << ", received data: " << d->data << endl;
        }
    }
}

void TSTP::subscribe(Sensor * s, TSTP_Common::Interest * in, Message_ID interest_id)
{
    db<TSTP>(TRC) << "TSTP::subscribe(" << *s << "," << *in << ")" << endl;

    for(unsigned int i=0; i < Traits<TSTP>::MAX_SCHEDULED_MESSAGES; i++) {
        Scheduled_Message * msg = &message_schedule[i];
        if(not msg->free) {
            if((msg->sensor == s) and (msg->interest_id == interest_id))
                // This sensor is already subscribed to this interest
                return;
        }
    }

    for(unsigned int i=0; i < Traits<TSTP>::MAX_SCHEDULED_MESSAGES; i++) {
        Scheduled_Message * msg = &message_schedule[i];
        if(msg->free) {
            new (msg) Scheduled_Message(s, in->t0(), in->period(), in->t0() + in->dt(), interest_id);
            break;
        }
    }
}

//void TSTP::publish(const Sensor & s)
//{
//    //db<TSTP>(TRC) << "TSTP::publish(Sensor " << (&s) << ")" << endl;
//
//    //auto buffer = _nic.alloc(&_nic, _nic.broadcast(), Traits<TSTP>::PROTOCOL_ID, 0, 0, sizeof(Report_Msg));
//    //if(!buffer) {
//    //    db<TSTP>(WRN) << "TSTP::publish(Sensor " << (&s) << ") : Didn't get buffer from NIC, aborting!" << endl;
//    //    return;
//    //}
//
//    //auto payload = buffer->frame()->data<Report_Msg>();
//    //new (payload) Report_Msg(s);
//
//    //_nic.send(buffer);
//}
//
//void TSTP::publish(const Interest & in)
//{
//    //db<TSTP>(TRC) << "TSTP::publish(Interest " << in << ")" << endl;
//
//    //auto buffer = _nic.alloc(&_nic, _nic.broadcast(), Traits<TSTP>::PROTOCOL_ID, 0, 0, sizeof(Interest_Msg));
//    //if(!buffer) {
//    //    db<TSTP>(WRN) << "TSTP::publish(Interest " << in << ") : Didn't get buffer from NIC, aborting!" << endl;
//    //    return;
//    //}
//
//    //auto payload = buffer->frame()->data<Interest_Msg>();
//
//    //new (payload) Interest_Msg(in);
//
//    //_nic.send(buffer);
//}
////void TSTP::update(NIC::Observed * o, NIC::Protocol p, NIC::Buffer *b)
////{
////    //db<TSTP>(TRC) << "TSTP::update(" << o << "," << p << "," << b << ")" << endl;
////
////    //auto header = b->frame()->data<Header>();
////
////    //if(auto msg = header->morph<Interest_Msg>()) {
////    //    db<TSTP>(TRC) << "TSTP::update: Interest message " << msg->unit << endl;
////    //    for(auto el = sensors.search_key(msg->unit); el; el = el->next()) {
////    //        auto sensor = el->object();
////    //        db<TSTP>(TRC) << "Found sensor " << (*sensor) << endl;
////    //        if((sensor->period() <= msg->period) and (sensor->precision() <= msg->precision)) {
////    //            subscribe(sensor, msg->period);
////    //        }
////    //    }
////    //}
////
////    //else if(auto msg = header->morph<Data_Msg>()) {
////    //    db<TSTP>(TRC) << "TSTP::update: Data message" << endl;
////    //    for(auto el = interests.search_key(msg->unit); el; el = el->next()) {
////    //        db<TSTP>(TRC) << "Found interest " << endl;
////    //        auto interest = el->object();
////    //        auto variable = interest->_data;
////    //        db<TSTP>(TRC) << "interest: " << *interest << " variable: " << variable << " received data: " << msg->data << endl;
////    //        *variable = msg->data;
////    //    }
////    //}
////
////    //else if(auto msg = header->morph<Report_Msg>()) {
////    //    db<TSTP>(TRC) << "TSTP::update: Report message" << endl;
////    //    for(auto el = interests.search_key(msg->unit); el; el = el->next()) {
////    //        db<TSTP>(TRC) << "Found interest " << endl;
////    //        auto interest = el->object();
////    //        if((interest->period() >= msg->period) and (interest->precision() >= msg->precision)) {
////    //            publish(*interest);
////    //        }
////    //    }
////    //}
////
////    //_nic.free(b);
////}

__END_SYS
