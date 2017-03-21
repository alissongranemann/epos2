#include <transducer.h>
#include <tstp.h>
#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Smart Door" << endl;
    cout << "Here = " << TSTP::here() << endl;

    GPIO in0('D', 2, GPIO::IN);
    GPIO in1('D', 4, GPIO::IN);
    GPIO door_control('D', 1, GPIO::OUT);
    GPIO button('A', 2, GPIO::IN);

    Door_Sensor ds0(0, &door_control, &button, 0, &in0, &in1);
    Door d0(0, 10 * 60 * 1000000, Door::ADVERTISED);

    Thread::self()->suspend();

    return 0;
}
