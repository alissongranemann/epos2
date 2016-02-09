// EPOS Application Binding

#include <utility/ostream.h>
#include <application.h>
#include <cpu.h>
#include <framework/main.h>
#include <big_kernel_lock.h>

// Framework class attributes
__BEGIN_SYS
Framework::Cache Framework::_cache;
__END_SYS


// Global objects
__BEGIN_SYS
OStream kerr;
__END_SYS


// Bindings
extern "C" {
    void _panic() { _API::Thread::exit(-1); }
    void _exit(int s) { _API::Thread::exit(s); }


    void _cpu_int_enable()
    {
        _API::CPU::int_enable();
    }

    void _cpu_int_disable()
    {
        _API::CPU::int_disable();
    }

    unsigned int _cpu_id()
    {
        return _API::Machine::cpu_id();
    }

    void _smp_barrier()
    {
        _API::Machine::smp_barrier();
    }

    unsigned int _this_thread_id()
    {
        return _API::This_Thread::this_thread_id();
    }

}

__USING_SYS;
extern "C" 
{
    void _syscall(void * m)
    { 
        CPU::syscall(m);
    }

    void _print(const char * s)
    {
        Message msg(Id(UTILITY_ID, 0), Message::PRINT, reinterpret_cast<unsigned int>(s));
        msg.act();
    }

    void * _create_segment_in_place(void * place, unsigned int size, unsigned int mmu_flags)
    {
        Message msg(Id(SEGMENT_ID, 0), Message::CREATE_SEGMENT_IN_PLACE, place, size, mmu_flags);
        msg.act();
        return reinterpret_cast<void *>(msg.id().unit());
    }

    void * _create_heap_in_place(void * place, void * heap_segment)
    {
        Message msg(Id(SEGMENT_ID, 0), Message::CREATE_HEAP_IN_PLACE, place, heap_segment);
        msg.act();
        return reinterpret_cast<void *>(msg.id().unit());
    }

}
