// EPOS Cortex-M ADC Mediator Declarations

#ifndef __cortex_m_adc_h_
#define __cortex_m_adc_h_

#include <machine.h>
#include <adc.h>

__BEGIN_SYS

class Cortex_M_ADC : private ADC_Common, private Cortex_M_Model
{
public:
    enum Channel {
        SINGLE_ENDED_ADC0 = 0,
        SINGLE_ENDED_ADC1 = 1,
        SINGLE_ENDED_ADC2 = 2,
        SINGLE_ENDED_ADC3 = 3,
        SINGLE_ENDED_ADC4 = 4,
        SINGLE_ENDED_ADC5 = 5,
        SINGLE_ENDED_ADC6 = 6,
        SINGLE_ENDED_ADC7 = 7,
        DIFF8             = 8,
        DIFF9             = 9,
        DIFF10            = 10,
        DIFF11            = 11,
        GND               = 12,
        TEMPERATURE       = 14,
        AVDD5_3           = 15,
    };

    enum Reference {
        INTERNAL_REF   = 0,
        EXTERNAL_REF   = 1, // External reference on AIN7 pin
        SYSTEM_REF     = 2,
        EXTERNAL_DIFF  = 3
    };

    enum Decimation {
        D64  = 0,  //  7 bit resolution
        D128 = 1,  //  9 bit resolution
        D256 = 2,  // 10 bit resolution
        D512 = 3   // 12 bit resolution
    };

    Cortex_M_ADC(const Channel & channel = SINGLE_ENDED_ADC0, const Reference & reference = SYSTEM_REF, const Decimation & decimation = D512) :
        _channel(channel), _reference(reference), _decimation(decimation)
    {
        Cortex_M_Model::adc_config(_channel);
    }

    short read()
    {
        reg(ADCCON3) = (_reference * ADCCON3_EREF) | (_decimation * ADCCON3_EDIV) | (_channel * ADCCON3_ECH);
        while(!reg(ADCCON1) & ADCCON1_EOC);
        short ret = (reg(ADCH) << 8) + reg(ADCL); // TODO: do we need to >> 2 ?
        switch(_decimation) {
            case D64:  ret &= 0xfe0; break;
            case D128: ret &= 0xff8; break;
            case D256: ret &= 0xffc; break;
            case D512: break;
        }
        return ret;
    }

private:
    volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32*>(ADC_BASE)[o / sizeof(Reg32)]; }

    Channel _channel;
    Reference _reference;
    Decimation _decimation;
};

__END_SYS

#endif
