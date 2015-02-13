// EPOS System Abstraction Initialization

#include <system.h>
#include <alarm.h>
#include <task.h>
#include <component_manager.h>

__BEGIN_SYS

void System::init()
{
    if(Traits<Alarm>::enabled)
        Alarm::init();

    if(Traits<Task>::enabled)
        Task::init();

    if(Traits<Component_Manager>::enabled)
        Component_Manager::init();
}

__END_SYS
