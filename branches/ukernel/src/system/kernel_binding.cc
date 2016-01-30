// EPOS Kernel Binding

#include <framework/main.h>
#include <framework/agent.h>
#include <framework/message.h>

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
    _handlers[MACHINE_ID]                           =       &Agent::handle_machine;
    _handlers[THIS_THREAD_ID]                       =       &Agent::handle_this_thread;
    _handlers[IP_ID]                                =       &Agent::handle_ip;
    _handlers[NETWORK_ID]                           =       &Agent::handle_network;
    _handlers[NIC_ID]                               =       &Agent::handle_nic;
    _handlers[CPU_ID]                               =       &Agent::handle_cpu;
    _handlers[PEDF_ID]                              =       &Agent::handle_pedf;
    _handlers[PERIODIC_THREAD_ID]                   =       &Agent::handle_periodic_thread;
    _handlers[THREAD_CONFIGURATION_ID]              =       &Agent::handle_thread_configuration;
    _handlers[PERIODIC_THREAD_CONFIGURATION_ID]     =       &Agent::handle_periodic_thread_configuration;
}


__END_SYS

__USING_SYS;
extern "C"
{
    void _exec(void * m)
    {
        if (Traits<Thread>::smp) {
            if ((reinterpret_cast<Agent *>(m)->id().type() != MACHINE_ID) || (reinterpret_cast<Agent *>(m)->method() != Message::MACHINE_SMP_BARRIER)) {
                // db<void>(WRN) << "BKL: will try to lock!" << endl;
                Thread::lock();
                // db<void>(WRN) << "BKL: locked!" << endl;
            }
        } // "inside kernel" Big Kernel Lock

        reinterpret_cast<Agent *>(m)->exec();

        if (Traits<Thread>::smp) {
            if ((reinterpret_cast<Agent *>(m)->id().type() != MACHINE_ID) || (reinterpret_cast<Agent *>(m)->method() != Message::MACHINE_SMP_BARRIER)) {
                // db<void>(WRN) << "BKL: will unlock, now!" << endl;
                Thread::unlock();
            }
        } // "inside kernel" Big Kernel Lock
        /* NOTE:
            * Suppose thread A is running.
            * Suppose a timed scheduling algorithm (e.g RR).
            * Suppose a reschedule happens before iret of IA32::syscalled and
            * thread B gets schedule.
            * Suppose thread B makes a system call.
            * Suppose thread A is scheduled again before B returns from the
            * system call.
            * The iret will be executed returning to the user stack of thread B
            * instead of thread A!
            *
            * Maybe there is the need for an "outside kernel" Big Kernel Lock
            * (in addition to this one).
        */
    }


    void _agent_init()
    {
        Agent::init();
    }
}
