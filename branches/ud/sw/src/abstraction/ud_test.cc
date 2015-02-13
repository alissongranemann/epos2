// EPOS Hello World Program

#include <utility/ostream.h>
//#include <adder.h>

using namespace EPOS;

OStream cout;

int main()
{
    _SYS::Handle<_SYS::Adder> add;

    cout << "Hello World!" << endl;
    cout << "1 + 2 = " << add.add(1, 2) << endl;
}
