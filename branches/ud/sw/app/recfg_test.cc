// Profiles method invocation of two reconfigurable components and tests the
// FPGA's dynamic reconfiguration by creating new components dynamically in
// different orders

#include <utility/ostream.h>
#include <chronometer.h>
#include <components/add.h>
#include <components/mult.h>

__USING_SYS

OStream cout;
Chronometer chrono;

void prof_add(Add * add, unsigned int a, unsigned int b) {
    cout << "Profiling Add.add(" << a << ", " << b << ")" << endl;

    chrono.reset();
    chrono.start();
    unsigned int result = add->add(a,b);
    chrono.stop();

    cout << "Result = " << result << " (in " << chrono.read() << " us)" << endl;
}

void prof_mult(Mult * mult, unsigned int a, unsigned int b) {
    cout << "Profiling Mult.mult(" << a << ", " << b << ")" << endl;

    chrono.reset();
    chrono.start();
    unsigned int result = mult->mult(a,b);
    chrono.stop();

    cout << "Result = " << result << " (in " << chrono.read() << " us)" << endl;
}

int main()
{
    Add * add;
    Mult * mult;

    add = new Add(Component_Manager::dummy_channel, Component_Manager::dummy_channel);
    mult = new Mult(Component_Manager::dummy_channel, Component_Manager::dummy_channel);

    prof_add(add, 2, 3);
    prof_mult(mult, 2, 3);

    delete add;
    delete mult;

    mult = new Mult(Component_Manager::dummy_channel, Component_Manager::dummy_channel);
    add = new Add(Component_Manager::dummy_channel, Component_Manager::dummy_channel);

    prof_mult(mult, 2, 3);
    prof_add(add, 2, 3);

    return 0;
}
