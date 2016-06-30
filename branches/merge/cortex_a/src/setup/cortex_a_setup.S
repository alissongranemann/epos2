// EPOS Cortex-A SETUP

        .file "cortex_a_setup.S"

        // Interrupt Vector Table
        .section .init
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

_start_addr:
        .word _start
_undefined_instruction_addr:
        .word _undefined_instruction
_swi_addr:
        .word _software_interrupt
_prefetch_abort_addr:
        .word _prefetch_abort
_data_abort_addr:
        .word _data_abort
_reserved_addr:
        .word _reserved
_irq_addr:
        .word _int_entry
_fiq_addr:
        .word _fiq
