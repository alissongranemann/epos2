// EPOS First Thread Initializer

#include <utility/heap.h>
#include <system.h>
#include <thread.h>
#include <alarm.h> // for FCFS
#include <task.h>

extern "C" { void __epos_app_entry(); }

__BEGIN_SYS

class Init_First
{
private:
    typedef int (* Main1)();
    typedef int (* Main2)(int argc, char * argv[]);

    typedef CPU::Log_Addr Log_Addr;

public:
    Init_First() {

        db<Init>(TRC) << "Init_First()" << endl;

        Machine::smp_barrier();

        if(!Traits<System>::multithread) {
            CPU::int_enable();
            return;
        }

        db<Init>(INF) << "Initializing the first process: " << endl;

        System_Info<Machine> * si = System::info();
        Thread * first;

        if(Traits<System>::multitask) {
            if(Machine::cpu_id() == 0) {
                new (SYSTEM) Task (new (SYSTEM) Address_Space(MMU::current()),
                                   new (SYSTEM) Segment(Log_Addr(si->lm.app_code), si->lm.app_code_size, Segment::Flags::APP),
                                   new (SYSTEM) Segment(Log_Addr(si->lm.app_data), si->lm.app_data_size, Segment::Flags::APP),
                                   reinterpret_cast<Main2>(si->lm.app_entry),
                                   Log_Addr(Memory_Map<Machine>::APP_CODE), Log_Addr(Memory_Map<Machine>::APP_DATA),
                                   static_cast<int>(si->lm.app_extra_size), reinterpret_cast<char **>(si->lm.app_extra));

                // Detach extra memory attached by SETUP for single-task configurations
    //            MMU::PT_Entry * sys_pd = reinterpret_cast<MMU::PT_Entry *>(Task::self()->_as->pd() | Memory_Map<Machine>::PHY_MEM);
    //            unsigned int from = MMU::directory(MMU::align_directory(Task::self()->_ds->phy_address() + Task::self()->_ds->size())) + 1;
    //            unsigned int to = MMU::directory(MMU::align_directory(Memory_Map<Machine>::PHY_MEM)) - 1;
    //            db<Init, Task>(INF) << "Task::init: detaching memory from entries " << hex << from << " to " << to << " of PD at " << sys_pd << endl;
    //            for(unsigned int i = from; i < to; i++)
    //                sys_pd[i] = 0;

                // Thread::self() and Task::self() can be safely called after the construction of the Main task.
                first = Thread::self();

                *Thread::_running_thread = reinterpret_cast<int>(first);

                // Idle thread creation must succeed main, thus avoiding implicit rescheduling.
                new (SYSTEM) Thread(Thread::Configuration(Thread::READY, Thread::IDLE), &Thread::idle);

            } else {
                /* A task for each core...
                TODO:
                new (SYSTEM) Task (new (SYSTEM) Address_Space(MMU::current()),
                                   new (SYSTEM) Segment(Log_Addr(si->lm.app_code), si->lm.app_code_size, Segment::Flags::APP),
                                   new (SYSTEM) Segment(Log_Addr(si->lm.app_data), si->lm.app_data_size, Segment::Flags::APP),
                                   &Thread::idle,
                                   Log_Addr(Memory_Map<Machine>::APP_CODE), Log_Addr(Memory_Map<Machine>::APP_DATA),
                                   static_cast<int>(si->lm.app_extra_size), reinterpret_cast<char **>(si->lm.app_extra));

                first = Thread::self();
                */
                // Only one task...
                first = new (SYSTEM) Thread(Thread::Configuration(Thread::READY, Thread::IDLE), &Thread::idle);
            }
        } else {
            if(Machine::cpu_id() == 0) {
                // If EPOS is not a kernel, then adjust the application entry point to __epos_app_entry,
                // which will directly call main(). In this case, _init will have already been called,
                // before Init_Application, to construct main()'s global objects.
                first = new (SYSTEM) Thread(Thread::Configuration(Thread::RUNNING, Thread::MAIN), reinterpret_cast<int (*)()>(__epos_app_entry));

                // Idle thread creation must succeed main, thus avoiding implicit rescheduling.
                new (SYSTEM) Thread(Thread::Configuration(Thread::READY, Thread::IDLE), &Thread::idle);
            } else {
                first = new (SYSTEM) Thread(Thread::Configuration(Thread::READY, Thread::IDLE), &Thread::idle);
            }
        }

        if(si->lm.has_ext)
            db<Init>(INF) << "Init_First: additional data from mkbi at "  << reinterpret_cast<void *>(si->lm.app_extra) << ":" << si->lm.app_extra_size << endl;

        Machine::smp_barrier();

        db<Init>(INF) << "done!" << endl;

        db<Init>(INF) << "INIT ends here!" << endl;

        db<Init, Thread>(INF) << "Dispatching the first thread: " << first << endl;

        This_Thread::not_booting();

        // This barrier is particularly important, since afterwards the temporary stacks
        // and data structures established by SETUP and announced as "free memory" will indeed be
        // available to user threads
        Machine::smp_barrier();

        first->_context->load();
    }
};

// Global object "init_first" must be constructed last in the context of the
// OS, for it activates the first application thread (usually main())
Init_First init_first;

__END_SYS
