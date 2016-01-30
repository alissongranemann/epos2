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

    void _big_kernel_lock_lock()
    {
        _SYS::Big_Kernel_Lock::lock();
    }


    void _big_kernel_lock_unlock()
    {
        _SYS::Big_Kernel_Lock::unlock();
    }

}

__USING_SYS;
extern "C" {
    void _syscall(void * m) { CPU::syscall(m); }
    void _print(const char * s) {
        Message msg(Id(UTILITY_ID, 0), Message::PRINT, reinterpret_cast<unsigned int>(s));
        msg.act();
    }
}
