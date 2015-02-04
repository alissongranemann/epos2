// EPOS Mult Abstraction Test Program

#include <components/mult.h>
#include <utility/ostream.h>
#include <chronometer.h>

__USING_SYS

OStream cout;

void call_mult(Mult &mult, unsigned int a, unsigned int b);

int main()
{
    //TODO Why should call this again here ??????????????
    Component_Manager::init_noc();

    Mult mult(Component_Manager::dummy_channel,Component_Manager::dummy_channel,0);

    cout << "\nMult Test\n\n";

    call_mult(mult, 0, 0);
    call_mult(mult, 0, 10);
    call_mult(mult, 10, 0);
    call_mult(mult, 1, 10);
    call_mult(mult, 10, 1);
    call_mult(mult, 10, 5);
    call_mult(mult, 5, 10);
    call_mult(mult, 10, 10);

    cout << "\nThe End\n";
    *((volatile unsigned int*)0xFFFFFFFC) = 0;

    return 0;
}

Chronometer chrono;

void call_mult(Mult &mult, unsigned int a, unsigned int b){
    cout << "Calling mult.mult(" << a << ", " << b << ")\n";
    chrono.reset();
    chrono.start();
    unsigned int result = mult.mult(a,b);
    chrono.stop();
    cout << "Result = " << result << " (in " << chrono.ticks() << " cycles)\n";
}
