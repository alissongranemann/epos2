// EPOS Thread Abstraction Initialization

#include <system.h>
#include <thread.h>
#include <alarm.h>
#include <task.h>

__BEGIN_SYS

void Thread::init()
{
    int (* entry)() = reinterpret_cast<int (*)()>(System::info()->lmm.app_entry);

    db<Init, Thread>(TRC) << "Thread::init(entry=" << reinterpret_cast<void *>(entry) << ")" << endl;

    // The installation of the scheduler timer handler must precede the
    // creation of threads, since the constructor can induce a reschedule
    // and this in turn can call timer->reset()
    // Letting reschedule() happen during thread creation is harmless, since
    // MAIN is created first and dispatch won't replace it nor by itself
    // neither by IDLE (that has a lower priority)

	if(Criterion::timed && (Machine::cpu_id() == 0))
        _timer = new (SYSTEM) Scheduler_Timer(QUANTUM, time_slicer);

    if(Machine::cpu_id() == 0)
        IC::int_vector(IC::INT_RESCHEDULER, rescheduler);
    //IC::enable(IC::INT_RESCHEDULER);
    Machine::smp_barrier();

	CPU::int_enable();

    Thread * first;

    if(Machine::cpu_id() == 0) {
        if(multitask) {
            // Create the application's main thread
            // This must precede idle, thus avoiding implicit rescheduling
            first = new (SYSTEM) Thread(*Task::_master, entry, RUNNING, MAIN);
            new (SYSTEM) Thread(*Task::_master, &idle, READY, IDLE);
        } else {
            first = new (SYSTEM) Thread(entry, RUNNING, MAIN);
            new (SYSTEM) Thread(&idle, READY, IDLE);
        }
    } else {
        if(multitask)
            first = new (SYSTEM) Thread(*Task::_master, &idle, RUNNING, IDLE);
        else
            first = new (SYSTEM) Thread(&idle, RUNNING, IDLE);
    }

    db<Init, Thread>(INF) << "Dispatching the first thread: " << first << endl;

    This_Thread::not_booting();

    // This barrier is particularly important, since afterwards the temporary stacks
    // and data structures established by SETUP and announced as "free memory" will indeed be
    // available to user threads
    Machine::smp_barrier();

    first->_context->load();
}

__END_SYS
