#include <tstp.h>
#include <ic.h>

__BEGIN_SYS

TSTP::MAC::MACS TSTP::MAC::_macs[TSTP::MAC::UNITS];

void TSTP::MAC::send(Buffer * buf) { // TODO
    send_frame<PHY_Layer>(buf);
    free(buf);
}

void TSTP::MAC::update(Buffer * buf) {
    auto t = buf->frame()->message_type();
    switch(t) {
        case MESSAGE_TYPE::INTEREST:
            _tstp->update(buf->frame()->as<Interest_Message>());
            break;
        case MESSAGE_TYPE::DATA:
            _tstp->update(buf->frame()->as<Data_Message>(), buf->size());
            break;
        default:
            break;
    }
}

TSTP_MAC::Buffer * TSTP_MAC::alloc(unsigned int size, Frame * f) {
    db<TSTP_MAC>(TRC) << "TSTP_MAC::alloc(size=" << size << ")" << endl;

    // Wait for the next buffer to become free and seize it
    unsigned int i = 1;
    for(bool locked = false; !locked; i++) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur %= TX_BUFS;
        if(i >= 2*TX_BUFS) {
            db<TSTP_MAC>(WRN) << "TSTP_MAC::alloc: No buffer available! Dropping packet" << endl;
            return 0; // No buffer found
        }
    }
    Buffer * buf = _tx_buffer[_tx_cur];

    new (buf) Buffer(_tstp, size, *f);

    db<TSTP_MAC>(INF) << "TSTP_MAC::alloc[" << _tx_cur << "]" << endl;

    ++_tx_cur %= TX_BUFS;

    return buf;
}

void TSTP_MAC::free(Buffer * b) {
    b->unlock();
}

__END_SYS
