#include <alarm.h>
#include <machine/cortex_m/emote3.h>

using namespace EPOS;

class Power_Meter
{
private:
    static const int DEFAULT_FREQ = 2000;
    static const int N_S = 128;

    typedef ADC::Single Single;

public:
    Power_Meter(Single v_chan, Single i_chan, int i_bias = 2926):
        _v_chan(v_chan), _i_chan(i_chan), _i_bias(i_bias) {}

    ~Power_Meter() {}

    // Calculate the average electrical power of sinusoidal signals using the
    // following equation: (1/T)*(integral from j=0 to j=T)(v(j)*i(j)).
    int sample() {
        int j;
        int v[N_S], v_shift[N_S], i[N_S];
        long i_avg = 0, p_avg = 0, p[N_S];

        for(j = 0; j < N_S; j++) {
            // Sample V and I
            i[j] = ADC::get(_i_chan) - _i_bias;
            v[j] = ADC::get(_v_chan);

            // The measured voltage stored in v[] is sinusoidal signal being
            // rectified by a diode. v_shift[] is an approximation of the
            // blocked half signal based on the measured voltage and is used to
            // reconstruct the complete signal.
            v_shift[(j + DEFAULT_FREQ/(2*60))%N_S] = v[j];

            i_avg += i[j];

            Alarm::delay(1000000/DEFAULT_FREQ);
        }

        i_avg = i_avg/N_S;

        for(j = 0; j < N_S; j++) {
            // Rebuild the complete voltage signal
            v[j] = v[j] - v_shift[j];
            // Remove DC bias from i[]
            i[j] = i[j] - i_avg;
            // Calculate instant power
            p[j] = v[j]*i[j];
        }

        // Remove glitch?
        for(j = 19; j < N_S; j++) {
            p_avg += p[j];
        }

        p_avg = p_avg/(N_S - 20);

        return p_avg;
    }

private:
    int _i_bias;
    Single _v_chan;
    Single _i_chan;
};

int main()
{
    Power_Meter pm(ADC::SINGLE0, ADC::SINGLE1);
    NIC * nic = new NIC();
    char data[2];
    int p;

    while(1) {
        p = pm.sample();

        data[0] = p&0xff;
        data[1] = (p>>8)&0xff;

        nic->send(nic->broadcast(), 0x1010, data, sizeof data);

        Alarm::delay(1000000);
    }

    return 0;
}
