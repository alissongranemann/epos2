// EPOS System Binding

#include <machine.h>
#include <display.h>
#include <thread.h>

__USING_SYS;
extern "C"
{
    void _panic()
    {
        Machine::panic();
    }

    void _cpu_int_enable()
    {
        CPU::int_enable();
    }

    void _cpu_int_disable()
    {
        CPU::int_disable();
    }

    unsigned int _cpu_id()
    {
        return Machine::cpu_id();
    }

    void _smp_barrier()
    {
        Machine::smp_barrier();
    }

    unsigned int _this_thread_id()
    {
        return This_Thread::id();
    }

    void _big_kernel_lock_lock() {}
    void _big_kernel_lock_unlock() {}

    void _exit(int s)
    {
        Thread::exit(s);
    }

    void __exit()
    {
        Thread::exit(CPU::fr());
    }  // must be handled by the Page Fault handler for user-level tasks

    void _print(const char * s)
    {
        Display::puts(s);
        if (! Traits<Serial_Display>::enabled) {
            Serial_Display::puts(s);
        }
    }

    // LIBC Heritage
    void __cxa_pure_virtual()
    {
        db<void>(ERR) << "Pure Virtual method called!" << endl;
    }
}
