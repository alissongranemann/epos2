#include <tstp.h>
#include <gpio.h>
#include <timer.h>
#include <tsc.h>

__USING_SYS

TSTP_MAC::Message_ID TSTP_MAC::_receiving_data_id;
TSTP * TSTP_MAC::_tstp;
TSTP_MAC::Statistics TSTP_MAC::_statistics;
TSTP_MAC::TX_Schedule TSTP_MAC::_tx_schedule;
TSTP_MAC::TX_Schedule::TX_Schedule_Entry * TSTP_MAC::_tx_pending_data;
TSTP_MAC::Buffer * TSTP_MAC::_tx_pending_mf_buffer;
TSTP_MAC::Microframe * TSTP_MAC::_tx_pending_mf;
NIC TSTP_MAC::_radio;
TSTP_MAC::Address TSTP_MAC::_address(Traits<TSTP_MAC>::ADDRESS_X, Traits<TSTP_MAC>::ADDRESS_Y, Traits<TSTP_MAC>::ADDRESS_Z);
TSTP_MAC::Address TSTP_MAC::_sink_address(0,0,0);
Traits<TSTP_MAC>::Timer TSTP_MAC::_timer;

bool TSTP_MAC::send(const Interest * interest)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::send(" << interest << ")" << endl;
    auto buffer = _radio.alloc(&_radio, sizeof(Interest_Message));
    if(buffer) {
        auto msg = new (buffer->frame()->data<Interest_Message>()) Interest_Message(*interest);
        msg->header()->origin_time(time_now());
        _tx_schedule.insert(true, id(msg), time_now(), backoff(), interest->t0(), interest->region().center, buffer);
        return true;
    }
    return false;
}

bool TSTP_MAC::send(const Unit & unit, const Data & data, const Time & deadline, const Time & when)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::send(" << data << ")" << endl;
    auto buffer = _radio.alloc(&_radio, sizeof(Data_Message));
    if(buffer) {
        auto msg = new (buffer->frame()->data<Data_Message>()) Data_Message(unit, data);
        msg->header()->origin_time(time_now());
        _tx_schedule.insert(true, id(msg), when, backoff(), deadline, _sink_address, buffer);
        return true;
    }
    return false;
}

void TSTP_MAC::TX_Schedule::TX_Schedule_Entry::free()
{
    //db<TSTP_MAC>(TRC) << "TX_Schedule_Entry::free() : " << this << endl;
    if(_buffer and _buffer->nic()) {
        _buffer->nic()->free(_buffer); 
    }
}

void TSTP_MAC::init()
{
    new (&_radio) NIC();
    MAC_Timer::config();
    MAC_Timer::enable();
    _radio.off();
    timeout(Traits<TSTP_MAC>::SLEEP_PERIOD, check_tx_schedule);
    _timer.enable();
}

void TSTP_MAC::check_tx_schedule(const unsigned int & int_id)
{
    _radio.off();
    _timer.clear_interrupt();
    _tstp->check_tx_schedule();
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::check_tx_schedule(" << int_id << ")" << endl;
    if((_tx_pending_data = _tx_schedule.tx_pending(time_now()))) {
        timeout(_tx_pending_data->backoff(), cca);
        db<TSTP_MAC>(TRC) << "TSTP_MAC::backing off " << _tx_pending_data->backoff() << endl;
    } else {
        timeout(Traits<TSTP_MAC>::SLEEP_PERIOD, rx_mf);
        db<TSTP_MAC>(TRC) << "TSTP_MAC::sleeping S" << endl;
    }
}

TSTP_MAC::Time TSTP_MAC::time_now()
{
    auto ret = MAC_Timer::ts_to_us(MAC_Timer::read());
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::time_now() : " << ret << endl;
    return ret;
}

void TSTP_MAC::timeout(Time time, const Timer_Handler & handler) 
{ 
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::timeout(t=" << time << ")" << endl;
    _timer.disable(); 
    _timer.set(time);
    _timer.handler(handler);
    _timer.enable(); 
}

