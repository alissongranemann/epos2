#include <nic.h>
#include <timer.h>
#include <tsc.h>

__USING_SYS

TSTP_MAC::Message_ID TSTP_MAC::_receiving_data_id;
TSTP_MAC::Statistics TSTP_MAC::_statistics;
TSTP_MAC::TX_Schedule TSTP_MAC::_tx_schedule;
TSTP_MAC::TX_Schedule::TX_Schedule_Entry * TSTP_MAC::_tx_pending_data;
TSTP_MAC::Buffer * TSTP_MAC::_tx_pending_mf_buffer;
TSTP_MAC::Microframe * TSTP_MAC::_tx_pending_mf;
NIC TSTP_MAC::_radio;
TSTP_MAC::Address TSTP_MAC::_address(Traits<TSTP_MAC>::ADDRESS_X, Traits<TSTP_MAC>::ADDRESS_Y, Traits<TSTP_MAC>::ADDRESS_Z);
TSTP_MAC::Address TSTP_MAC::_sink_address(0,0,0);
Traits<TSTP_MAC>::Timer TSTP_MAC::_timer;

void TSTP_MAC::send(const Interest * interest)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::send(" << interest << ")" << endl;
    auto buffer = _radio.alloc(&_radio, sizeof(Interest));
    if(buffer) {
        auto i = new (buffer->frame()) Interest(*interest);
        db<TSTP_MAC>(TRC) << "{region=" << i->_region << ",t0=" << i->_t0 << ",dt=" << i->_dt << ",p=" << i->_period << ",u=" << i->_unit << ",pr=" << i->_precision << ",rm=" << i->_response_mode << "}" << endl;
        _tx_schedule.insert(true, id(interest), time_now(), backoff(), interest->destination(), buffer);
    }
}

void TSTP_MAC::TX_Schedule::TX_Schedule_Entry::free()
{
    //db<TSTP_MAC>(TRC) << "TX_Schedule_Entry::free() : " << this << endl;
    if(_payload and _payload->nic()) {
        _payload->nic()->free(_payload); 
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
    db<TSTP_MAC>(TRC) << "TSTP_MAC::check_tx_schedule(" << int_id << ")" << endl;
    _radio.off();
    _timer.clear_interrupt();
    if((_tx_pending_data = tx_pending())) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::backing off " << _tx_pending_data->backoff() << endl;
        timeout(_tx_pending_data->backoff(), cca);
    } else {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::sleeping S" << endl;
        timeout(Traits<TSTP_MAC>::SLEEP_PERIOD, rx_mf);
    }
}

TSTP_MAC::TX_Schedule::TX_Schedule_Entry * TSTP_MAC::tx_pending() 
{
    if(auto next = _tx_schedule.next_message()) {
        if(next->transmit_at() <= time_now()) {
            ++(*next);
            db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_pending() : " << next << endl;
            return next;
        }
    }
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_pending() : no" << endl;
    return 0;
}

TSTP_MAC::Time TSTP_MAC::time_now()
{
    auto ret = MAC_Timer::ts_to_us(MAC_Timer::read());
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::time_now() : " << ret << endl;
    return ret;
}

void TSTP_MAC::timeout(Time time, const Timer_Handler & handler) 
{ 
//    db<TSTP_MAC>(TRC) << "TSTP_MAC::timeout(t=" << time << ")" << endl;
    _timer.disable(); 
    _timer.set(time);
    _timer.handler(handler);
    _timer.enable(); 
}

void TSTP_MAC::clear_timeout() 
{ 
    db<TSTP_MAC>(TRC) << "TSTP_MAC::clear_timeout()" << endl;
    _timer.disable(); 
}

void TSTP_MAC::cca(const unsigned int & int_id)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::cca(" << int_id << ")" << endl;
    _timer.clear_interrupt();
    if(_radio.channel_busy()) {
        rx_mf();
    }
    else {
        prepare_tx_mf();
    }
}

