// EPOS Cortex-M IC Mediator Declarations

#ifndef __cortex_m_ic_h
#define __cortex_m_ic_h

#include <cpu.h>
#include <ic.h>
#include __MODEL_H

__BEGIN_SYS

class Cortex_M_IC: private IC_Common, private Cortex_M_Model
{
    friend class Cortex_M;

private:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;

    // IRQs
    static const unsigned int IRQS = 44;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_TIMER       = 0,
        IRQ_KEYBOARD    = 1,
        IRQ_CASCADE     = 2,
        IRQ_SERIAL24    = 3,
        IRQ_SERIAL13    = 4,
        IRQ_PARALLEL23  = 5,
        IRQ_FLOPPY      = 6,
        IRQ_PARALLEL1   = 7,
        IRQ_RTC         = 8,
        IRQ_MOUSE       = 12,
        IRQ_MATH        = 13,
        IRQ_DISK1       = 14,
        IRQ_DISK2       = 15,
        IRQ_LAST        = IRQ_DISK2
    };

    // Interrupts
    static const unsigned int INTS = 64;
    static const unsigned int HARD_INT = 16;
    static const unsigned int SOFT_INT = HARD_INT + IRQS;
    enum {
        INT_TIMER       = 15,
        INT_FIRST_HARD  = HARD_INT,
        INT_LAST_HARD   = HARD_INT + IRQ_LAST,
        INT_RESCHEDULER = SOFT_INT
    };

public:
    Cortex_M_IC() {}

    static Interrupt_Id irq2int(const IRQ & i) { return i + 16; }
    static IRQ int2irq(const Interrupt_Id & i) { return i - 16; }

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
        scs(IRQ_ENABLE) = ~0;
        scs(IRQ_ENABLE1) = ~0;
    }

    static void enable(const IRQ & i) {
        db<IC>(TRC) << "IC::enable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32)
            scs(IRQ_ENABLE) |= 1 << i;
        else
            scs(IRQ_ENABLE1) |= 1 << (i-32);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        scs(IRQ_DISABLE) = ~0;
        scs(IRQ_DISABLE1) = ~0;
    }

    static void disable(const IRQ & i) {
        db<IC>(TRC) << "IC::disable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32)
            scs(IRQ_DISABLE) |= 1 << i;
        else
            scs(IRQ_DISABLE1) |= 1 << (i-32);
    }

    static void unpend() {
        db<IC>(TRC) << "IC::unpend()" << endl;
        scs(IRQ_UNPEND) = ~0;
        scs(IRQ_UNPEND1) = ~0;
    }

    static void unpend(const IRQ & i) {
        db<IC>(TRC) << "IC::unpend(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32)
            scs(IRQ_UNPEND) |= 1 << i;
        else
            scs(IRQ_UNPEND) |= 1 << (i-32);
    }

    static void ipi_send(unsigned int cpu, unsigned int interrupt) {}

private:
    static void dispatch() __attribute__((naked));
    static void _dispatch();

    static void int_not(const Interrupt_Id & i);

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif
