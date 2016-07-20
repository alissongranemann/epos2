#include <utility/ostream.h>
#include <ic.h>
#include <thread.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;


void dummy_keyboard_int_handler(const IC::Interrupt_Id &)
{
    cout << "hi";
    // IC::enable(IC::INT_KEYBOARD);
}


int main()
{
    cout << "Hello EPOS!" << endl;

    // CPU::int_disable();
    IC::enable();
    // IC::int_vector(IC::INT_KEYBOARD, &dummy_keyboard_int_handler);
    // IC::enable(IC::INT_KEYBOARD);

    // CPU::int_enable();

    // cout << "Bye!" << endl;

    while(true) { Alarm::delay(2000000); cout << " "; Thread::yield(); }

    return 0;
}
