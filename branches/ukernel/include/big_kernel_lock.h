#ifndef __big_kernel_lock_h
#define __big_kernel_lock_h


#include <system/config.h>

__BEGIN_SYS

class Big_Kernel_Lock
{

    static const bool smp = Traits<Thread>::smp;

public:
    Big_Kernel_Lock() {}

public:
    static void lock()
    {
        if(smp) {
            int me = *_running_thread;

            while (CPU::cas(_owner, 0, me) != me);

            _level++;
        }
    }


    static void unlock()
    {
        if(smp) {
            if(--_level <= 0) {
                _owner = 0;
            }
        }
    }

private:
    static volatile unsigned int _level;
    static volatile int _owner;

private:
    static int * _running_thread;
};


__END_SYS

#endif
