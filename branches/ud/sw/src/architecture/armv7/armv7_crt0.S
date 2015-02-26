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

.section .init
.align 2
.global _start
.type _start, function
_start:
    // TODO: Oddly, Zynq is booting in system mode instead of supervisor mode as
    // stated in its Technical Reference Manual. Here, we're forcing it to
    // supervisor mode. This issue must be further investigated.
    mrs r0, cpsr
    mvn r1, #0x1f
    and r2, r1, r0
    orr r2, r2, #0x13
    msr cpsr, r2

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
