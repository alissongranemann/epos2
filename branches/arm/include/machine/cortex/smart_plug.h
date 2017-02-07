// EPOS Cortex Smart Plug Mediator Declarations

#ifndef __cortex_smart_plug_h
#define __cortex_smart_plug_h

#include <adc.h>
#include <machine.h>
#include <utility/observer.h>

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
        long i_avg = 0, ref, //p_avg = 0, v[N], v_shift[N],
             i[N], i_rms = 0;//p[N];

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

            Machine::delay(1000000/SAMP_FREQ);
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

class Smart_Plug
{
private:
    friend class Machine;
    typedef Power_Meter Engine;

public:
    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    Smart_Plug() {}

    static unsigned int current(unsigned int dev) {
        assert((dev >= 0) && (dev <= 1));
        return _dev[dev]->average();
    }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
    static Engine * _dev[2];
};

__END_SYS

#endif