void TSTP_MAC::clear_timeout() 
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::clear_timeout()" << endl;
    _timer.disable(); 
}

void TSTP_MAC::cca(const unsigned int & int_id)
{
    _timer.clear_interrupt();
    db<TSTP_MAC>(TRC) << "TSTP_MAC::cca(" << int_id << ")" << endl;
    if(_radio.channel_busy()) {
        rx_mf();
    }
    else {
        prepare_tx_mf();
    }
}

bool TSTP_MAC::all_listen(Frame * f)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::all_listen(" << f << ")" << endl;
    return f->data<Header>()->message_type() == Message_Type::INTEREST;
}

bool TSTP_MAC::is_ack(TX_Schedule::TX_Schedule_Entry * e)
{
    bool ret;
    if(e->is_new()) {
        ret = false;
    } else if(auto interest = to_interest(e->buffer()->frame())) {
        ret = interest->region().contains(_address) and (interest->response_mode() == RESPONSE_MODE::SINGLE);
    } else {
        if(e->destination() == _sink_address) {
            ret = is_sink();
        } else {
            ret = e->destination().contains(_address);
        }
    }
    db<TSTP_MAC>(TRC) << "TSTP_MAC::is_ack(" << e << ") => " << ret << endl;
    return ret;
}

void TSTP_MAC::prepare_tx_mf()
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf()" << endl;
    _radio.off();
    timeout(Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES - Traits<TSTP_MAC>::Tu - 3, tx_mf);
    _tx_pending_mf_buffer = _radio.alloc(&_radio, sizeof(Microframe));
    if(_tx_pending_mf_buffer) {
        if(auto interest = to_interest(_tx_pending_data->buffer()->frame())) {
            _tx_pending_mf = new (_tx_pending_mf_buffer->frame()->data<Microframe>()) Microframe(true, _address - interest->region().center, _tx_pending_data->id());
        } else {
            _tx_pending_mf = new (_tx_pending_mf_buffer->frame()->data<Microframe>()) Microframe(true, _address - _sink_address, _tx_pending_data->id());
        }

        db<TSTP_MAC>(TRC) << "SENDING: {all=" << _tx_pending_mf->_all_listen << " ,c=" << _tx_pending_mf->_count << " ,lhd=" << _tx_pending_mf->_last_hop_distance  << " ,id=" << _tx_pending_mf->_id  << "}" << endl;
    }
    else {
        _tx_schedule.update_timeout(_tx_pending_data, time_now() + Traits<TSTP_MAC>::DATA_ACK_TIMEOUT);
        check_tx_schedule();
    }
}

void TSTP_MAC::tx_mf(const unsigned int & int_id)
{
    _timer.clear_interrupt();
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_mf(" << int_id << ")" << endl;
    unsigned int count = _tx_pending_mf->count();
    if(count > 0) {
        // It is very important to get the timing correct here, because any error will potentially 
        // be multiplied by hundreds and cause the receiver to wake up before the data is being transmited.
        // 6us is the measured time that this code takes
        timeout(Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES + Traits<TSTP_MAC>::MICROFRAME_TIME - 6, tx_mf);
        _radio.send(_tx_pending_mf_buffer);
        --(*_tx_pending_mf); // Decreases count
    }
    else { // Last microframe
        if(is_ack(_tx_pending_data)) {
            timeout(Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES + Traits<TSTP_MAC>::MICROFRAME_TIME + Traits<TSTP_MAC>::Tu, check_tx_schedule);
            _tx_schedule.remove(_tx_pending_data->id());
        }
        else {
            timeout(Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES + Traits<TSTP_MAC>::MICROFRAME_TIME, tx_data);
        }
        _radio.send(_tx_pending_mf_buffer);
        _radio.free(_tx_pending_mf_buffer);
    }
}

