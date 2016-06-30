// EPOS Cortex-A IC Mediator Declarations

#ifndef __cortex_a_ic_h
#define __cortex_a_ic_h

#include <cpu.h>
#include <ic.h>
#include __MODEL_H

__BEGIN_SYS

class GIC_PL390: protected Cortex_A_Model
{
protected:
    static void enable() {
        dist(ICDISER0) = ~0;
        dist(ICDISER1) = ~0;
        dist(ICDISER2) = ~0;
    }

    static void enable(int i) { dist(ICDISER0 + (i/32)*4) = 1 << (i%32); }

    static void disable() {
        dist(ICDICER0) = ~0;
        dist(ICDICER1) = ~0;
        dist(ICDICER1) = ~0;
    }

    static void disable(int i) { dist(ICDICER0 + (i/32)*4) = 1 << (i%32); }

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
    static const unsigned int INT_ID_MASK = 0x3FF;
};

class Cortex_A_IC: private IC_Common, private GIC_PL390
{
    friend class Cortex_A;

protected:
    typedef GIC_PL390 Engine;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;

    static const unsigned int INTS = 96;
    enum {
        INT_TIMER       = 29,
        INT_RESCHEDULER = 42    // FIXME: Find the real number
    };

public:
    Cortex_A_IC() {}

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
    static void dispatch() {
        unsigned int icciar = cpu_itf(ICCIAR);
        register Interrupt_Id id = icciar & INT_ID_MASK;

        // For every read of a valid interrupt id from the ICCIAR, the ISR must
        // perform a matching write to the ICCEOIR
        cpu_itf(ICCEOIR) = icciar;

        if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
            db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

        _int_vector[id](id);
    }

    static void int_not(const Interrupt_Id & i);

    // Physical handler
    static void entry() __attribute__((naked));

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif
