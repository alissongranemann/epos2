#include <utility/ostream.h>
#include <components/adpcm_codec.h>
#include <tsc.h>
#include <chronometer.h>

__USING_SYS

static const int PCM = 42;

int main()
{
    OStream cout;
    ADPCM_Codec adpcm(Component_Manager::dummy_channel, Component_Manager::dummy_channel);
    Chronometer chrono;
    int pcm = PCM;
    unsigned int adpcm;

    cout << "Profiling ADPCM_Codec" << endl;

    chrono.reset();
    chrono.start();
    adpcm = codec.encode(pcm);
    chrono.stop();

    cout << "encode(pcm) = " << adpcm << " (in " << chrono.read() << " us)"
        << endl;

    chrono.reset();
    chrono.start();
    pcm = codec.decode(adpcm);
    chrono.stop();

    cout << "decode(adpcm) = " << pcm << " (in " << chrono.read() << " us)"
        << endl;

    if(pcm != PCM)
        cout << "Decoding failed: " << pcm << "!= " << PCM << endl;

    TSC::Time_Stamp stamps[7];
    codec.enter_recfg(&(stamps[0]));

    cout << "ticks_acquire      " << stamps[0] << endl;
    cout << "ticks_get_state    " << stamps[1] << endl;
    cout << "ticks_alloc_hw_res " << stamps[2] << endl;
    cout << "ticks_load_comp    " << stamps[3] << endl;
    cout << "ticks_set_state    " << stamps[4] << endl;
    cout << "ticks_set_domain   " << stamps[5] << endl;
    cout << "ticks_release      " << stamps[6] << endl;

    return 0;
}