void TSTP_MAC::tx_data(const unsigned int & int_id)
{
    _timer.clear_interrupt();
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_data(" << _tx_pending_data->buffer() << ")" << endl;
    auto buffer = _tx_pending_data->buffer();
    auto header = buffer->frame()->data<Header>();
    
    header->last_hop_address(_address);
    header->last_hop_time(time_now());

    auto bytes = _radio.send(buffer);

    if(bytes > 0) {
        _statistics.tx_payload_frames++;
    } else {
        _statistics.dropped_tx_packets++;
        _statistics.dropped_tx_bytes -= bytes;
    }

    _tx_schedule.update_timeout(_tx_pending_data, time_now() + Traits<TSTP_MAC>::DATA_ACK_TIMEOUT);
    check_tx_schedule();
}

void TSTP_MAC::rx_mf(const unsigned int & int_id)
{
    _timer.clear_interrupt();
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_mf(" << int_id << ")" << endl;
    timeout(Traits<TSTP_MAC>::RX_MF_TIMEOUT, check_tx_schedule);
    _radio.receive(process_mf);
}

TSTP_MAC::Microframe * TSTP_MAC::to_microframe(Buffer * b)
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::to_microframe(" << b << ")" << endl;
    //db<TSTP_MAC>(TRC) << b->size() << " " << sizeof(Microframe) << endl;
	if(b->size() == sizeof(Microframe)) {
        return b->frame()->data<Microframe>();
    }
    return 0;
}

//TSTP_MAC::Payload * TSTP_MAC::to_payload(Buffer * b)
//{
//    db<TSTP_MAC>(TRC) << "TSTP_MAC::to_frame(" << b << ")" << endl;
//    db<TSTP_MAC>(TRC) << "size = " << b->size() << endl;
//    if(b->size() > sizeof(Microframe)) {
//        auto pld = b->frame()->payload();
//        unsigned int type = pld->header()->message_type();
//        db<TSTP_MAC>(TRC) << "type = " << type << endl;
//        if(type < 7 and type != 3) {
//            return reinterpret_cast<Payload*>(b->frame());
//        }
//    }
//    return 0;
//}

bool TSTP_MAC::relevant(Microframe * mf)
{
    if(mf->all_listen()) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::relevant(" << mf << ") : true" << endl;
        return true;
    }
    auto my_distance_to_sink = _sink_address - _address;
    bool ret = my_distance_to_sink < mf->last_hop_distance();
    db<TSTP_MAC>(TRC) << "TSTP_MAC::relevant(" << mf << ") : false" << endl;
    return ret;
}

TSTP_MAC::Time TSTP_MAC::time_until_data(Microframe * mf)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::time_until_data(" << mf << ")" << endl;
    return Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES + mf->count() * (Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES + Traits<TSTP_MAC>::MICROFRAME_TIME) - Traits<TSTP_MAC>::DATA_LISTEN_MARGIN;
}

TSTP_MAC::Time TSTP_MAC::backoff(const Address & destination, const Distance & last_hop_distance)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::backoff(" << destination << "," << last_hop_distance << ")" << endl;
    auto D = _address - destination;
    auto Dmsg = last_hop_distance;
    auto R = Traits<TSTP_MAC>::RADIO_RADIUS;
    auto g = Traits<TSTP_MAC>::G;
    auto S = Traits<TSTP_MAC>::SLEEP_PERIOD;
    auto ret = ((Math::abs(D - (Dmsg - R))*S) / (g*R)) * g;
    //kout << "D = " << D << endl;
    //kout << "Dmsg = " << Dmsg << endl;
    //kout << "R = " << R << endl; 
    //kout << "g = "<< g << endl;
    //kout << "S = "<< S << endl;
    //kout << "ret = "<< ret << endl; 
    return ret;
}

TSTP_MAC::Time TSTP_MAC::backoff()
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::backoff()" << endl;
    return (Random::random() % (Traits<TSTP_MAC>::SLEEP_PERIOD / Traits<TSTP_MAC>::G)) * Traits<TSTP_MAC>::G;
}

