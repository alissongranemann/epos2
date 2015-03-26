.file "armv7_crt0.S"

// Currently, there are two exception models in the ARM architecture. In the
// classic model, used in pre-Cortex chips and current Cortex-A/R chips, the
// memory at 0 contains several exception handlers. In the new model used in the
// Cortex-M chips, the vector table at 0 is actually a table of pointers, not
// instructions. Thus, Cortex-A9 and Cortex-M3 vector tables are not compatible.

.section .init
// Interrupt Vector Table
.type _vector_table, object
_vector_table:
    ldr pc, _start_addr
    ldr pc, _undefined_instruction_addr
    ldr pc, _swi_addr
    ldr pc, _prefetch_abort_addr
    ldr pc, _data_abort_addr
    ldr pc, _reserved_addr
    ldr pc, _irq_addr
    ldr pc, _fiq_addr

_start_addr:                .word _start
_undefined_instruction_addr:.word _undefined_instruction
_swi_addr:                  .word _software_interrupt
_prefetch_abort_addr:       .word _prefetch_abort
_data_abort_addr:           .word _data_abort
_reserved_addr:             .word _reserved
_irq_addr:                  .word _irq
_fiq_addr:                  .word _fiq

.section .init
.align 2
.global _start
.type _start, function
_start:
    // Clear BSS
    mov r0, #0
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
.L1:
    str r0, [r1]
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
