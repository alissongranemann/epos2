#include <utility/malloc.h>
#include <tstp.h>
#include <alarm.h>

__BEGIN_SYS

void TSTP::process(TSTP_Common::Interest * i)
{
//    for(auto el = sensors.search_key(i->unit()); el; el = el->next()) {
//        auto sensor = el->object();
//        db<TSTP>(TRC) << "Found sensor " << (*sensor) << endl;
//        if((sensor->period() <= i->period) and (sensor->precision() <= i->precision)) {
//            subscribe(sensor, i);
//        }
//    }
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
//
//void TSTP::send_data(Sensor * s)
//{
//    //db<TSTP>(TRC) << "TSTP::send_data(" << (*s) << ")" << endl;
//
//    //NIC * nic = &(instance->_nic);
//    //auto buffer = nic->alloc(nic, nic->broadcast(), Traits<TSTP>::PROTOCOL_ID, 0, 0, sizeof(Data_Msg));
//    //if(!buffer) {
//    //    db<TSTP>(WRN) << "TSTP::send_data(" << (*s) << ") : Didn't get buffer from NIC, aborting!" << endl;
//    //    return;
//    //}
//
//    //auto payload = buffer->frame()->data<Data_Msg>();
//    //new (payload) Data_Msg(*s);
//
//    //nic->send(buffer);
//}

//void TSTP::subscribe(Sensor * s, TSTP_Common::Interest * i)
//{
//    db<TSTP>(TRC) << "TSTP::subscribe(" << *s << "," << *i << ")" << endl;
//    //new Alarm(period, new Functor_Handler<Sensor>(&send_data, s), Alarm::INFINITE);
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
