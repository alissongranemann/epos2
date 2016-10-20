#ifndef __big_kernel_lock_h
#define __big_kernel_lock_h


#include <system/config.h>
#include <utility/spin.h>

__BEGIN_SYS

/* Inside-kernel Big Kernel Lock (BKL) */
class Big_Kernel_Lock
{

    static const bool smp = Traits<Thread>::smp;

public:
    static void lock()
    {
        CPU::int_disable();
        if(smp)
            _lock.acquire();
    }

    static void unlock()
    {
        if(smp)
            _lock.release();
        CPU::int_enable();
    }

private:
    static Spin _lock;
};


__END_SYS

#endif
