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
    static const unsigned int IRQS = Cortex_M_Model::IRQS;
    typedef Interrupt_Id IRQ;

    // Interrupts
    static const unsigned int INTS = 64;
    static const unsigned int EXC_INT = 0;
    static const unsigned int HARD_INT = 16;
    static const unsigned int SOFT_INT = HARD_INT + IRQS;
    enum {
        INT_HARD_FAULT  = EXC_INT + CPU::EXC_HARD,
        INT_TIMER       = 15,
        INT_FIRST_HARD  = HARD_INT,
        INT_LAST_HARD   = SOFT_INT - 1,
        INT_RESCHEDULER = SOFT_INT
    };

public:
    Cortex_M_IC() {}

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
        scs(IRQ_ENABLE0) = ~0;
        if(IRQS > 32) scs(IRQ_ENABLE1) = ~0;
        if(IRQS > 64) scs(IRQ_ENABLE2) = ~0;
    }

    static void enable(const IRQ & i) {
        db<IC>(TRC) << "IC::enable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_ENABLE0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_ENABLE1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_ENABLE2) = 1 << (i - 64);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        scs(IRQ_DISABLE0) = ~0;
        if(IRQS > 32) scs(IRQ_DISABLE1) = ~0;
        if(IRQS > 64) scs(IRQ_DISABLE2) = ~0;
    }

    static void disable(const IRQ & i) {
        db<IC>(TRC) << "IC::disable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_DISABLE0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_DISABLE1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_DISABLE2) = 1 << (i - 64);
        unpend(i);
    }

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }

    static void ipi_send(int dest, int interrupt) {}

private:
    static void unpend() {
        db<IC>(TRC) << "IC::unpend()" << endl;
        scs(IRQ_UNPEND0) = ~0;
        scs(IRQ_UNPEND1) = ~0;
        scs(IRQ_UNPEND2) = ~0;
    }

    static void unpend(const IRQ & i) {
        db<IC>(TRC) << "IC::unpend(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_UNPEND0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_UNPEND1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_UNPEND2) = 1 << (i - 64);
    }

    static void dispatch() {
        register Interrupt_Id id = CPU::int_id();

        if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
            db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

        _int_vector[id](id);
    }

    // Logical handlers
    static void int_not(const Interrupt_Id & i);
    static void hard_fault(const Interrupt_Id & i);

    // Physical handler
    static void entry() __attribute__((naked));

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif
