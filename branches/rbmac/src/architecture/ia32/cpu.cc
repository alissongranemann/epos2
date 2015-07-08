// EPOS IA32 CPU Mediator Implementation

#include <architecture/ia32/cpu.h>
#include <thread.h>

extern "C" { void _exec(void *); }

__BEGIN_SYS

// Class attributes
unsigned int IA32::_cpu_clock;
unsigned int IA32::_bus_clock;

// Class methods
void IA32::Context::save() volatile
{
    // Save the running thread context into its own stack (mostly for debugging)
    ASM("     push    %ebp                                            \n"
        "     mov     %esp, %ebp                                      \n"
        "     mov     8(%ebp), %esp   # sp = this                     \n"
        "     add     $40, %esp       # sp += sizeof(Context)         \n"
        "     push    4(%ebp)         # push eip                      \n"
        "     pushf                                                   \n"
        "     push    %eax                                            \n"
        "     push    %ecx                                            \n"
        "     push    %edx                                            \n"
        "     push    %ebx                                            \n"
        "     push    %ebp            # push esp                      \n"
        "     push    (%ebp)          # push ebp                      \n"
        "     push    %esi                                            \n"
        "     push    %edi                                            \n"
        "     mov     %ebp, %esp                                      \n"
        "     pop     %ebp                                            \n");
}

void IA32::Context::load() const volatile
{
    if(Traits<System>::multitask) {
        // Discard the context pushed into the stack during thread creation, but preserve the entry point (IP)
        // Adjust the user-level stack pointer in the dummy TSS (that will be used by system calls and interrupts
        ASM("       mov     4(%esp), %esp         # sp = this           \n");
        ASM("       pop     %0                                          \n" : "=m"(reinterpret_cast<TSS *>(Memory_Map<PC>::TSS0)->esp) : );
        ASM("       popa                                                \n"
            "       popf                                                \n");
        ASM("       mov     %%esp, %0                                   \n" : "=m"(Thread::running()->_context) : );

        // Reload Segment Registers with user-level selectors
        ASM("       mov     %0, %%ds                                    \n"
            "       mov     %0, %%es                                    \n"
            "       mov     %0, %%fs                                    \n"
            "       mov     %0, %%gs                                    \n"
            : : "r"(SEL_APP_DATA));

        // Restore the thread's entry point left on the stack while initializing its context
        ASM("       pop     %esi                # si = this->_eip       \n");

        // Fake a cross-level stack for a return from kernel to user-level: ss, esp, eflags, cs, eip
        ASM("       push    %0                                          \n"
            "       push    %2                                          \n"
            "       pushf                                               \n"
            "       push    %1                                          \n"
            "       push    %%esi                                       \n"
            "       iret                                                \n"
            : : "r"(SEL_APP_DATA), "r"(SEL_APP_CODE), "r"(System::info()->lm.app_heap));
    } else {
        // Pop the context pushed into the stack during thread creation to initialize the CPU's context, discarding user-level stack pointer
        // Obs: POPA ignores the ESP saved by PUSHA. ESP is just normally incremented
        ASM("       mov     4(%esp), %esp       # sp = this             \n"
            "       pop     %eax                                        \n"
            "       popa                                                \n"
            "       popf                                                \n");
    }
}

void IA32::switch_context(Context * volatile * o, Context * volatile n)
{
    // Save the previously running thread context ("o") into its stack (including the user-level stack pointer stored in the dummy TSS)
    // and updates the its _context attribute
    // PUSHA saves an extra "esp" (which is always "this"), but saves several instruction fetches
    ASM("	pushf                  		                \n"
        "	pusha				                \n");
    ASM("       push    %0                                      \n" : : "m"(reinterpret_cast<TSS *>(Memory_Map<PC>::TSS0)->esp));
    ASM("       mov     44(%esp), %eax          # old           \n"
        "       mov     %esp, (%eax)                            \n");

    // Restore the next thread context ("n") from its stack (and the user-level stack pointer, updating the dummy TSS)
    ASM("	mov     48(%esp), %esp          # new	        \n");
    ASM("       pop     %0                                      \n" : "=m"(reinterpret_cast<TSS *>(Memory_Map<PC>::TSS0)->esp) : );
    ASM("	popa				                \n"
        "	popf			                	\n");
    ASM("       mov     %%esp, %0                               \n" : "=m"(Thread::running()->_context) : );
}

void IA32::syscalled()
{
    // We get here when an APP triggers INT_SYSCALL with the message address in AX
    // The CPU saves the user-level stack pointer in the dummy TSS and restores the kernel-level system stack pointer also from it

    if(Traits<System>::multitask) {
        // Changes the stack to the running thread's kernel one, preserving the message and the kernel-level system stack pointer for posterior "iret"
        ASM("       mov     %eax, %ecx                              \n"
            "       mov     %esp, %edx                              \n");
        ASM("       mov     %0, %%esp                               \n" : : "m"(Thread::running()->_context));
        ASM("       push    %edx                                    \n");

        // Do the system call by calling _exec with the message pointed by AX (copied to CX)
        ASM("       push    %ecx                                    \n"
            "       call    _exec                                   \n"
            "       pop     %eax                                    \n"
            "       pop     %edx                                    \n");
        ASM("       mov     %%esp, %0                               \n" : "=m"(Thread::running()->_context) : );
        ASM("       mov     %edx, %esp                              \n"
            "       iret                                            \n");
    }
}

__END_SYS
