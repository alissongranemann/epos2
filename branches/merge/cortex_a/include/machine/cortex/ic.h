// EPOS ARM Cortex IC Mediator Declarations

#ifndef __cortex_ic_h
#define __cortex_ic_h

#include <cpu.h>
#include <ic.h>
#include __MODEL_H

__BEGIN_SYS

class NVIC;
class GIC;

#ifdef __mmod_zynq__

class GIC: public IC_Common, protected Cortex_Model
{
private:
//    typedef CPU::Reg32 Reg32;
//    typedef CPU::Log_Addr Log_Addr;

public:
    // IRQs
    // FIXME: a real list with typical A9 assignments must come here. In last case, use Xilinx's for Zynq
    //    static const unsigned int IRQS = Cortex_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_XXXXX       = 0,
    };


    // Interrupts
    static const unsigned int INTS = 96;
    static const unsigned int EXC_INT = 0;
    static const unsigned int HARD_INT = 16;
//    static const unsigned int SOFT_INT = HARD_INT + IRQS;
    static const unsigned int SOFT_INT = 42;
    enum {
        INT_HARD_FAULT  = EXC_INT + CPU::EXC_HARD,
        INT_TIMER       = 29,
//        INT_FIRST_HARD  = HARD_INT,
//        INT_MACTIMER    = HARD_INT + IRQ_MACTIMER,
//        INT_LAST_HARD   = SOFT_INT - 1,
        INT_RESCHEDULER = SOFT_INT
    };


public:
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

    static Interrupt_Id int_id() {
        Reg32 icciar = cpu_itf(ICCIAR) & INT_ID_MASK;

        // For every read of a valid interrupt id from the ICCIAR, the ISR must
        // perform a matching write to the ICCEOIR
        cpu_itf(ICCEOIR) = icciar;
        return icciar;
    }

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

#else

class NVIC: public IC_Common, protected Cortex_Model
{
private:
//    typedef CPU::Reg32 Reg32;
//    typedef CPU::Log_Addr Log_Addr;

public:
    // IRQs
    static const unsigned int IRQS = Cortex_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_GPIOA       = 0,
        IRQ_GPIOB       = 1,
        IRQ_GPIOC       = 2,
        IRQ_GPIOD       = 3,
        IRQ_GPIOE       = 4,
        IRQ_UART0       = 5,
        IRQ_UART1       = 6,
        IRQ_SSI0        = 7,
        IRQ_I2C         = 8,
        IRQ_ADC         = 14,
        IRQ_WATCHDOG    = 18,
        IRQ_GPT0A       = 19,
        IRQ_GPT0B       = 20,
        IRQ_GPT1A       = 21,
        IRQ_GPT1B       = 22,
        IRQ_GPT2A       = 23,
        IRQ_GPT2b       = 24,
        IRQ_AC          = 25,
        IRQ_RFTXRX      = 26,
        IRQ_RFERR       = 27,
        IRQ_SC          = 28,
        IRQ_FC          = 29,
        IRQ_AES         = 30,
        IRQ_PKA         = 31,
        IRQ_SMT         = 32,
        IRQ_MACTIMER    = 33,
        IRQ_SSI1        = 34,
        IRQ_GPT3A       = 35,
        IRQ_FPT3B       = 36,
        IRQ_UDMASW      = 46,
        IRQ_UDMAERR     = 47,
        IRQ_LAST        = IRQ_UDMAERR
    };

    // Interrupts
    static const unsigned int INTS = 64;
    static const unsigned int EXC_INT = 0;
    static const unsigned int HARD_INT = 16;
    static const unsigned int SOFT_INT = HARD_INT + IRQS;
    enum {
        INT_HARD_FAULT  = EXC_INT + CPU::EXC_HARD,
        INT_TIMER       = 15,
        INT_FIRST_HARD  = HARD_INT,
        INT_MACTIMER    = HARD_INT + IRQ_MACTIMER,
        INT_LAST_HARD   = SOFT_INT - 1,
        INT_RESCHEDULER = SOFT_INT
    };

public:
    NVIC() {}

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

    static Interrupt_Id int_id() { return CPU::flags() & 0x3f; }

    static void init(void) {};

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
};

#endif


class Cortex_IC: private IF<Traits<Build>::MODEL == Traits<Build>::Zynq, GIC, NVIC>::Result
{
    friend class Cortex;

private:
    typedef IF<Traits<Build>::MODEL == Traits<Build>::Zynq, GIC, NVIC>::Result Engine;
//
//    typedef CPU::Reg32 Reg32;
//    typedef CPU::Log_Addr Log_Addr;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_HARD_FAULT;
    using Engine::INT_TIMER;
    using Engine::INT_RESCHEDULER;

public:
    Cortex_IC() {}

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
        assert(i < INTS);
        Engine::enable();
    }

    static void enable(int i) {
        db<IC>(TRC) << "IC::enable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        assert(i < INTS);
        Engine::disable();
    }

    static void disable(int i) {
        db<IC>(TRC) << "IC::disable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::disable(i);
    }

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }

    static void ipi_send(unsigned int cpu, Interrupt_Id int_id) {}

private:
    static void dispatch(unsigned int i) {
        Interrupt_Id id = int_id();

        if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
            db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

        _int_vector[id](id);
    }

    // Logical handlers
    static void int_not(const Interrupt_Id & i);
    static void hard_fault(const Interrupt_Id & i);

    // Physical handlers
    static void entry();

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif