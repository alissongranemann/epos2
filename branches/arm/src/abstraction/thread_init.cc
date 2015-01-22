// EPOS Thread Abstraction Initialization

#include <system.h>
#include <thread.h>
#include <alarm.h>
#include <task.h>

extern "C" { void __epos_app_entry(); }

__BEGIN_SYS

void Thread::init()
{
    // If EPOS is a library, then adjust the application entry point to __epos_app_entry,
    // which will directly call main(). In this case, _init will have already been called,
    // before Init_Application, to construct main()'s global objects.
    int (* entry)();
    if(Traits<Build>::MODE == Traits<Build>::LIBRARY)
        entry = reinterpret_cast<int (*)()>(__epos_app_entry);
    else
        entry = reinterpret_cast<int (*)()>(System::info()->lm.app_entry);

    db<Init, Thread>(TRC) << "Thread::init(entry=" << reinterpret_cast<void *>(entry) << ")" << endl;

    // The installation of the scheduler timer handler must precede the
    // creation of threads, since the constructor can induce a reschedule
    // and this in turn can call timer->reset()
    // Letting reschedule() happen during thread creation is harmless, since
    // MAIN is created first and dispatch won't replace it nor by itself
    // neither by IDLE (that has a lower priority)
    if(Criterion::timed && (Machine::cpu_id() == 0))
        _timer = new (SYSTEM) Scheduler_Timer(QUANTUM, time_slicer);

    if(smp) {
        if(Machine::cpu_id() == 0)
            IC::int_vector(IC::INT_RESCHEDULER, rescheduler);
        IC::enable(IC::INT_RESCHEDULER);

        Machine::smp_barrier();
    }

    Thread * first;
    if(Machine::cpu_id() == 0) {
        if(multitask) {
            // Create the application's main thread
            // This must precede idle, thus avoiding implicit rescheduling
            first = new (SYSTEM) Thread(Configuration(RUNNING, MAIN, Traits<System>::STACK_SIZE, reinterpret_cast<char *>(System::info()->lm.app_heap)), *Task::_master, entry);
            new (SYSTEM) Thread(Configuration(READY, IDLE, Traits<System>::STACK_SIZE), *Task::_master, &idle);
        } else {
            first = new (SYSTEM) Thread(Configuration(RUNNING, MAIN), entry);
            new (SYSTEM) Thread(Configuration(READY, IDLE), &idle);
        }
    } else {
        if(multitask)
            new (SYSTEM) Thread(Configuration(READY, IDLE, Traits<System>::STACK_SIZE), *Task::_master, &idle);
        else
            first = new (SYSTEM) Thread(Configuration(READY, IDLE), &idle);
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
