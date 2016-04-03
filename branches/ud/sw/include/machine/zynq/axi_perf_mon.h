// EPOS Zynq AXI AXI Performance Monitor Mediator

#ifndef __zynq_axi_perf_mon_h
#define __zynq_axi_perf_mon_h

#include <cpu.h>
#include <axi_perf_mon.h>

__BEGIN_SYS

class Zynq_AXI_Perf_Mon: public AXI_Perf_Mon_Common
{
private:
    typedef CPU::Reg32 Reg32;

    static const unsigned int MAX_COUNTERS = 64;

    static const unsigned int N_SLOT = 8;

    // Registers for each slot excluding minimum and maximum latency registers
    static const unsigned int N_SLOT_REGS = 6;

    enum {
        AXI_PERF_MON_BASE = 0x43C10000
    };

    // AXI Performance Monitor registers offsets
    enum {
        SR      = 0x002C,   // Sample
        SMC0    = 0x0200,   // Sampled Metric Counter 0
        CR      = 0x0300,   // Control
        SMC12   = 0x0600,   // Sampled Metric Counter 12
        SMC24   = 0x0800,   // Sampled Metric Counter 24
        SMC36   = 0x0A00    // Sampled Metric Counter 36
    };
    enum CR {
        METRICS_CNT_EN      = 1 << 0,
        METRICS_CNT_RESET   = 1 << 1,
    };

    enum {
        WR_BYTES    = 0,
        WR_TRANS    = 1,
        WR_LATENCY  = 2,
        RD_BYTES    = 3,
        RD_TRANS    = 4,
        RD_LATENCY  = 5
    };

public:
    Zynq_AXI_Perf_Mon() { reset(); }

    ~Zynq_AXI_Perf_Mon() {}

    void enable() { apm(CR) |= METRICS_CNT_EN; }
    void disable() { apm(CR) &= ~METRICS_CNT_EN; }

    void reset() {
        // Reset metric counters
        apm(CR) |= METRICS_CNT_RESET;

        // Deassert reset
        apm(CR) &= ~METRICS_CNT_RESET;
    }

    // Returns the number of clock cycles since last invocation. Each call to
    // sample() resets the metric counters.
    Reg32 sample() { return apm(SR); }

    Reg32 wr_bytes(int slot) { return sampled_counter(N_SLOT_REGS*slot + WR_BYTES); }
    Reg32 wr_trans(int slot) { return sampled_counter(N_SLOT_REGS*slot + WR_TRANS); }
    Reg32 wr_total_lat(int slot) { return sampled_counter(N_SLOT_REGS*slot + WR_LATENCY); }
    Reg32 wr_max_lat(int slot) { return (sampled_counter(N_SLOT_REGS*N_SLOT + 2*slot)>>16)&0xFFFF; }
    Reg32 wr_min_lat(int slot) { return (sampled_counter(N_SLOT_REGS*N_SLOT + 2*slot))&0xFFFF; }

    Reg32 rd_bytes(int slot) { return sampled_counter(N_SLOT_REGS*slot + RD_BYTES); }
    Reg32 rd_trans(int slot) { return sampled_counter(N_SLOT_REGS*slot + RD_TRANS); }
    Reg32 rd_total_lat(int slot) { return sampled_counter(N_SLOT_REGS*slot + RD_LATENCY); }
    Reg32 rd_max_lat(int slot) { return (sampled_counter(N_SLOT_REGS*N_SLOT + 2*slot + 1)>>16)&0xFFFF; }
    Reg32 rd_min_lat(int slot) { return (sampled_counter(N_SLOT_REGS*N_SLOT + 2*slot + 1))&0xFFFF; }

private:
    static volatile Reg32 & apm(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(AXI_PERF_MON_BASE)[o / sizeof(Reg32)]; }

    // Must be invoked after sample()
    Reg32 sampled_counter(int counter) {
        Reg32 ret;

        assert(counter < MAX_COUNTERS);

        if(counter < 12)
            ret = apm(SMC0 + (counter*16));
        else if(counter < 24)
            ret = apm(SMC12 + ((counter - 12) * 16));
        else if(counter < 36)
            ret = apm(SMC24 + ((counter - 24) * 16));
        else if(counter < 48)
            ret = apm(SMC36 + ((counter - 36) * 16));
        else if(counter < 50)
            ret = apm(SMC0 + 0x54 + ((counter - 48) * 4));
        else if(counter < 52)
            ret = apm(SMC0 + 0xB4 + ((counter - 50) * 4));
        else if(counter < 54)
            ret = apm(SMC12 + 0x54 + ((counter - 52) * 4));
        else if(counter < 56)
            ret = apm(SMC12 + 0xB4 + ((counter - 54) * 4));
        else if(counter < 58)
            ret = apm(SMC24 + 0x54 + ((counter - 56) * 4));
        else if(counter < 60)
            ret = apm(SMC24 + 0xB4 + ((counter - 58) * 4));
        else if(counter < 62)
            ret = apm(SMC36 + 0x54 + ((counter - 60) * 4));
        else
            ret = apm(SMC36 + 0xB4 + ((counter - 62) * 4));

        return ret;
    }
};

__END_SYS

#endif
