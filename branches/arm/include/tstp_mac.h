#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <system.h>
#include <utility/buffer.h>
#include <mmu.h>
#include <cpu.h>
#include <ic.h>
#include <tstp_router.h>

__BEGIN_SYS

class TSTP_MAC
{
    typedef CPU::Reg32 Reg32;

    friend class TSTP;
    friend class TSTP_NIC;
    typedef Traits<TSTP>::MAC_Config<0>::PHY_Layer PHY_Layer; // TODO: Polymorphic PHY
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef CPU::IO_Irq IO_Irq;
    typedef Traits<TSTP>::Router::Address Address;

public:
    class Frame {// TODO
    public:
        template<typename T>
        T* data() { return 0; } 
    private:
        Address _me;
    }__attribute__((packed));

    typedef _UTIL::Buffer<TSTP_MAC, Frame> Buffer;

private:
    Buffer ** _rx_buffer;
    Buffer ** _tx_buffer;
    unsigned int _rx_cur;
    unsigned int _tx_cur;

    unsigned int _tx_bufs;
    unsigned int _rx_bufs;

    void update(Buffer * buf) { }

    template<unsigned int UNIT, typename PHY = PHY_Layer>
    static void init(unsigned int unit = UNIT);

    template<typename PHY = PHY_Layer>
    TSTP_MAC(PHY * phy, unsigned int tx_bufs, unsigned int rx_bufs, DMA_Buffer * dma_buf);// : _phy(phy), _tx_bufs(tx_bufs), _rx_bufs(rx_bufs) { } // TODO: Polymorphic PHY

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

    PHY_Layer * _phy;
    TSTP * _tstp;
};

__END_SYS

#endif
