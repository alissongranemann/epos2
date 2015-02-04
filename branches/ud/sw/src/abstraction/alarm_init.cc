// EPOS Alarm Abstraction Initialization

#include <system.h>
#include <alarm.h>

__BEGIN_SYS

int Alarm::init()
{
    db<Init, Alarm>(TRC) << "Alarm::init()" << endl;

    _timer = new (SYSTEM) Alarm_Timer(handler);

    return 0;
}

__END_SYS
