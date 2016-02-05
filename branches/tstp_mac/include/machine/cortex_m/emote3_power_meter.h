#ifndef __emote3_power_meter_h
#define __emote3_power_meter_h

#include <adc.h>
#include "timer.h"

__BEGIN_SYS

class Power_Meter
{
private:
    static const int SAMP_FREQ = 2400;
    static const int N = 128;
    static const int N_ERR = 11;

    typedef ADC::Channel Channel;

public:
    Power_Meter(Channel v_chan, Channel i_chan, Channel ref_chan):
        _v_chan(v_chan), _i_chan(i_chan), _ref_chan(ref_chan) {}

    ~Power_Meter() {}

    // Calculate the average electrical power of sinusoidal signals using the
    // following equation: integrate (from 0 to T) { v[t]*i[t]*dt }
    unsigned int average() {
        int j;
        long i_avg = 0, p_avg = 0, ref, //v[N], v_shift[N], 
             i[N], p[N], i_rms = 0;

        ref = _ref_chan.read();

        for(j = 0; j < N; j++) {
            i[j] = _i_chan.read() - ref;
            //v[j] = _v_chan.read() - ref;

            // The measured voltage stored in v[] is sinusoidal signal being
            // rectified by a diode. v_shift[] is an approximation of the
            // blocked half signal based on the measured voltage and is used to
            // reconstruct the complete signal.
            //v_shift[j + N_ERR] = v[j];

            i_avg += i[j];

            eMote3_GPTM::delay(1000000/SAMP_FREQ);
        }

        i_avg /= N;

        for(j = 0; j < N; j++) {
            // Rebuild the complete voltage signal
            //v[j] = v[j] - v_shift[j];
            // Remove DC bias from i[]
            i[j] = i[j] - i_avg;
            i_rms += i[j] * i[j];
            // Calculate instant power
            //p[j] = v[j]*i[j];
        }
        i_rms /= N;

        // Remove the 11 first voltage samples, that aren't properly
        // reconstructed
        //for(j = N_ERR; j < N; j++) {
        //    p_avg += p[j];
        //}

        //p_avg = p_avg/(N - N_ERR);

        if(i_rms < 0)
            return 0;
        else
            // Conversion constants to watts
            return i_rms;
        //if(p_avg < 0)
        //    return 0;
        //else
        //    // Conversion constants to watts
        //    return p_avg/3912;
    }

private:
    ADC _v_chan;
    ADC _i_chan;
    ADC _ref_chan;
};

__END_SYS

#endif
