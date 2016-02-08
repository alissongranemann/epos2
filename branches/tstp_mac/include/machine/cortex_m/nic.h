// EPOS Cortex-M NIC Mediator Declarations

#ifndef __cortex_m_nic_h
#define __cortex_m_nic_h

#include <nic.h>

#include <tstp_mac.h>
#include <system/config.h>
#include "cc2538_radio.h"

__BEGIN_SYS

class Cortex_M_Radio: public TSTP_MAC
{
    friend class Cortex_M;

private:
    typedef Traits<Cortex_M_Radio>::NICS NICS;
    static const unsigned int UNITS = NICS::Length;

public:
    template<unsigned int UNIT = 0>
    Cortex_M_Radio(unsigned int u = UNIT) {
        _dev = Meta_NIC<NICS>::Get<UNIT>::Result::get(u);
        db<Cortex_M_Radio>(TRC) << "NIC::NIC(u=" << UNIT << ",d=" << _dev << ") => " << this << endl;
    }
    ~Cortex_M_Radio() { _dev = 0; }
    
    Buffer * alloc(NIC * nic, unsigned int payload) {
        return _dev->alloc(nic, payload);
    }

    typedef void (Frame_Handler)(Buffer *);

    void set_tx() {_dev->set_tx(); } //TODO : REMOVE
    void clear_tx(){_dev->clear_tx(); }//TODO : REMOVE
    void set_rx() {_dev->set_rx(); } //TODO : REMOVE
    void clear_rx(){_dev->clear_rx(); }//TODO : REMOVE

    void off() { _dev->off(); }
    void receive(const Frame_Handler & handler) { _dev->receive(handler); }
    int send(Buffer * buf) { return _dev->send(buf); }
    void free(Buffer * buf) { _dev->free(buf); }

    const Address & address() { return _dev->address(); }

    const Statistics & statistics() { return _dev->statistics(); }

    void address(const Address & address) { _dev->address(address); }
    bool channel_busy() { return _dev->channel_busy(); }

private:
    static void init();

private:
    Meta_NIC<NICS>::Base * _dev;
};

__END_SYS

#endif
