// EPOS Application Initializer

#include <utility/heap.h>
#include <mmu.h>
#include <machine.h>
#include <application.h>
#include <address_space.h>
#include <segment.h>


extern "C" { unsigned int _cpu_id(); }
extern "C" { void _smp_barrier(); }
extern "C" { void * _create_segment_in_place(void * place, unsigned int size, unsigned int mmu_flags); }
extern "C" { void * _create_heap_in_place(void * place, void * heap_segment); }

__BEGIN_SYS

class Init_Application
{
private:
    static const unsigned int HEAP_SIZE = Traits<Application>::HEAP_SIZE;
    static const unsigned int STACK_SIZE = Traits<Application>::STACK_SIZE;

public:
    Init_Application()
    {
        db<Init>(TRC) << "Init_Application()" << endl;

        /// _smp_barrier();
        /* As far I could notice, only the main thread will
        * enter here.
        * APs' entry point is Thread::idle so APs will not enter here when the
        * system is starting.
        *
        * A AP can enter here if a new thread is created an assigned to it (AP)
        * and the thread entry point is application _start.
        * In that case, usually the new thread belongs to another task
        * (it is the Thread Main of the task) and it should run Init_Application
        * so the task can create its own heap.
        *
        * In any case, the smp_barrier seems unnecessary.
        * Therefore, commenting it. */

        // Initialize Application's heap
        db<Init>(INF) << "Initializing application's heap: " << endl;
        if(Traits<System>::multiheap) {

            db<Init>(TRC) << "_heap_segment: " << Application::_heap_segment
                            << ", place to create segment: " << reinterpret_cast<void *>(&Application::_preheap[0])
                            << ", place to create heap: " << reinterpret_cast<void *>(&Application::_preheap[sizeof(Segment)])
                            << ", mmu flags: " << reinterpret_cast<void *>(Segment::Flags::APP)
                            << endl;

            Application::_heap_segment = reinterpret_cast<Segment *>(_create_segment_in_place(&Application::_preheap[0], HEAP_SIZE, Segment::Flags::APP));

            db<Init>(TRC) << "_heap_segment: " << Application::_heap_segment << endl;

            Application::_heap = reinterpret_cast<Heap *>(_create_heap_in_place(&Application::_preheap[sizeof(Segment)], Application::_heap_segment));

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
