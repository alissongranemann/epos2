// EPOS Kernel Binding

#include <framework/main.h>
#include <framework/agent.h>

__BEGIN_SYS

IPC::Observed IPC::_observed;

Agent::Member Agent::_handlers[];

void Agent::init()
{
    _handlers[THREAD_ID]                            =       &Agent::handle_thread;
    _handlers[TASK_ID]                              =       &Agent::handle_task;
    _handlers[ACTIVE_ID]                            =       &Agent::handle_active;
    _handlers[ADDRESS_SPACE_ID]                     =       &Agent::handle_address_space;
    _handlers[SEGMENT_ID]                           =       &Agent::handle_segment;
    _handlers[MUTEX_ID]                             =       &Agent::handle_mutex;
    _handlers[SEMAPHORE_ID]                         =       &Agent::handle_semaphore;
    _handlers[CONDITION_ID]                         =       &Agent::handle_condition;
    _handlers[CLOCK_ID]                             =       &Agent::handle_clock;
    _handlers[ALARM_ID]                             =       &Agent::handle_alarm;
    _handlers[CHRONOMETER_ID]                       =       &Agent::handle_chronometer;
    _handlers[IPC_COMMUNICATOR_ID]                  =       &Agent::handle_ipc;
    _handlers[UTILITY_ID]                           =       &Agent::handle_utility;
}


__END_SYS

__USING_SYS;
extern "C"
{
    void _exec(void * m) { reinterpret_cast<Agent *>(m)->exec(); }

    void _agent_init()
    {
        Agent::init();
    }

}
