.file "armv7_crt0.S"

.section .init
.align 2
.global _start
.type _start, function
_start:
    // Clear BSS
    mov r0, #0
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
.L1: str r0, [r1]
    add r1, #4
    cmp r1, r2
    blt .L1

    bl _init

.align 2
.global __epos_app_entry
.type   __epos_app_entry, function
__epos_app_entry:
    // mov r0, #1
    // msr CONTROL, r0
    b  main // Do not link! LR was loaded by Context::load()
    // push {r0} // Save main's return value to be used by exit()
    bl  _fini
    bl  _exit
