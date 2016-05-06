// EPOS PC Keyboard Mediator Initialization

#include <ic.h>
#include <keyboard.h>

__BEGIN_SYS

void PC_Keyboard::init()
{
    db<Init, Keyboard>(TRC) << "Keyboard::init()" << endl;

    Engine::leds(0);
    Engine::flush();
    Engine::int_enable();

    IC::int_vector(IC::INT_KEYBOARD, int_handler);
    IC::enable(IC::INT_KEYBOARD);
}

__END_SYS
