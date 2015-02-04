// EPOS System Initializer

#include <machine.h>
#include <system.h>
#include <address_space.h>
#include <segment.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

class Init_System
{
private:
    static const unsigned int HEAP_SIZE = Traits<System>::HEAP_SIZE;

public:
    Init_System() {
        db<Init>(TRC) << "Init_System()" << endl;

        Machine::smp_barrier();

        // Only the boot CPU runs INIT_SYSTEM fully
        if(Machine::cpu_id() != 0) {
            // Wait until the boot CPU has initialized the machine
            Machine::smp_barrier();
            // For IA-32, timer is CPU-local. What about other SMPs?
            Timer::init();
            Machine::smp_barrier();
            return;
        }

        // Initialize the processor
        db<Init>(INF) << "Initializing the CPU: " << endl;
        CPU::init();
        db<Init>(INF) << "done!" << endl;

        // If EPOS is a library then adjust the application entry point (that
        // was set by SETUP) based on the ELF SYSTEM+APPLICATION image
        System_Info<Machine> * si = System::info();
        if(!si->lm.has_sys)
            si->lmm.app_entry = reinterpret_cast<unsigned int>(&__epos_library_app_entry);

        // Initialize System's heap
        db<Init>(INF) << "Initializing system's heap: " << endl;
        if(Traits<System>::multiheap) {
            System::_heap_segment = new (&System::_preheap[0]) Segment(HEAP_SIZE);
            System::_heap = new (&System::_preheap[sizeof(Segment)]) Heap(
                Address_Space(MMU::current()).attach(*System::_heap_segment, Memory_Map<Machine>::SYS_HEAP),
                System::_heap_segment->size());
        } else
            System::_heap = new (&System::_preheap[0]) Heap(MMU::alloc(MMU::pages(HEAP_SIZE)), HEAP_SIZE);
        db<Init>(INF) << "done!" << endl;

        // Initialize the machine
        db<Init>(INF) << "Initializing the machine: " << endl;
        Machine::init();
        db<Init>(INF) << "done!" << endl;

        Machine::smp_barrier(); // signalizes "machine ready" to other CPUs
        Machine::smp_barrier(); // wait for them to finish Machine::init()

        // Initialize system abstractions
        db<Init>(INF) << "Initializing system abstractions: " << endl;
        System::init();
        db<Init>(INF) << "done!" << endl;

        // Initialization continues at init_first
    }
};

// Global object "init_system" must be constructed first.
Init_System init_system;

__END_SYS
