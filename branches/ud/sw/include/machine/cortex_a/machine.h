// EPOS Cortex-A Mediator Declarations

#ifndef __cortex_a_h
#define __cortex_a_h

#include <machine.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

class Cortex_A: public Machine_Common
{
private:
    friend class Init_System;

    typedef CPU_Common::Reg32 Reg32;

    static const unsigned int IO_PLL_CLOCK = 1000000000;

    static const unsigned int DIV_MAX = 63;

    enum {
        SLCR_BASE = 0xF8000000
    };

    // SLCR Registers offsets
    enum {
        FPGA0_CLK_CTRL  = 0x170,
        PSS_RST_CTRL    = 0x200,
        FPGA_RST_CTRL   = 0x240
    };
    enum FPGAN_CLK_CTRL {
        DIVISOR0    = 1 << 8,
        DIVISOR1    = 1 << 20
    };

public:
    Cortex_A() {}

    static unsigned int n_cpus() { return Traits<Machine>::CPUS; }
    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0x3;
    }

    static void panic() {}
    static void reboot() {
        db<Machine>(WRN) << "Machine::reboot()" << endl;
        // FIXME: The line bellow will mess with qemu but seems to be working on
        // real hardware, possibly a bug in qemu. Note that the asserting reset
        // will clear the RAM where the application is stored.
        slcr(PSS_RST_CTRL) = 1;
    }

    static void poweroff() {}

    static void smp_barrier(unsigned int n_cpus = n_cpus()) {
        static volatile unsigned int ready[2];
        static volatile unsigned int i;

        if(smp) {
            int j = i;

            CPU::finc(ready[j]);

            if(cpu_id() == 0) {
                while(ready[j] < n_cpus); // wait for all CPUs to be ready
                i = !i;                   // toggle ready
                ready[j] = 0;             // signalizes waiting CPUs
            } else
                while(ready[j]);          // wait for CPU[0] signal
        }
    }
    static void smp_init(unsigned int n_cpus) {};

    // Returns the frequency set, -1 if frequency can't be set
    static int fpga0_clk_freq(unsigned int freq) {
        const unsigned int tol = 20;
        unsigned int div0 = 0, div1 = 0;
        Reg32 tmp;

        while(++div1 <= DIV_MAX) {
            div0 = 1;
            while(++div0 <= DIV_MAX)
                if((IO_PLL_CLOCK/(div0*div1) < (freq + freq/tol)) &&
                        (IO_PLL_CLOCK/(div0*div1) > (freq - freq/tol)))
                    goto set_clk_ctrl;
        }

        return -1;

        set_clk_ctrl:
        tmp = slcr(FPGA0_CLK_CTRL);
        tmp &= ~((DIVISOR0 * 0x3f) | (DIVISOR1 * 0x3f));
        slcr(FPGA0_CLK_CTRL) = tmp | (DIVISOR0 * div0) | (DIVISOR1 * div1);

        return IO_PLL_CLOCK/(div0*div1);
    }

    // PL logic connecting to the PS must not be reset when active transactions
    // exist, since uncompleted transactions could be left pending in the PS.
    // n = { 0, 1, 2, 3 }.
    static void fpga_reset(int n) {
        slcr(FPGA_RST_CTRL) |= 1 << n;
        // FPGA peripherals will reset on the rising/falling edge of their
        // clocks if reset is asserted. This "rough" 1 us delay will ensure all
        // peripherals operating on clocks higher than 1 MHz will be correctly
        // reseted.
        for(unsigned int i = 0; i < Traits<CPU>::CLOCK/1000000; i++)
            ASM("nop");
        slcr(FPGA_RST_CTRL) &= ~(1 << n);
    }

private:
    static void init();

    static volatile Reg32 & slcr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SLCR_BASE)[o / sizeof(Reg32)]; }

private:
    static const bool smp = Traits<System>::multicore;
};

__END_SYS

#include "info.h"
#include "uart.h"
#include "rtc.h"
#include "timer.h"
#include "nic.h"
#include "gpio.h"

#endif
