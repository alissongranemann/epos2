#include "boot.S"
.file "armv7_crt0.S"

.section .init
.align 2

_vector_table:
    ldr pc, _start_addr
    ldr pc, _undefined_instruction_addr
    ldr pc, _software_interrupt_addr
    ldr pc, _prefetch_abort_addr
    ldr pc, _data_abort_addr
    ldr pc, _reserved_addr
    ldr pc, _irq_handler_addr
    ldr pc, _fiq_handler_addr
    
    // These vectors and code are used for rom patching
    .org 0x20
    .code 16
    _RPTV_0_START:
    bx lr
    
    .org 0x60
    _RPTV_1_START:
    bx lr
    
    .org 0xa0
    _RPTV_2_START:
    bx lr
    
    .org 0xe0
    _RPTV_3_START:
    bx lr
    
    .org 0x120
    ROM_var_start: .word 0
    .org 0x7ff
    ROM_var_end:   .word 0
    .code 32

_start_addr:                .word _init
_undefined_instruction_addr:.word _undefined_instruction
_software_interrupt_addr:   .word _prefetch_abort
_prefetch_abort_addr:       .word _undefined_instruction
_data_abort_addr:           .word _data_abort
_reserved_addr:             .word _reserved
_irq_handler_addr:          .word _irq_handler
_fiq_handler_addr:          .word _fiq_handler

// From memory_map.h (TOP)
_stack_addr: .word 0x1ffffffc 

.equ ARM_MODE_USR,      0x10
.equ ARM_MODE_FIQ,      0x11
.equ ARM_MODE_IRQ,      0x12
.equ ARM_MODE_SVC,      0x13

.equ IRQ_BIT,           0x80;
.equ FIQ_BIT,           0x40;

// EPOS fits in less than 64KB without debug flags. The values below are fairly
// arbitrary
// 0x00100000 = 1mb
.set __supervisor_stack_cpu1,   0x00080000 // 512kb minus the EPOS's code size (around 20-30kb)
.set __supervisor_stack,        0x00100000 // 512kb (way bigger than necessary, I know
.set __stack,                   0x00100040 // 64 bytes, sys stack
.set __irq_stack,               0x00100080 // 64 bytes
.set __abort_stack,             0x001000c0 // 64 bytes
.set __fiq_stack,               0x00100100 // 64 bytes
.set __undef_stack,             0x00100140 // 64 bytes
.set __user_stack,              0x1ffffffc 

// MMU uses a two-level memory mapped table. The first level has 4k entries
// (with 4 bytes each), which points to a second level (l2) table, which has 256
// word-sized entries.  In total, it is needed 4kb*256*4 + 4kb*4 = 4mb + 16kb to
// map the whole address space.

// Must be aligned in a 16kb boundary
.set MMUTable,          0x00104000

.section .init
.align 2
.global _start
.type _start, function
_start:
    msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT
    ldr sp, =__supervisor_stack
    //ldr sp, _stack_addr

    ldr r0, =MMUTable
    // Outer shareable flags. Assuming multiprocessing.
    add r0, r0, #0x4a
    // Write r0 in TTBR0 (register that holds the MMU's page table location).
    // TTBCR reset value is 0, no need to change it if we are going to use only
    // TTBR0.  Must map system's VA to higher addresses otherwise.
    MCR p15,0,r0,c2,c0,0
    
    // Do not remove this jump unless you set each stack somewhere else
    b _boot

    .global boot_return
    boot_return:

    // Clear bss
    mov r0, #0
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
  
    bss_clean_loop:
        cmp r1, r2
        strlo r0, [r1], #4
        blo bss_clean_loop

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
