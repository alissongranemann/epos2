#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <system.h>
#include <utility/buffer.h>
#include <mmu.h>
#include <cpu.h>
#include <ic.h>
#include <tstp_api.h>

__BEGIN_SYS

class TSTP_MAC : public TSTP_API
{
    typedef CPU::Reg32 Reg32;

    friend class TSTP;
    friend class TSTP_NIC;
    typedef Traits<TSTP>::MAC_Config<> Config;
    typedef Config::PHY_Layer PHY_Layer; // TODO: Polymorphic PHY
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef CPU::IO_Irq IO_Irq;

    typedef TSTP_API API;

    typedef CPU::Reg16 CRC;

    static const unsigned int TX_BUFS = Config::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Config::RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

private:
    // Frame to hold PHY packets. Will be cast to either Frame or Microframe
    class MAC_Frame {
    public:
        template<typename T>
        T* data() { return reinterpret_cast<T*>(_data); }
    private:
        unsigned char _data[MTU - sizeof(CRC)];
        CRC _crc;
    }__attribute__((packed));

    class Microframe {
    private:
        unsigned _all_listen : 1;
        unsigned _id : 15;
        unsigned _count : 8;
        Address_Hint _hint;
        // CRC is handled by the interrupt handler
    }__attribute__((packed));

public: //TODO: remove "public"
    // Bridge between the hardware-dependent PHY layer and hardware-independent TSTP
    void update(Buffer * buf);

    // == Methods for interacting with the PHY layer ==
    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];
    unsigned int _rx_cur;
    unsigned int _tx_cur;

    template<typename PHY = PHY_Layer>
    static void init(unsigned int unit);

    template<typename PHY = PHY_Layer>
    TSTP_MAC(PHY * phy, DMA_Buffer * dma_buf);

    // Interrupt dispatching binding
    struct MACS {
        TSTP_MAC * mac;
        unsigned int interrupt;
    };

    static const unsigned int UNITS = Traits<NIC>::UNITS;

    static MACS _macs[UNITS];

    static TSTP_MAC * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<TSTP_MAC>(WRN) << "TSTP_MAC::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _macs[unit].mac;
    }

    static TSTP_MAC * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++) {
            if(_macs[i].interrupt == interrupt) {
                return _macs[i].mac;
            }
        }
        return 0;
    }

    template <typename PHY = typename Config::PHY_Layer>
    static void int_handler(const IC::Interrupt_Id & interrupt) {
        TSTP_MAC * mac = get_by_interrupt(interrupt);

        db<TSTP_MAC>(TRC) << "TSTP_MAC::int_handler(int=" << interrupt << ",mac=" << mac << ")" << endl;

        if(!mac)
            db<TSTP_MAC>(WRN) << "TSTP_MAC::int_handler: handler not assigned!" << endl;
        else
            mac->handle_int<PHY>();
    }

    template <typename PHY = PHY_Layer>
    void handle_int();

    template <typename PHY = PHY_Layer>
    void send_mf(Buffer * b);

    template <typename PHY = PHY_Layer>
    void send_frame(Buffer * b);

    // == TSTP -> TSTP_MAC interface ==
    Buffer * alloc(unsigned int size, Frame * f);
    void send(Buffer * b);
    void free(Buffer * b);

    Time last_frame_time;

    PHY_Layer * _phy;
    TSTP * _tstp;
};

__END_SYS

#endif
