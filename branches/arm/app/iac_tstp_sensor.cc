#include <machine.h>
#include <thread.h>
#include <smart_data.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP IAC Sensor test" << endl;

    cout << "My coordinates are " << TSTP::here() << endl;
    cout << "The time now is " << TSTP::now() << endl;
    cout << "I am" << (TSTP::here() == TSTP::sink() ? " " : " not ") << "the sink" << endl;

    Acceleration a(0, 1000000, Acceleration::ADVERTISED);

    Thread::self()->suspend();

    return 0;
}
