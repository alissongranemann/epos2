#ifndef __emote3_adc_h_
#define __emote3_adc_h_

__BEGIN_SYS

class ADC : private Cortex_M_Model {
public:
    typedef CPU::Reg32 Reg32;

    enum Single {
        SINGLE0 = 0,
        SINGLE1 = 1,
        SINGLE2 = 2,
        SINGLE3 = 3,
        SINGLE4 = 4,
        SINGLE5 = 5,
        SINGLE6 = 6,
        SINGLE7 = 7,
        DIFF8 = 8,
        DIFF9 = 9,
        DIFF10 = 10,
        DIFF11 = 11,
        GND = 12,
        RESERVED = 13,
        TEMPERATURE = 14,
        AVDD5_3 = 15
    };

    enum Reference {
        INTERNAL = 0,
        EXTERNAL = 1,  // External reference on AIN7 pin
        AVDD5 = 2,
        EXTERNAL_DIFF = 3
    };

    enum Decimation {
        D64 = 0,   //  7 bit resolution
        D128 = 1,  //  9 bit resolution
        D256 = 2,  // 10 bit resolution
        D512 = 3   // 12 bit resolution
    };

    // Make a single conversion using the ADC, as documented on page 375 of
    // the user guide. AVDD5 is connected to VDD in the eMote3, so it is
    // the most reliable reference, thus it is the default argument.
    // Using the internal reference yields totally different values, meaning
    // it is different from VDD.
    static int get(
        Single channel,
        Decimation decimation = D512,
        Reference reference = AVDD5
    )
    {
        start_single_conversion(channel, decimation, reference);
        while (!single_conversion_ended());
        return read_single_conversion_result();
    }

private:
    enum {
        ADC_BASE = 0x400D7000
    };

    enum Offset {
        //Register Name      Offset  Type  Width  Reset Value
        ADCCON1           =  0x00, //RW    32     0x00000033
        ADCCON2           =  0x04, //RW    32     0x00000010
        ADCCON3           =  0x08, //RW    32     0x00000000
        ADCL              =  0x0C, //RW    32     0x00000000
        ADCH              =  0x10, //RO    32     0x00000000
    };

    enum Shifts {
        EOC_START = 7,
        SIGN_BITS = sizeof(int)*8 - 14,
        LOW_START = 2,
        LOW_SIZE = 6,
        REF_START = 6,
        DECIMATION_START = 4
    };

    enum Mask {
        ADCCON3_CLEAR = ~0xFF,
        EOC = (1 << EOC_START),
        HIGH = 0xFF,
        LOW = (0x3F << LOW_START)
    };


    static volatile Reg32 & reg(Offset o) { return reinterpret_cast<volatile Reg32 *>(ADC_BASE)[o / sizeof(Reg32)]; }

    static void start_single_conversion(Single channel, Decimation decimation, Reference reference)
    {
        auto r = reference << REF_START;
        auto d = decimation << DECIMATION_START;
        reg(ADCCON3) = (reg(ADCCON3) & ADCCON3_CLEAR) | r | d | channel;
    }

    static bool single_conversion_ended()
    {
        return reg(ADCCON1) & EOC;
    }

    static int read_single_conversion_result()
    {
        auto whole = int{read_high() | read_low()};
        return (whole << SIGN_BITS) >> SIGN_BITS;  // Extends signal from signal bit.
    }

    static unsigned int read_high()
    {
        auto r = reg(ADCH);
        auto value = (r & HIGH) << LOW_SIZE;
        return value;
    }

    static unsigned int read_low()
    {
        auto r = reg(ADCL);
        auto value = (r & LOW) >> LOW_START;
        return value;
    }
};

__END_SYS

#endif
