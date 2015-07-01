// EPOS eMote3 Time-Stamp Counter Mediator Declarations

#ifndef __emote3_tsc_h
#define __emote3_tsc_h

#include <cpu.h>
#include <architecture/armv7/tsc.h>

__BEGIN_SYS

// CC2538 Sleep Timer
class CC2538_TSC: private ARMv7_TSC
{
private:
    // Clock for the sleep timer is fixed at 32-kHz
    static const unsigned int CLOCK = 32000;

    typedef CPU::Reg32 Reg32;

    enum
    {
        TSC_BASE = 0x400D5000
    };

    enum Offset
    {
        //Register Name      Offset  Type  Width  Reset Value 
        SMWDTHROSC_ST0    =  0x40, //RW    32     0x00000000 
        SMWDTHROSC_ST1    =  0x44, //RW    32     0x00000000 
        SMWDTHROSC_ST2    =  0x48, //RW    32     0x00000000 
        SMWDTHROSC_ST3    =  0x4C, //RW    32     0x00000000 
        SMWDTHROSC_STLOAD =  0x50, //RO    32     0x00000001 
        SMWDTHROSC_STCC   =  0x54, //RW    32     0x00000038 
        SMWDTHROSC_STCS   =  0x58, //RW    32     0x00000000 
        SMWDTHROSC_STCV0  =  0x5C, //RO    32     0x00000000 
        SMWDTHROSC_STCV1  =  0x60, //RO    32     0x00000000 
        SMWDTHROSC_STCV2  =  0x64, //RO    32     0x00000000 
        SMWDTHROSC_STCV3  =  0x68, //RO    32     0x00000000 
    };

    static volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(TSC_BASE)[o / sizeof(Reg32)]; }

public:
    using ARMv7_TSC::Hertz;
    typedef Reg32 Time_Stamp;

public:
    static Hertz frequency() { return CLOCK; }

    static volatile Time_Stamp time_stamp()
    {
        Time_Stamp ret = 0;
        // Ensuring read order. The time read from ST1-3 is latched at the moment ST0 is read
        ret += reg(SMWDTHROSC_ST0);
        ret += reg(SMWDTHROSC_ST1) << 8;
        ret += reg(SMWDTHROSC_ST2) << 16;
        ret += reg(SMWDTHROSC_ST3) << 24;
        
        return _positive_offset ? (ret + _offset) : (ret - _offset);
    }

    // Positive flag accounts for unsigned values
    // Not setting a default value for "positive" argument reminds the programmer of this
    static void offset(Time_Stamp o, bool positive) { _offset = o; _positive_offset = positive; }

private:
    static Time_Stamp _offset;
    static bool _positive_offset;
};

__END_SYS

#endif
