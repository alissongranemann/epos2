// EPOS Hello World Program

#include <utility/ostream.h>
#include <gpio.h>
#include <chronometer.h>
#include <adder.h>

using namespace EPOS;

const unsigned int N = 1000;

OStream cout;

void save_st(_SYS::Handle<_SYS::Adder> & add)
{
    _SYS::TSC_Chronometer chrono;
    unsigned long long dur = 0;
    unsigned int st[200];

    for (int i = 0; i < N; i++) {
        chrono.reset();
        chrono.start();

        int len = add.get_st_len();

        for (int i = 0; i < len; i++)
            st[i] = add.save_st();

        chrono.stop();
        dur += chrono.ticks();
    }

    dur /= N;

    cout << "State saved in " << dur/(666666687/1000000) << " us" << endl;
    cout << "State saved in " << dur << " ticks" << endl;
}

int main()
{
    _SYS::Handle<_SYS::Adder> add;
    GPIO led(61);
    GPIO g0(62);
    GPIO g1(63);

    led.output();
    g0.output();
    g1.output();

    led.put(true);

    cout << "State saving test with two traffic generator" << endl;

    g0.put(false);
    g1.put(false);
    save_st(add);

    g0.put(true);
    g1.put(false);
    save_st(add);

    g0.put(true);
    g1.put(true);
    save_st(add);

    return 0;
}