bool TSTP_MAC::all_listen(TX_Schedule::TX_Schedule_Entry * e)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::all_listen(" << e << ")" << endl;
    return e->payload()->frame()->payload()->header()->message_type() == Message_Type::INTEREST;
}

bool TSTP_MAC::is_destination(TX_Schedule::TX_Schedule_Entry * e)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::is_destination(" << e << ")" << endl;
    bool ret = e->destination() == _address;
    db<TSTP_MAC>(TRC) << " => " << ret << endl;
    return ret;
}

TSTP_MAC::Distance TSTP_MAC::distance_to(TX_Schedule::TX_Schedule_Entry * e)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::distance_to(" << e << ")" << endl;
    auto ret = e->destination() - _address;
    db<TSTP_MAC>(TRC) << " = " << ret << endl;
    return ret;
}

void TSTP_MAC::prepare_tx_mf()
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf()" << endl;
    _radio.off();
    _radio.clear_rx();        
    timeout(Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES - Traits<TSTP_MAC>::Tu - 3, tx_mf);
    _tx_pending_mf_buffer = _radio.alloc(&_radio, sizeof(Microframe));
    if(_tx_pending_mf_buffer) {
        _tx_pending_mf = new (_tx_pending_mf_buffer->frame()) Microframe(all_listen(_tx_pending_data), Traits<TSTP_MAC>::N_MICROFRAMES - 1, distance_to(_tx_pending_data), _tx_pending_data->id());
        db<TSTP_MAC>(TRC) << "{all=" << _tx_pending_mf->_all_listen << " ,c=" << _tx_pending_mf->_count << " ,lhd=" << _tx_pending_mf->_last_hop_distance  << " ,id=" << _tx_pending_mf->_id  << " ,crc=" << _tx_pending_mf->_CRC << "}" << endl;
    }
    else {
        _tx_schedule.update_timeout(_tx_pending_data, time_now() + Traits<TSTP_MAC>::DATA_ACK_TIMEOUT);
        check_tx_schedule();
    }
}

void TSTP_MAC::tx_mf(const unsigned int & int_id)
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_mf(" << int_id << ")" << endl;
    _timer.clear_interrupt();
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
        if(is_destination(_tx_pending_data)) {
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
    db<TSTP_MAC>(TRC) << "TSTP_MAC::tx_data(" << _tx_pending_data->payload() << ")" << endl;
    auto h = _tx_pending_data->payload()->frame()->payload()->header();
    h->last_hop_address(_address);
    h->last_hop_time(time_now());
    if(_radio.send(_tx_pending_data->payload()) > 0) {
        _statistics.tx_payload_frames++;
    }
    _tx_schedule.update_timeout(_tx_pending_data, time_now() + Traits<TSTP_MAC>::DATA_ACK_TIMEOUT);
    auto i = reinterpret_cast<Interest*>(_tx_pending_data->payload()->frame());
    db<TSTP_MAC>(TRC) << "{region=" << i->_region << ",t0=" << i->_t0 << ",dt=" << i->_dt << ",p=" << i->_period << ",u=" << i->_unit << ",pr=" << i->_precision << ",rm=" << i->_response_mode << "}" << endl;
    check_tx_schedule();
}

void TSTP_MAC::rx_mf(const unsigned int & int_id)
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_mf(" << int_id << ")" << endl;
    _timer.clear_interrupt();
    timeout(Traits<TSTP_MAC>::RX_MF_TIMEOUT, check_tx_schedule);
    _radio.receive(process_mf);
}

TSTP_MAC::Microframe * TSTP_MAC::to_microframe(Buffer * b)
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::to_microframe(" << b << ")" << endl;
    //db<TSTP_MAC>(TRC) << b->size() << " " << sizeof(Microframe) << endl;
	if(b->size() == sizeof(Microframe)) {
        return b->frame()->microframe();
    }
    return 0;
}

