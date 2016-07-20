// EPOS System Abstraction Initialization

#include <system.h>
#include <alarm.h>
#include <task.h>
#include <network.h>

__BEGIN_SYS

void System::init()
{
    if (Traits<Alarm>::enabled)
        Alarm::init();

    if (Traits<Thread>::enabled)
        Thread::init();

    if (Traits<Network>::enabled)
        Network::init();
}

__END_SYS