void TSTP_MAC::process_mf(Buffer * b)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::process_mf(" << b << ")" << endl;

    if(auto mf = to_microframe(b)) {
        clear_timeout();
        _radio.off();
        auto time = time_until_data(mf);
        if(_tx_pending_data and _tx_pending_data->id() == mf->id() and is_ack(_tx_pending_data)) {
            timeout(time + Traits<TSTP_MAC>::DATA_SKIP_TIME, check_tx_schedule);
        } else {
            auto removed = _tx_schedule.remove_not_ack(mf->id());
            if(not removed and relevant(mf)) {
                _receiving_data_id = mf->id();
                timeout(time, rx_data);
                db<TSTP_MAC>(TRC) << "Time until data: " << time << endl;
            } else {
                timeout(time + Traits<TSTP_MAC>::SLEEP_PERIOD, check_tx_schedule);
            }
        }

        db<TSTP_MAC>(TRC) << "{all=" << mf->_all_listen << " ,c=" << mf->_count << " ,lhd=" << mf->_last_hop_distance  << " ,id=" << mf->_id  << "}" << endl;
    }


    //if(auto mf = to_microframe(b)) {
    //    clear_timeout();
    //    _radio.off();
    //    auto time = time_until_data(mf);
    //    if(_tx_pending_data) {
    //        if(is_ack(_tx_pending_data)) {
    //            auto removed = _tx_schedule.remove(mf->id());
    //            if(not removed and relevant(mf)) {
    //                _receiving_data_id = mf->id();
    //                timeout(time, rx_data);
    //                db<TSTP_MAC>(TRC) << "Time until data: " << time << endl;
    //            } else {
    //                timeout(time + Traits<TSTP_MAC>::SLEEP_PERIOD, check_tx_schedule);
    //            }
    //        }
    //        if(_tx_pending_data->id() == mf->id()) {
    //            timeout(time + Traits<TSTP_MAC>::SLEEP_PERIOD, check_tx_schedule);
    //        } else {
    //            timeout(time + Traits<TSTP_MAC>::DATA_SKIP_TIME, check_tx_schedule);
    //        }
    //        _tx_schedule.remove(mf->id());
    //    } else {
    //        auto removed = _tx_schedule.remove(mf->id());
    //        if(not removed and relevant(mf)) {
    //            _receiving_data_id = mf->id();
    //            timeout(time, rx_data);
    //            db<TSTP_MAC>(TRC) << "Time until data: " << time << endl;
    //        } else {
    //            timeout(time + Traits<TSTP_MAC>::SLEEP_PERIOD, check_tx_schedule);
    //        }
    //    }
    //    db<TSTP_MAC>(TRC) << "{all=" << mf->_all_listen << " ,c=" << mf->_count << " ,lhd=" << mf->_last_hop_distance  << " ,id=" << mf->_id  << "}" << endl;
    //}

    _radio.free(b);
}

bool TSTP_MAC::should_forward(Data_Message * d)
{
    if(is_sink()) {
        // Should transmit ACK
        return true;
    }

    auto src = d->header()->last_hop_address();
    auto dest = _sink_address;
    auto my_distance = _address - dest;
    auto their_distance = src - dest;
    auto ret = my_distance < their_distance;
    db<TSTP_MAC>(TRC) << "TSTP_MAC::should_forward(" << d << ") : " << ret << endl;
    return ret;
}

bool TSTP_MAC::should_forward(Interest_Message * i)
{
    if(i->region().contains(_address)) {
        // Should transmit ACK
        return true;
    }

    auto src = i->header()->last_hop_address();
    auto dest = i->region().center;
    auto my_distance = _address - dest;
    auto their_distance = src - dest;
    auto ret = my_distance < their_distance;
    db<TSTP_MAC>(TRC) << "TSTP_MAC::should_forward(" << i << ") : " << ret << endl;
    return ret;
}

