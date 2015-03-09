// EPOS Zynq IC Mediator Declarations

#ifndef __zynq_ic_h
#define __zynq_ic_h

#include <cpu.h>
#include <ic.h>

__BEGIN_SYS

class GIC_PL390
{
protected:
    // Distributor Register Description
    enum {
        ICDDCR      = 0x000, // Distributor Control Register
        ICDISRN     = 0x080, // 0x080 - 0x09C - Interrupt Security Registers
        ICDISERN    = 0x100, // 0x104-0x11C Interrupt Set-Enable Registers
        ICDICERN    = 0x180, // 0x184-0x19C Interrupt Clear-Enable Registers
        ICDIPRN     = 0x400, // 0x400-0x4FC Interrupt Priority Registers
        ICDIPTRN    = 0x800, // 0x800-0x8FC Interrupt Processor Targets Registers
        ICDICFRN    = 0xC00, // 0xC00, 0xC08, 0xC08-0xC3C Interrupt Configuration Registers
    };

    // Processor Interface Register
    enum {
        ICCICR  = 0x000, // CPU Interface Control Register
        ICCPMR  = 0x004, // Interrupt Priority Mask Register
        ICCBPR  = 0x008, // Binary Point Register
        ICCIAR  = 0x00C, // Interrupt Ack Register
        ICCEOI  = 0x010, // End Of Interrupt Register
    };

    static const unsigned int PROC_INTERFACE    = 0xF8F00100;
    static const unsigned int INTR_DISTRIBUTOR  = 0xF8F01000;

    static const unsigned int INTERRUPT_MASK = 0x000003FF;

protected:
    static void interrupt_distributor_init(void) {
        // TODO: Maybe that's not necessary, it's already disabled after reset
        disable_distributor();

        // Configure every Shared Peripheral Interrupt (SPI)
        // TODO: WTF?
        CPU::out32(INTR_DISTRIBUTOR + ICDICFRN + 8, 0x555D5757);
        CPU::out32(INTR_DISTRIBUTOR + ICDICFRN + 12, 0x5555D5D5);
        CPU::out32(INTR_DISTRIBUTOR + ICDICFRN + 16, 0x75555555);
        CPU::out32(INTR_DISTRIBUTOR + ICDICFRN + 20, 0xFF555555);

        // Reset interrupt priorities of every SPI. Temporarily setting
        // everybody to the highest priority (0).
        for(unsigned int i = 0; i < 24; i++)
            CPU::out32(INTR_DISTRIBUTOR + ICDIPRN + i*4, 0x00);

        // Set CPU target of every SPI
        for(unsigned int i = 0; i < 24; i++)
            CPU::out32(INTR_DISTRIBUTOR + ICDIPTRN + i*4, 0xFF);

        // Enable PPIs
        // TODO: It's not necessary to enable all PPIs!
        for(unsigned int i = 0; i < 3; i++)
            CPU::out32(INTR_DISTRIBUTOR + ICDISERN + i*4, 0xFFFFFFFF);

        // Set all interrupts as non-secure
        for(unsigned int i = 0; i < 3; i++) // (!) Secure accesses only
            CPU::out32(INTR_DISTRIBUTOR + ICDISRN + i*4, 0xFFFFFFFF);

        cpu_interface_init();
        enable_distributor();
    }

    static void cpu_interface_init(void) {
        // Mask no interrupt
        CPU::out32(PROC_INTERFACE + ICCPMR, 0xFF);

        // Disable interrupt handling preemption
        CPU::out32(PROC_INTERFACE + ICCBPR, 0x07);

        enable_cpu_interface();
    }

    static void enable_distributor() {
        CPU::out32(INTR_DISTRIBUTOR + ICDDCR, 0x00000001);
    }

    static void disable_distributor() {
        CPU::out32(INTR_DISTRIBUTOR + ICDDCR, 0x00000000);
    }

    static void enable_cpu_interface() {
        CPU::out32(PROC_INTERFACE + ICCICR, 0x00000001);
    }

    static void disable_cpu_interface() {
        CPU::out32(PROC_INTERFACE + ICCICR, 0x00000000);
    }
};

class Zynq_IC: public IC_Common, private GIC_PL390
{
    friend class Zynq;

public:
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
        enable_cpu_interface();
        enable_distributor();
    }

    static void enable(Interrupt_Id i) {
        db<IC>(TRC) << "IC::enable(irq=" << i << ")" << endl;
        unsigned int word = i/32;

        i %= 32;
        i = 1 << i;

        CPU::out32(INTR_DISTRIBUTOR + ICDISERN + (word*4), CPU::in32(INTR_DISTRIBUTOR + ICDISERN + (word*4)) | i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        disable_distributor();
        disable_cpu_interface();
    }

    static void disable(Interrupt_Id i) {
        db<IC>(TRC) << "IC::disable(irq=" << i << ")" << endl;
        unsigned int word = i/32;

        i %= 32;
        i = 1 << i;

        CPU::out32(INTR_DISTRIBUTOR + ICDICERN + (word*4), i);
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