TSTP_MAC::Payload * TSTP_MAC::to_payload(Buffer * b)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::to_frame(" << b << ")" << endl;
    db<TSTP_MAC>(TRC) << "size = " << b->size() << endl;
    if(b->size() > sizeof(Microframe)) {
        auto pld = b->frame()->payload();
        unsigned int type = pld->header()->message_type();
        db<TSTP_MAC>(TRC) << "type = " << type << endl;
        if(type < 7 and type != 3) {
            return reinterpret_cast<Payload*>(b->frame());
        }
    }
    return 0;
}

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

TSTP_MAC::Time TSTP_MAC::backoff(Payload * p)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::backoff(" << p << ")" << endl;
    auto D = _address - p->destination();
    auto Dmsg = p->header()->last_hop_address() - p->destination();
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
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::process_mf(" << b << ")" << endl;
    //kout << "TSTP_MAC::process_mf(" << b << ")" << endl;
    if(auto mf = to_microframe(b)) {
        clear_timeout();
        _radio.off();
        auto removed = _tx_schedule.remove(mf->id());
        auto time = time_until_data(mf);
        if(not removed and relevant(mf)) {
            _receiving_data_id = mf->id();
            timeout(time, rx_data);
            db<TSTP_MAC>(TRC) << "Time until data: " << time << endl;
        } else {
            timeout(time + Traits<TSTP_MAC>::DATA_SKIP_TIME, check_tx_schedule);
        }
        db<TSTP_MAC>(TRC) << "{all=" << mf->_all_listen << " ,c=" << mf->_count << " ,lhd=" << mf->_last_hop_distance  << " ,id=" << mf->_id  << " ,crc=" << mf->_CRC << "}" << endl;
    }
    _radio.free(b);
}

bool TSTP_MAC::should_forward(Payload * p)
{
    auto src = p->header()->last_hop_address();
    auto dest = p->destination();
    auto my_distance = _address - dest;
    auto their_distance = src - dest;
    auto ret = my_distance < their_distance;
    db<TSTP_MAC>(TRC) << "TSTP_MAC::should_forward(" << p << ") : " << ret << endl;
    return ret;
}

void TSTP_MAC::process_data(Buffer * b)
{
    _statistics.waited_to_rx_payload++;
    db<TSTP_MAC>(TRC) << "TSTP_MAC::process_data(" << b << ")" << endl;
    if(auto payload = to_payload(b)) {
        if(payload->header()->message_type() == INTEREST) {
            auto i = reinterpret_cast<Interest*>(payload);            
            db<TSTP_MAC>(TRC) << "{region=" << i->_region << ",t0=" << i->_t0 << ",dt=" << i->_dt << ",p=" << i->_period << ",u=" << i->_unit << ",pr=" << i->_precision << ",rm=" << i->_response_mode << "}" << endl;
        }
        _radio.off();
        clear_timeout();
        if(should_forward(payload)) {            
            // Copy RX Buffer to TX Buffer
            auto tx_buf = _radio.alloc(&_radio, b->size());
            if(tx_buf) {
                new (tx_buf->frame()) Frame(b->frame()->data<char>(), b->size());
                _tx_schedule.insert(false, _receiving_data_id, time_now(), backoff(payload), payload->destination(), tx_buf);
            }
        }
        _radio.free(b); // TODO
        _statistics.rx_payload_frames++;
        //_tstp->data_received(b); //TODO
        check_tx_schedule();
    } else {
//        static int dropped_data;
//        kout << "Dropped data!" << (++dropped_data) << endl;
//        while(true);
        _statistics.dropped_payload_frames++;
        _radio.free(b); // TODO
    }
}

void TSTP_MAC::rx_data(const unsigned int & int_id)
{
    //db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_data(" << int_id << ")" << endl;
    _timer.clear_interrupt();
    timeout(Traits<TSTP_MAC>::RX_DATA_TIMEOUT, check_tx_schedule);
    _radio.receive(process_data);
}
