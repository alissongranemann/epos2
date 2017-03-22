#include <transducer.h>
#include <tstp.h>
#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Smart Plug" << endl;
    cout << "Here = " << TSTP::here() << endl;

    Current c0(0, 1000000, Current::ADVERTISED);
    //Current c1(1, 1000000, Current::ADVERTISED);

    Thread::self()->suspend();

    return 0;
}
