#include <utility/ostream.h>
#include <thread.h>

using namespace EPOS;

Thread* t;
OStream cout;

int lan(int x, int y, int z)
{
    cout << reinterpret_cast<void *>(x) << ":" << reinterpret_cast<void *>(y) << ":" << reinterpret_cast<void *>(z) << endl;
    Assert((x == 0xA) && (y == 0xB) && (z == 0xC));
    return 0;
}

int tor(int x, int y, int z, int w)
{
    cout << reinterpret_cast<void *>(x) << ":" << reinterpret_cast<void *>(y) << ":" << reinterpret_cast<void *>(z) << ":" << reinterpret_cast<void *>(w) << endl;
    Assert((x == 0xA) && (y == 0xB) && (z == 0xC) && (w == 0xD));
    return 0;
}

int foo(int x, char y, char z, int w)
{
    cout << reinterpret_cast<void *>(x) << ":" << y << ":" << z << ":" << reinterpret_cast<void *>(w) << endl;
    Assert((x == 0xA) && (y == 'B') && (z == 'C') && (w == 0xD));
    return 0;
}


int bar(char u,  int v, char w)
{
    cout << u << ":" << reinterpret_cast<void *>(v) << ":" << w << endl;
    Assert((u == 'A') && (v == 0xB) && (w == 'C'));
    return 0;
}


int main()
{
    cout << "Hi!" << endl;
    
    cout << "Calling lan" << endl;
    lan(0xA, 0xB, 0xC);
    cout << "Creating a thread to call lan" << endl;
    t = new Thread(&lan, 0xA, 0xB, 0xC);
    t->join();
    delete t;

    cout << "Calling tor" << endl;
    tor(0xA, 0xB, 0xC, 0xD);
    cout << "Creating a thread to call tor" << endl;
    t = new Thread(&tor, 0xA, 0xB, 0xC, 0xD);
    t->join();
    delete t;

    cout << "Calling foo" << endl;
    foo(0xA, 'B', 'C', 0xD);
    cout << "Creating a thread to call foo" << endl;
    t = new Thread(&foo, 0xA, 'B', 'C', 0xD);
    t->join();
    delete t;

    cout << "Calling bar" << endl;
    bar('A', 0xB, 'C');
    cout << "Creating a thread to call bar" << endl;
    t = new Thread(&bar, 'A', 0xB, 'C');
    t->join();
    delete t;

    cout << "Bye!" << endl;

    return 0;
}
