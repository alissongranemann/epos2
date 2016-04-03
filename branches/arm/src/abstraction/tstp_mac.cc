#include <tstp.h>
#include <ic.h>

__BEGIN_SYS

TSTP::MAC::MACS TSTP::MAC::_macs[TSTP::MAC::UNITS];

void One_Hop_MAC::send(Buffer * buf) { // TODO
    buf->frame()->header()->last_hop_address(_tstp->address());
    send_frame<PHY_Layer>(buf);
    free(buf);
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::send() : Sent header = " << *(buf->frame()->header()) << endl;
    db<One_Hop_MAC>(TRC) << "size = " << buf->size() << endl;
}

void One_Hop_MAC::update(Buffer * buf) {
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::update(buf=" << buf << ")" << endl;
    auto t = buf->frame()->message_type();
    db<One_Hop_MAC>(TRC) << "Message type = " << t << endl;
    if(t == MESSAGE_TYPE::INTEREST) {
        if(!Traits<TSTP>::is_sink) {
            auto msg = buf->frame()->as<Interest_Message>();
            db<One_Hop_MAC>(TRC) << "Interest_Message = " << *msg << endl;
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

One_Hop_MAC::Buffer * One_Hop_MAC::alloc(unsigned int size, Frame * f) {
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::alloc(size=" << size << ")" << endl;

    // Wait for the next buffer to become free and seize it
    unsigned int i = 1;
    for(bool locked = false; !locked; i++) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur %= TX_BUFS;
        if(i >= 2*TX_BUFS) {
            db<One_Hop_MAC>(WRN) << "One_Hop_MAC::alloc: No buffer available! Dropping packet" << endl;
            return 0; // No buffer found
        }
    }
    Buffer * buf = _tx_buffer[_tx_cur];

    new (buf) Buffer(_tstp, size, *f);

    db<One_Hop_MAC>(INF) << "One_Hop_MAC::alloc[" << _tx_cur << "]" << endl;

    ++_tx_cur %= TX_BUFS;

    return buf;
}

void One_Hop_MAC::free(Buffer * b) {
    b->unlock();
}

__END_SYS
