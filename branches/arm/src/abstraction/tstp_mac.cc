#include <tstp.h>
#include <ic.h>

__BEGIN_SYS

// Common MAC definitions
TSTP::MAC::MACS TSTP::MAC::_macs[TSTP::MAC::UNITS];

TSTP::MAC::Buffer * TSTP::MAC::alloc(unsigned int size) {
    db<TSTP::MAC>(TRC) << "TSTP::MAC::alloc(size=" << size << ")" << endl;

    // Wait for the next buffer to become free and seize it
    unsigned int i = 1;
    for(bool locked = false; !locked; i++) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur %= TX_BUFS;
        if(i >= 2*TX_BUFS) {
            db<TSTP::MAC>(WRN) << "TSTP::MAC::alloc: No buffer available! Dropping packet" << endl;
            return 0; // No buffer found
        }
    }
    Buffer * buf = _tx_buffer[_tx_cur];

    new (buf) Buffer(_tstp, size);

    db<TSTP::MAC>(INF) << "TSTP::MAC::alloc[" << _tx_cur << "] => " << buf << endl;

    ++_tx_cur %= TX_BUFS;

    return buf;
}

TSTP::MAC::Buffer * TSTP::MAC::alloc(unsigned int size, Frame * f) {
    db<TSTP::MAC>(TRC) << "TSTP::MAC::alloc(size=" << size << ")" << endl;

    // Wait for the next buffer to become free and seize it
    unsigned int i = 1;
    for(bool locked = false; !locked; i++) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur %= TX_BUFS;
        if(i >= 2*TX_BUFS) {
            db<TSTP::MAC>(WRN) << "TSTP::MAC::alloc: No buffer available! Dropping packet" << endl;
            return 0; // No buffer found
        }
    }
    Buffer * buf = _tx_buffer[_tx_cur];

    new (buf) Buffer(_tstp, size);
    memcpy(buf->frame(), f, size);

    db<TSTP::MAC>(INF) << "TSTP::MAC::alloc[" << _tx_cur << "] => " << buf << endl;

    ++_tx_cur %= TX_BUFS;

    return buf;
}

// TSTP_MAC definitions
void TSTP_MAC::State::schedule(const Time & when, Handler * what) {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::State::schedule(when=" << when << ", what=" << what << ")" << endl;
    auto evt = new (&_event) Event(when, what);
    _mac->tstp()->schedule(evt);
}

void TSTP_MAC::State::unschedule() {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::State::unschedule()" << endl;
    _mac->tstp()->unschedule(&_event);
}

void TSTP_MAC::State::schedule(const Time & when, Handler * what, const Time & period, const Time & end) {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::State::schedule(when=" << when << ", what=" << what << ", per=" << period << ", end=" << end << ")" << endl;
    auto evt = new (&_event) Event(when, what, period, end);
    _mac->tstp()->schedule(evt);
}

void TSTP_MAC::send(Buffer * buf)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::send()" << endl;
    buf->tx_link()->rank(buf->frame()->as<TSTP_API::Header>()->deadline());
    buf->set_id();
    auto old_size = _tx_ready_schedule.size();
    _tx_ready_schedule.insert(buf->tx_link());
    auto new_size = _tx_ready_schedule.size();
    db<TSTP_MAC>(TRC) << "TSTP_MAC::send() : _tx_ready_schedule size : " << old_size << " => " << new_size << endl;
}

