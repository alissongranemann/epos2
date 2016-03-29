#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <system.h>
#include <utility/buffer.h>
#include <mmu.h>
#include <cpu.h>
#include <ic.h>
#include <tstp_api.h>

__BEGIN_SYS

class TSTP_MAC
{
private:
    typedef CPU::Reg32 Reg32;

    friend class TSTP;
    friend class TSTP_NIC;
    typedef Traits<TSTP>::MAC_Config<0>::PHY_Layer PHY_Layer; // TODO: Polymorphic PHY
    typedef Traits<TSTP>::MAC_Config<0>::Timer Timer; // TODO: Several/polymorphic timers
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef CPU::IO_Irq IO_Irq;

    typedef TSTP_API API;
    typedef API::Local_Address Local_Address;
    typedef API::Remote_Address Remote_Address;
    typedef API::Address_Hint Hint;
    typedef API::Buffer Buffer;
    typedef API::Scheduled_Message Scheduled_Message;
    static const auto MTU = API::MTU;

    typedef CPU::Reg16 CRC;

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
        Hint _hint;
        // CRC is handled by the interrupt handler
    }__attribute__((packed));

    // Bridge between the hardware-dependent PHY layer and hardware-independent TSTP
    void update(Buffer * buf) { }

    // == Methods for interacting with the PHY layer ==
    Buffer ** _rx_buffer;
    Buffer ** _tx_buffer;
    unsigned int _rx_cur;
    unsigned int _tx_cur;

    unsigned int _tx_bufs;
    unsigned int _rx_bufs;

    template<unsigned int UNIT, typename PHY = PHY_Layer>
    static void init(unsigned int unit = UNIT);

    template<typename PHY = PHY_Layer>
    TSTP_MAC(PHY * phy, unsigned int tx_bufs, unsigned int rx_bufs, DMA_Buffer * dma_buf);

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

    static void int_handler(const IC::Interrupt_Id & interrupt);

    template <typename PHY = PHY_Layer>
    void handle_int();

    // == TSTP -> TSTP_MAC interface ==
    void send(Scheduled_Message * message);
    bool alloc(Scheduled_Message * message);

    PHY_Layer * _phy;
    TSTP * _tstp;
};

__END_SYS

#endif
