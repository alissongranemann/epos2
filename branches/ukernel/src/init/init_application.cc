// EPOS Application Initializer

#include <utility/heap.h>
#include <mmu.h>
#include <machine.h>
#include <application.h>

extern "C" { char _end; } // defined by GCC
extern "C" { unsigned int _cpu_id(); }
extern "C" { void _smp_barrier(); }

__BEGIN_SYS

class Init_Application
{
private:
    static const unsigned int HEAP_SIZE = Traits<Application>::HEAP_SIZE;
    static const unsigned int STACK_SIZE = Traits<Application>::STACK_SIZE;

public:
    Init_Application() {
        db<Init>(TRC) << "Init_Application()" << endl;

        /// _smp_barrier();
        /* As far I could notice, only the main thread will
        * enter here.
        * APs' entry point is Thread::idle so APs will not enter here when the
        * system is starting.
        *
        * A AP can enter here if a new thread is created an assigned to it (AP)
        * and the thread entry point is application _start.
        * In that case, usually the new thread belongs to another task and it
        * should run Init_Application so the task can have its own heap.
        *
        * In any case, the smp_barrier seems unnecessary.
        * Therefore, commenting it. */

        // Only the boot CPU runs INIT_APPLICATION.
        /* That is true even in KERNEL mode taking into consideration that there
         * is a single master task for the whole system.
         * In the case that there is a master task for each CPU, then APs must
         * run INIT_APPLICATION as well. */
        if(_cpu_id() != 0)
            return;

        // Initialize Application's heap
        db<Init>(INF) << "Initializing application's heap: " << endl;
        if(Traits<System>::multiheap) { // heap in data segment arranged by SETUP
            char * heap = MMU::align_page(&_end);

            if(Traits<Build>::MODE != Traits<Build>::KERNEL) // if not a kernel, then use the stack allocated by SETUP, otherwise make that part of the heap
                heap += MMU::align_page(Traits<Application>::STACK_SIZE);

            Application::_heap = new (&Application::_preheap[0]) Heap(heap, HEAP_SIZE);
        } else
            for(unsigned int frames = MMU::allocable(); frames; frames = MMU::allocable())
                System::_heap->free(MMU::alloc(frames), frames * sizeof(MMU::Page));
        db<Init>(INF) << "done!" << endl;
    }
};

// Global object "init_application"  must be linked to the application (not
// to the system) and there constructed at first.
Init_Application init_application;

__END_SYS
