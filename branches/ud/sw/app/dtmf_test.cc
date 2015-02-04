#include <utility/ostream.h>
#include <components/dtmf_detector.h>
#include <tsc.h>
#include <chronometer.h>

__USING_SYS

int main()
{
    OStream cout;
    DTMF_Detector dtmf(Component_Manager::dummy_channel, Component_Manager::dummy_channel);
    Chronometer chrono;
    Implementation::DTMF_Detector::sample_t sample;
    char tone;

    cout << "Profiling DTMF_Detector" << endl;

    chrono.reset();
    chrono.start();
    dtmf.add_sample(sample);
    chrono.stop();

    cout << "add_sample(sample) (in " << chrono.read() << " us)" << endl;

    chrono.reset();
    chrono.start();
    tone = dtmf.do_dtmf_detection();
    chrono.stop();

    cout << "do_dtmf_detection() = " << tone << " (in " << chrono.read()
        << " us)" << endl;

    TSC::Time_Stamp stamps[7];
    dtmf.enter_recfg(&(stamps[0]));

    cout << "ticks_acquire      " << stamps[0] << endl;
    cout << "ticks_get_state    " << stamps[1] << endl;
    cout << "ticks_alloc_hw_res " << stamps[2] << endl;
    cout << "ticks_load_comp    " << stamps[3] << endl;
    cout << "ticks_set_state    " << stamps[4] << endl;
    cout << "ticks_set_domain   " << stamps[5] << endl;
    cout << "ticks_release      " << stamps[6] << endl;

    return 0;
}
