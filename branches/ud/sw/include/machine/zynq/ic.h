// EPOS Zynq IC Mediator Declarations

#ifndef __zynq_ic_h
#define __zynq_ic_h

#include <cpu.h>
#include <ic.h>

__BEGIN_SYS

class GIC_PL390
{
protected:
    static void enable() {
        dist(ICDISERN + 0) = ~0;
        dist(ICDISERN + 4) = ~0;
        dist(ICDISERN + 8) = ~0;
    }

    static void enable(int i) { dist(ICDISERN + (i/32)*4) = 1 << (i%32); }

    static void disable() {
        dist(ICDICERN + 0) = ~0;
        dist(ICDICERN + 4) = ~0;
        dist(ICDICERN + 8) = ~0;
    }

    static void disable(int i) { dist(ICDICERN + (i/32)*4) = 1 << (i%32); }

    static void init(void) {
        // Enable distributor
        dist(ICDDCR) = DIST_EN_S;

        // Mask no interrupt
        cpu_itf(ICCPMR) = 0xF0;

        // Enable interrupts signaling by the CPU interfaces to the connected
        // processors
        cpu_itf(ICCICR) = ACK_CTL | ITF_EN_NS | ITF_EN_S;
    }

protected:
    typedef CPU::Log_Addr Log_Addr;

    // Base address for memory-mapped registers
    enum {
        CPU_ITF_BASE  = 0xF8F00100,
        DIST_BASE     = 0xF8F01000
    };

    // Distributor Registers offsets
    enum {
        ICDDCR      = 0x000, // Distributor Control
        ICDISERN    = 0x100, // Interrupt Set-Enable
        ICDICERN    = 0x180  // Interrupt Clear-Enable
    };
    enum ICDDCR {
        DIST_EN_S   = 1 << 0
    };

    // CPU Interface Registers offsets
    enum {
        ICCICR  = 0x000, // CPU Interface Control
        ICCPMR  = 0x004, // Interrupt Priority Mask
        ICCIAR  = 0x00C, // Interrupt Ack
        ICCEOIR = 0x010  // End Of Interrupt
    };
    enum ICCICR {
        ITF_EN_S    = 1 << 0,
        ITF_EN_NS   = 1 << 1,
        ACK_CTL     = 1 << 2
    };

    static const unsigned int INT_ID_MASK = 0x3FF;

    static Log_Addr & cpu_itf(unsigned int o) { return reinterpret_cast<Log_Addr *>(CPU_ITF_BASE)[o / sizeof(Log_Addr)]; }
    static Log_Addr & dist(unsigned int o) { return reinterpret_cast<Log_Addr *>(DIST_BASE)[o / sizeof(Log_Addr)]; }
};

class Zynq_IC: private IC_Common, private GIC_PL390
{
    friend class Zynq;

protected:
    typedef GIC_PL390 Engine;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;

    static const unsigned int INTS = 96;
    enum {
        INT_TIMER       = 29,   // Each core has its own private timer interrupt
        INT_RESCHEDULER = 42    // TODO: Implement this shit
    };

public:
    Zynq_IC() {}

    static Interrupt_Handler int_vector(const Interrupt_Id & i) {
        assert(i < INTS);
        return _int_vector[i];
    }

    static void int_vector(const Interrupt_Id & i, const Interrupt_Handler & h) {
        db<IC>(TRC) << "IC::int_vector(int=" << i << ",h=" << reinterpret_cast<void *>(h) <<")" << endl;
        assert(i < INTS);
        _int_vector[i] = h;
    }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        Engine::enable();
    }

    static void enable(Interrupt_Id i) {
        db<IC>(TRC) << "IC::enable(irq=" << i << ")" << endl;
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        Engine::disable();
    }

    static void disable(Interrupt_Id i) {
        db<IC>(TRC) << "IC::disable(irq=" << i << ")" << endl;
        Engine::disable(i);
    }

    static void ipi_send(unsigned int cpu, Interrupt_Id int_id) {}

private:
    static void dispatch();

    static void int_not(const Interrupt_Id & i);

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif
