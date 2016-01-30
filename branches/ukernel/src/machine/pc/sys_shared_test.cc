#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Hi!" << endl;
    unsigned long * shared = reinterpret_cast<unsigned long *>(_SYS::Memory_Map<_SYS::PC>::SYS_SHARED);

    cout << "shared = " << reinterpret_cast<void *>(shared) << endl;

    cout << "(r) *shared = " << reinterpret_cast<void *>(*shared) << endl;
    cout << "(w) *shared..." << endl;
    *shared = 0xdeadbeef;
    cout << "(r) *shared = " << reinterpret_cast<void *>(*shared) << endl;

    cout << "shared = " << reinterpret_cast<void *>(shared) << endl;

    cout << "Bye!" << endl;

    // while(true);

    return 0;
}