void TSTP_MAC::process_data(Data_Message * data)
{
    _radio.off();
    clear_timeout();
    db<TSTP_MAC>(TRC) << "TSTP_MAC::process_data(data=" << data << ")" << endl;
    if(should_forward(data)) {
        // Copy RX Buffer to TX Buffer
        auto tx_buf = _radio.alloc(&_radio, sizeof(Data_Message));
        if(tx_buf) {
            new (tx_buf->frame()->data<Data_Message>()) Data_Message(*data);
            auto bkf = backoff(_sink_address, data->header()->last_hop_address() - _sink_address);
            _tx_schedule.insert(false, _receiving_data_id, time_now(), bkf, time_now() + Traits<TSTP_MAC>::RETRANSMISSION_DEADLINE, _sink_address, tx_buf);
        }
    }

    // TODO: time and RSSI
    auto time = time_now();
    auto rssi = 0;
    if(is_sink()) {
        _tstp->process(time, rssi, data->header(), data->labeled_data());
    } else {
        _tstp->process(time, rssi, data->header());
    }
}

void TSTP_MAC::process_data(Interest_Message * interest)
{
    _radio.off();
    clear_timeout();

    //if(interest->header()->last_hop_address() == _sink_address)
        MAC_Timer::set(interest->header()->last_hop_time() + Traits<TSTP_MAC>::TX_UNTIL_PROCESS_DATA_DELAY);

    db<TSTP_MAC>(TRC) << "TSTP_MAC::process_data(interest=" << interest << ")" << endl;
    if(should_forward(interest)) {
        // Copy RX Buffer to TX Buffer
        auto tx_buf = _radio.alloc(&_radio, sizeof(Interest_Message));
        if(tx_buf) {
            new (tx_buf->frame()->data<Interest_Message>()) Interest_Message(*interest);
            _tx_schedule.insert(false, _receiving_data_id, time_now(), backoff(interest->region().center, interest->header()->last_hop_address() - interest->region().center), interest->t0(), interest->region().center, tx_buf);
        }
    }

    // TODO: time and RSSI
    auto time = time_now();
    auto rssi = 0;
    if(interest->region().contains(_address)) {
        _tstp->process(time, rssi, interest->header(), interest->interest(), _receiving_data_id);
    } else {
        _tstp->process(time, rssi, interest->header());
    }
}

TSTP_MAC::Interest_Message * TSTP_MAC::to_interest(Frame * f)
{
    if(f->header()->frame_length() == (sizeof(Interest_Message) + sizeof(CRC))) {
        auto ret = f->data<Interest_Message>();
        if(ret->header()->message_type() == INTEREST) {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::to_interest(" << f << ") => " << ret << endl;
            return ret;
        }
    }
    db<TSTP_MAC>(TRC) << "TSTP_MAC::to_interest(" << f << ") => " << 0 << endl;
    return 0;
}

void TSTP_MAC::parse_data(Buffer * b)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::parse_data(" << b << ")" << endl;

    db<TSTP_MAC>(TRC) <<  (b->frame()->data<Header>()->message_type()) << " ";
    db<TSTP_MAC>(TRC) << DATA << endl;
    db<TSTP_MAC>(TRC) << (b->size()) << " ";
    db<TSTP_MAC>(TRC) << sizeof(Data_Message) << endl;
    _statistics.waited_to_rx_payload++;

    bool success = false;

    switch (b->frame()->data<Header>()->message_type()) {
        case INTEREST:
            if((not is_sink()) and (b->size() == sizeof(Interest_Message))) {
                process_data(b->frame()->data<Interest_Message>());
                success = true;
            }
            break;
        case DATA:
            if(b->size() == sizeof(Data_Message)) {
                process_data(b->frame()->data<Data_Message>());
                success = true;
            }
            break;
        default:
            break;
    }

    _radio.free(b);

    if(success) {
        _statistics.rx_payload_frames++;
        check_tx_schedule();
    } else {
        _statistics.dropped_payload_frames++;
    }
}

void TSTP_MAC::rx_data(const unsigned int & int_id)
{
    _timer.clear_interrupt();

    //db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_data(" << int_id << ")" << endl;
    timeout(Traits<TSTP_MAC>::RX_DATA_TIMEOUT, check_tx_schedule);
    _radio.receive(parse_data);
}
