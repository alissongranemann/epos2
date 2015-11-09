	.file "armv7_crt0.S"

        .section .init
        // Interrupt Vector Table
        .type   _vector_table, object
_vector_table:
        .word   0x20004000              // Stack pointer at reset
        .word   _start + 1              // Reset  (lsb must be 1 in thumb mode)
        .word   _int_dispatch + 1       // NMI
        .word   _int_dispatch + 1       // Hard fault
        .word   _int_dispatch + 1       // Memory management fault
        .word   _int_dispatch + 1       // Bus fault
        .word   _int_dispatch + 1       // Usage fault
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // SVCall
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // Reserved
        .word   _int_dispatch + 1       // PendSV
        .word   _int_dispatch + 1       // Systick
        .word   _int_dispatch + 1       // IRQ0
        .word   _int_dispatch + 1       // IRQ1
        .word   _int_dispatch + 1       // IRQ2
        .word   _int_dispatch + 1       // IRQ3
        .word   _int_dispatch + 1       // IRQ4
        .word   _int_dispatch + 1       // IRQ5
        .word   _int_dispatch + 1       // IRQ6
        .word   _int_dispatch + 1       // IRQ7
        .word   _int_dispatch + 1       // IRQ8
        .word   _int_dispatch + 1       // IRQ9
        .word   _int_dispatch + 1       // IRQ10
        .word   _int_dispatch + 1       // IRQ11
        .word   _int_dispatch + 1       // IRQ12
        .word   _int_dispatch + 1       // IRQ13
        .word   _int_dispatch + 1       // IRQ14
        .word   _int_dispatch + 1       // IRQ15
        .word   _int_dispatch + 1       // IRQ16
        .word   _int_dispatch + 1       // IRQ17
        .word   _int_dispatch + 1       // IRQ18
        .word   _int_dispatch + 1       // IRQ19
        .word   _int_dispatch + 1       // IRQ20
        .word   _int_dispatch + 1       // IRQ21
        .word   _int_dispatch + 1       // IRQ22
        .word   _int_dispatch + 1       // IRQ23
        .word   _int_dispatch + 1       // IRQ24
        .word   _int_dispatch + 1       // IRQ25
        .word   _int_dispatch + 1       // IRQ26
        .word   _int_dispatch + 1       // IRQ27
        .word   _int_dispatch + 1       // IRQ28
        .word   _int_dispatch + 1       // IRQ29
        .word   _int_dispatch + 1       // IRQ30
        .word   _int_dispatch + 1       // IRQ31
        .word   _int_dispatch + 1       // IRQ32
        .word   _int_dispatch + 1       // IRQ33
        .word   _int_dispatch + 1       // IRQ34
        .word   _int_dispatch + 1       // IRQ35
        .word   _int_dispatch + 1       // IRQ36
        .word   _int_dispatch + 1       // IRQ37
        .word   _int_dispatch + 1       // IRQ38
        .word   _int_dispatch + 1       // IRQ39
        .word   _int_dispatch + 1       // IRQ40
        .word   _int_dispatch + 1       // IRQ41
        .word   _int_dispatch + 1       // IRQ42
        .word   _int_dispatch + 1       // IRQ43
        .word   _int_dispatch + 1       // IRQ44
        .word   _int_dispatch + 1       // IRQ45
	.section .init
	.align  2
	.global _start
	.type   _start, function
_start:
        // Clear BSS
        mov     r0, #0
# Loading 0x20000000 into r1
# FIXME: This value should not be hardcoded, but __bss_start__ is pointing to the wrong address!
        mov     r1, #32
        lsl     r1, r1, #24
#        ldr     r1, =__bss_start__
        ldr     r2, =__bss_end__
  .L1:  str     r0, [r1]
        add     r1, #4
        cmp     r1, r2
        blt     .L1

	bl      _init

        .align  2
        .globl  __epos_app_entry
        .type   __epos_app_entry, function
__epos_app_entry:
        bl      main
        push    {r0}         // save main's return value to be used by exit()
    	bl	_fini
    	bl	_exit