void TSTP_MAC::check_tx_schedule()
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::check_tx_schedule()" << endl;
    _rx_pin.clear();
    _tx_pin.clear();
    _phy->off();
    auto t = _tstp->time();
    bool new_ready_frame = false;
    for(auto head = _tx_later_schedule.head(); head and (head->rank() <= t); head = _tx_later_schedule.head()) {
        _tx_later_schedule.remove_head();
        auto deadline = head->object()->frame()->deadline();
        if((deadline > t) and ((deadline - t) > Config::PERIOD)) {
            new_ready_frame = true;
            head->rank(head->object()->frame()->deadline());
            _tx_ready_schedule.insert(head);
        } else {
            free(head->object());
        }
    }
    // Channel was silent in the last round, this means that no one is trying to forward my message. Retransmit.
    if(not new_ready_frame and _channel_silent and _tx_later_schedule.head()) {
        auto head = _tx_later_schedule.remove_head();
        head->rank(head->object()->frame()->deadline());
        _tx_ready_schedule.insert(head);
    }
    _channel_silent = false;
    auto head = _tx_ready_schedule.head();
    for(; head and (head->rank() != 0) and ((head->rank() <= t) or ((head->rank() - t) <= Config::PERIOD)); _tx_ready_schedule.remove_head(), free(head->object()), head = _tx_ready_schedule.head());
    _tx_pending = head;
    if(head) {
        Time backoff;
        if(head->rank() == 0) {
            auto id = head->object()->id();
            if(id != _last_fwd_id) {
                _last_backoff = tstp()->_router->backoff(reinterpret_cast<char *>(head->object()->frame()));
                _last_fwd_id = id;
            }
            backoff = _last_backoff;
        } else {
            backoff = tstp()->_router->backoff(reinterpret_cast<char *>(head->object()->frame()));
        }
        _cca.schedule(t + backoff);
    } else {
        _rx_mf.schedule(t + Config::SLEEP_PERIOD);
    }
}

void TSTP_MAC::cca() {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::cca()" << endl;
    auto t0 = _tstp->time();
    bool cca = true;
    _phy->start_cca();
    while((not _phy->cca_valid()) and ((_tstp->time() - t0) < Config::CCA_TIME));
    while(cca and ((_tstp->time() - t0) < Config::CCA_TIME)) {
        cca = _phy->cca();
    }
    _phy->end_cca();
    if(cca) {
        prepare_tx_mf();
    } else {
        rx_mf();
    }
}

void TSTP_MAC::prepare_tx_mf() {
    _phy->off();
    _tx_pin.clear();
    _rx_pin.clear();
    auto time = tstp()->time();
    db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf()" << endl;
    _sending_microframe = alloc(sizeof(Microframe));
    if(_sending_microframe) {
        const auto period = Config::TIME_BETWEEN_MICROFRAMES + Config::MICROFRAME_TIME;
        auto t = _tx_pending->object()->frame()->as<Header>()->message_type();
        if(t == MESSAGE_TYPE::INTEREST) {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf() : interest message" << endl;
            _acking = is_ack(_tx_pending);
            new (_sending_microframe->frame()->as<Microframe>()) Microframe(!_acking, _tx_pending->object()->id(), Config::N_MICROFRAMES, 0);
            _tx.schedule(time + period, period, time + ((2 * Config::N_MICROFRAMES + 10) * period)); // More time than necessary. Will unschedule itself in tx() when done
        }
        else if(t == MESSAGE_TYPE::DATA) {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf() : data message" << endl;
            _acking = is_ack(_tx_pending);
            new (_sending_microframe->frame()->as<Microframe>()) Microframe(false, _tx_pending->object()->id(), Config::N_MICROFRAMES, tstp()->_router->hint(tstp()->_router->sink_address()));
            _tx.schedule(time + period, period, time + ((2 * Config::N_MICROFRAMES + 10) * period)); // More time than necessary. Will unschedule itself in tx() when done
        } // TODO: BOOTSTRAP messages
        else {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf() : can't recognize frame type " << t << endl;
            free(_sending_microframe);
            _sending_microframe = 0;
            _tx_ready_schedule.remove(_tx_pending);
            _tx_pending = 0;
            check_tx_schedule();
        }
    } else {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::prepare_tx_mf() : failed to alloc microframe!" << endl;
        check_tx_schedule();
    }
}

