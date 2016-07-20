// EPOS IA32 CPU System Call Function Implementation

#include <architecture/ia32/cpu.h>
#include <ic.h>
#include <big_kernel_lock.h>

extern "C" { void _big_kernel_lock_lock(); }
extern "C" { void _big_kernel_lock_unlock(); }

__BEGIN_SYS

void IA32::syscall(void * msg)
{
    ASM("int %0" : : "i"(IC::INT_SYSCALL), "c"(msg));
}

__END_SYS
