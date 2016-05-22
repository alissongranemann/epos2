// EPOS Cortex-M TSTP NIC Mediator Declarations

#ifndef __emote3_tstp_mac_h
#define __emote3_tstp_mac_h

#include <ic.h>
#include <cpu.h>
#include <machine/cortex_m/emote3_ieee802_15_4_phy.h>
#include <tstp_mac.h>

__BEGIN_SYS

// CC2538 TSTP MAC Radio Mediator
class eMote3_TSTP_MAC: public TSTP_MAC, public TSTP_MAC::Observed, private eMote3_IEEE802_15_4_PHY
{
    // Transmit and Receive Ring sizes
    static const unsigned int TX_BUFS = Traits<eMote3_TSTP_MAC>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<eMote3_TSTP_MAC>::RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

    template <int unit> friend void call_init();

    typedef CPU::IO_Irq IO_Irq;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef MMU::DMA_Buffer DMA_Buffer;
    static const unsigned int MTU = TSTP_MAC::MTU;
    typedef TSTP_MAC::Frame Frame;
    typedef TSTP_MAC::Header Header;
    typedef TSTP_MAC::Statistics Statistics;
    typedef TSTP_MAC::CRC CRC;

private:
    static void lock() { CPU::int_disable(); }
    static void unlock() { CPU::int_enable(); }

    static const unsigned int UNITS = Traits<eMote3_TSTP_MAC>::UNITS;

    // Interrupt dispatching binding
    struct Device {
        eMote3_TSTP_MAC * device;
        unsigned int interrupt;
    };

protected:
    eMote3_TSTP_MAC(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf);

public:
    unsigned int channel() { return _channel; }
    void channel(unsigned int c) { if((c > 10) and (c < 27)) { _channel = c; eMote3_IEEE802_15_4_PHY::channel(_channel); } }

    ~eMote3_TSTP_MAC();

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);
    void free(Buffer * buf);
    int send(Buffer * buf);
private:
    Buffer * alloc_mf(bool all_listen, const Frame_ID & id);
public:

    // Just to comply with EPOS' NIC interface
    const Address & address() { return _address; }
    void address(const Address & address) { _address = address; }
private:
    Address _address;

public:

    const Statistics & statistics() { return _statistics; }

    void reset();

    static eMote3_TSTP_MAC * get(unsigned int unit = 0) { return get_by_unit(unit); }

private:
    unsigned int _channel;
    void handle_int();

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static unsigned int _timer_int_unit;

    STATE _scheduled_state;

    void next_state(const STATE & s, const Time & when);
    static void state_machine() { get_by_unit(_timer_int_unit)->state(); }

    void state() { state(_scheduled_state); }
    void state(const STATE & s) {
        switch(s) {
            case STATE::CHECK_TX_SCHEDULE: check_tx_schedule(); break;
            case STATE::RX_MF: rx_mf(); break;
            case STATE::RX_DATA: rx_data(); break;
            case STATE::CCA: cca(); break;
            case STATE::TX_MF: tx_mf(); break;
            case STATE::TX_DATA: tx_data(); break;
            default: break;
        }
    }

    static eMote3_TSTP_MAC * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _devices[unit].device;
    }

    static eMote3_TSTP_MAC * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++)
            if(_devices[i].interrupt == interrupt) {
                return _devices[i].device;
            }

        return 0;
    };

    static void init(unsigned int unit);

    // TSTP MAC methods
    void prepare_tx_mf();
    void process_mf(Buffer * buf);
    void process_data(Buffer * buf);
    //// State Machine
    void check_tx_schedule();
    void rx_mf();
    void rx_data(); 
    void cca(); 
    void tx_mf(); 
    void tx_data(); 

private:
    unsigned int _unit;

    Statistics _statistics;

    IO_Irq _irq;

    DMA_Buffer * _dma_buf;

    int _rx_cur;
    int _tx_cur;

    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];

    static Device _devices[UNITS];
    Buffer::List _tx_schedule;
    Buffer * _tx_pending;
    Buffer * _sending_microframe;
    STATE _rx_state;
    Frame_ID _receiving_data_id;
};

__END_SYS

#endif