void TSTP_MAC::tx() {
    auto old_count = _sending_microframe->frame()->as<Microframe>()->dec_count();
    if(Traits<TSTP_MAC>::hysterically_debugged) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::tx() : " << old_count << endl;
    }
    if(old_count >= 1) {
        send_mf(_sending_microframe);
    } else {
        if(not _acking) {
            send_frame(_tx_pending->object());
        }
        _phy->off();
        _tx.unschedule();
        free(_sending_microframe);
        _sending_microframe = 0;
        _tx_ready_schedule.remove(_tx_pending);
        auto t = tstp()->time();
        if(_acking) {
            free(_tx_pending->object());
        } else {
            auto deadline = _tx_pending->object()->frame()->header()->deadline();
            Time adjust;
            if((t < deadline) and ((adjust = ((deadline - t) / 2)) > Config::PERIOD)) {
                _tx_pending->rank(t + adjust);
                _tx_later_schedule.insert(_tx_pending);
            } else {
                free(_tx_pending->object());
            }
        }
        _tx_pending = 0;
        _rx_mf.schedule(t + Config::SLEEP_PERIOD);
    }
}

void TSTP_MAC::rx_mf() {
    if(Traits<TSTP_MAC>::hysterically_debugged) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_mf()" << endl;
    }
    _rx_state = RX_MF; 
    _channel_silent = true;
    _check_tx_schedule.schedule(tstp()->time() + Config::RX_MF_TIMEOUT);
    _rx_pin.set();
    _phy->rx(); 
}

void TSTP_MAC::rx_data() {
    if(Traits<TSTP_MAC>::hysterically_debugged) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::rx_data()" << endl;
    }
    _rx_state = RX_DATA; 
    _check_tx_schedule.schedule(tstp()->time() + Config::RX_DATA_TIMEOUT);
    _rx_pin.set();
    _phy->rx(); 
}

void TSTP_MAC::update(Buffer * buf)
{
    if(Traits<TSTP_MAC>::hysterically_debugged) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::update()" << endl;
    }
    if(_rx_state == RX_MF) {
        process_mf(buf);
    } else if(_rx_state == RX_DATA) {
        process_data(buf);
    }
}

void TSTP_MAC::process_mf(Buffer * buf)
{
    if(buf->size() == sizeof(Microframe)) {        
        _rx_pin.clear();
        _tx_pin.clear();
        _phy->off();
        _check_tx_schedule.unschedule();
        _channel_silent = false;
        db<TSTP_MAC>(TRC) << "TSTP_MAC::process_mf()" << endl;
        auto mf = buf->frame()->as<Microframe>();

        auto data_time = tstp()->_time->last_sfd() + (mf->count() + 1) * (Config::TIME_BETWEEN_MICROFRAMES + Config::MICROFRAME_TIME);

        TX_Schedule::Element * next;
        for(auto el = _tx_ready_schedule.head(); el; el = next) {
            next = el->next();
            if(el->object()->id() == mf->id()) {
                _tx_ready_schedule.remove(el);
                free(el->object());
            }
        }
        for(auto el = _tx_later_schedule.head(); el; el = next) {
            next = el->next();
            if(el->object()->id() == mf->id()) {
                _tx_later_schedule.remove(el);
                free(el->object());
            }
        }

        if(mf->all_listen() or tstp()->_router->relevant(mf->hint())) {
            _receiving_data_id = mf->id();
            _rx_data.schedule(data_time - Config::DATA_LISTEN_MARGIN);
        } else {
            _check_tx_schedule.schedule(data_time + Config::SLEEP_PERIOD);
        }
    }
    free(buf);
}

bool TSTP_MAC::is_ack(TX_Schedule::Element * el) {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::is_ack()" << endl;
    auto header = el->object()->frame()->as<Header>();
    if(header->origin_address() == tstp()->address()) {
        return false;
    } else {
        auto t = header->message_type();
        if(t == MESSAGE_TYPE::INTEREST) {
            return tstp()->_router->accept(el->object()->frame()->as<Interest_Message>()->destination());
        } else {
            return Traits<TSTP>::is_sink;
        } // TODO: other types of message
    }
}

