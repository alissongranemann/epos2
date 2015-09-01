#include <utility/ostream.h>
#include <framework/main.h>

using namespace EPOS;

OStream cout;

int main()
{
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
