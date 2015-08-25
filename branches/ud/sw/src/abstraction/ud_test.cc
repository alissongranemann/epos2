#include <utility/ostream.h>
#include <framework/main.h>

// FIXME: This should be transparent!
#define BIND_UD(X) typedef _SYS::IF<_SYS::Traits<_SYS::X>::hardware, _SYS::Handle<_SYS::X>, _SYS::X>::Result X;

using namespace EPOS;

OStream cout;

int main()
{
    BIND_UD(Adder)
    Adder * add;

    add = new Adder;

    cout << "Hello World!" << endl;

    cout << "3 - 4 = " << add->add(3, -4) << endl;
    cout << "-3 + 4 = " << add->add(-3, 4) << endl;
    cout << "3 + 4 = " << add->add(3, 4) << endl;
    cout << "12648448 + 12648448 = " << add->add(12648448, 12648448) << endl;
    cout << "-12648448 - 12648448 = " << add->add(-12648448, -12648448) << endl;

    delete add;

    return 0;
}