void TSTP_MAC::process_data(Buffer * buf)
{
    db<TSTP_MAC>(TRC) << "TSTP_MAC::process_data(buf=" << buf << ")" << endl;
    auto t = buf->frame()->as<Header>()->message_type();
    db<TSTP_MAC>(TRC) << "Message type = " << t << endl;
    bool success = false;

    if((not Traits<TSTP>::is_sink) and (t == MESSAGE_TYPE::INTEREST) and (buf->size() == sizeof(Interest_Message)))
    {
        _check_tx_schedule.unschedule();
        _rx_pin.clear();
        _tx_pin.clear();
        _phy->off();
        buf->id(_receiving_data_id);
        success = true;
        auto interest = buf->frame()->as<Interest_Message>();
        bool for_me = _tstp->_router->accept(interest->destination());
        bool should_forward = for_me or _tstp->_router->should_forward(reinterpret_cast<char*>(interest), interest->destination()); // if message is for me, I should send an ack
        if(should_forward) {
            // Copy RX Buffer to TX Buffer
            auto tx_buf = alloc(sizeof(Interest_Message), reinterpret_cast<Frame*>(interest));
            if(tx_buf) {
                tx_buf->id(buf->id());
                tx_buf->tx_link()->rank(0); // forwardings / acks have priority
                _tx_ready_schedule.insert(tx_buf->tx_link());
            }
        }
        _tstp->update(interest, for_me);
    }
    else if((t == MESSAGE_TYPE::DATA) and (buf->size() <= sizeof(Data_Message)) and ((buf->size() > sizeof(Data_Message) - MAX_DATA_SIZE))) 
    {
        _check_tx_schedule.unschedule();
        _rx_pin.clear();
        _tx_pin.clear();
        _phy->off();
        buf->id(_receiving_data_id);
        success = true;
        auto data = buf->frame()->as<Data_Message>();
        const bool for_me = Traits<TSTP>::is_sink;
        bool should_forward = for_me or tstp()->_router->should_forward(reinterpret_cast<char*>(data), tstp()->_router->sink_address()); // if message is for me, I should send an ack
        if(should_forward) {
            // Copy RX Buffer to TX Buffer
            auto tx_buf = alloc(buf->size(), reinterpret_cast<Frame*>(data));
            if(tx_buf) {
                tx_buf->id(buf->id());
                tx_buf->tx_link()->rank(0); // forwardings / acks have priority
                _tx_ready_schedule.insert(tx_buf->tx_link());
            }
        }
        _tstp->update(data, buf->size(), for_me);
    }

    free(buf);

    if(success) {
        check_tx_schedule();
    }
}

// One_Hop_MAC definitions
void One_Hop_MAC::send(Buffer * buf) { // TODO
    buf->frame()->header()->last_hop_address(_tstp->address());
    send_frame<PHY_Layer>(buf);
    free(buf);
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::send() : Sent header = " << buf->frame()->header() << endl;
    db<One_Hop_MAC>(TRC) << "size = " << buf->size() << endl;
}

void One_Hop_MAC::update(Buffer * buf) {
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::update(buf=" << buf << ")" << endl;
    auto t = buf->frame()->message_type();
    db<One_Hop_MAC>(TRC) << "Message type = " << t << endl;
    if(t == MESSAGE_TYPE::INTEREST) {
        if(!Traits<TSTP>::is_sink) {
            auto msg = buf->frame()->as<Interest_Message>();
            db<One_Hop_MAC>(TRC) << "Interest_Message = " << msg << endl;
            if(_tstp->_router->accept(msg->destination())) {
                _tstp->update(msg);
            }
        }
    } else if (t == MESSAGE_TYPE::DATA) {
        if(Traits<TSTP>::is_sink) {
            _tstp->update(buf->frame()->as<Data_Message>(), buf->size());
        }
    }
    free(buf);
}

__END_SYS
