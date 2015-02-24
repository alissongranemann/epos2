.globl MMUTable
.global _boot
.global __stack
.global __irq_stack
.global __supervisor_stack
.global __abort_stack
.global __fiq_stack
.global __undef_stack

.set TblBase, MMUTable

// Workaround for simulation not working when L1 D and I caches,MMU and  L2 cache enabled - DT568997
.set SIM_MODE, 1
.if SIM_MODE == 1
.set CRValMmuCac, 0b00000000000000 // Disable IDC and MMU
.else
.set CRValMmuCac, 0b01000000000101 // Enable IDC and MMU
.endif

// Stack Pointer locations for boot code
.set Undef_stack, __undef_stack
.set FIQ_stack, __fiq_stack
.set Abort_stack, __abort_stack
.set SPV_stack, __supervisor_stack
.set IRQ_stack, __irq_stack
.set SYS_stack, __stack

.section .boot,"ax"
_boot:
    // Only allow cpu 0 through
    mrc p15,0,r1,c0,c0,5
    and r1, r1, #0xf
    cmp r1, #0
    beq OKToRun
EndlessLoop0:
    wfe
    b EndlessLoop0

OKToRun:
    // Set scu enable bit in scu
    ldr r7, =0xf8f00000
    ldr r0, [r7]
    orr r0, r0, #0x1
    str r0, [r7]

    // Invalidate scu
    ldr r7, =0xf8f0000c
    ldr r6, =0xffff
    str r6, [r7]

    // Write to ACTLR
    mrc p15, 0, r0, c1, c0, 1       // Read ACTLR
    orr r0, r0, #(0x01 << 6)        // Set SMP bit
    orr r0, r0, #(0x01 )
    mcr p15, 0, r0, c1, c0, 1       // Write ACTLR

    // Invalidate caches and TLBs
    // Reference: http://infocenter.arm.com/help/topic/com.arm.doc.ddi0151c/ARM920T_TRM1_S.pdf
    mov r0, #0                   // r0 = 0
    mcr p15, 0, r0, c8, c7, 0   // Invalidate TLBs
    mcr p15, 0, r0, c8, c7, 0   // Invalidate TLBs
    mcr p15, 0, r0, c7, c5, 0   // Invalidate icache
    mcr p15, 0, r0, c7, c5, 6   // Invalidate branch predictor array, not in the manual
    bl  invalidate_dcache       // Invalidate dcache

    // Disable MMU, if enabled
    mrc p15, 0, r0, c1, c0, 0   // Read CP15 register 1
    bic r0, r0, #0x1            // Clear bit 0
    mcr p15, 0, r0, c1, c0, 0   // Write value back

    // Caution: Never use an MSR instruction to force a change to the state of
    // the T bit in the CPSR. If you do this, the processor enters an
    // unpredictable state.

    // Mode bits from CPSR are explained in
    // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0210c/DDI0210B.pdf
    // That manual is from ARM7, but the same applies to ARM9, apparently.

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the irq stack pointer
    and r2, r1, r0
    orr r2, r2, #0x12           // IRQ mode
    msr cpsr, r2
    ldr r13,=IRQ_stack          // IRQ stack pointer

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the Abort stack pointer
    and r2, r1, r0
    orr r2, r2, #0x17           // Abort mode
    msr cpsr, r2
    ldr r13,=Abort_stack        // Abort stack pointer

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the FIQ stack pointer
    and r2, r1, r0
    orr r2, r2, #0x11           // FIQ mode
    msr cpsr, r2
    ldr r13,=FIQ_stack          // FIQ stack pointer

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the Undefine stack pointer
    and r2, r1, r0
    orr r2, r2, #0x1b           // Undefine mode
    msr cpsr, r2
    ldr r13,=Undef_stack        // Undefine stack pointer

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the system stack pointer
    and r2, r1, r0
    orr r2, r2, #0x1f           // SYS mode
    msr cpsr, r2
    ldr r13,=SYS_stack          // SYS stack pointer

    mrs r0, cpsr                // Get the current PSR
    mvn r1, #0x1f               // Set up the supervisor stack pointer
    and r2, r1, r0
    orr r2, r2, #0x13           // Supervisor mode
    msr cpsr, r2
    ldr r13,=SPV_stack          // Supervisor stack pointer

    // Enable MMU and cache
    ldr r0,=TblBase             // Load MMU translation table base
    orr r0, r0, #0x5B           // Outer-cacheable, WB
    mcr p15, 0, r0, c2, c0, 0   // TTB0

    mvn r0,#0                   // Load MMU domains -- all ones=manager
    mcr p15,0,r0,c3,c0,0

    // Enable MMU, icache and dcache
    mrc p15,0,r0,c1,c0,0        // SCTLR register
    ldr r1,=CRValMmuCac
    orr r0, r0, r1
    mcr p15,0,r0,c1,c0,0        // Enable caches and MMU

    // No instruction in program order after this instruction executes until
    // this instruction completes. This instruction completes when: All explicit
    // memory accesses before this instruction complete. All Cache, Branch
    // predictor and TLB maintenance operations before this instruction
    // complete.
    and r0, r0, r0
    and r0, r0, r0

    // dsb  allow the MMU to start up
    dsb

    and r0, r0, r0
    and r0, r0, r0

    isb                         // Isb flush prefetch buffer

    mov r0, r0
    mrc p15, 0, r1, c1, c0, 2   // Read cp access control register (CACR) into r1
    orr r1, r1, #(0xf << 20)    // Enable full access for p10 & p11
    mcr p15, 0, r1, c1, c0, 2   // Write back into CACR

    mrc p15,0,r0,c1,c0,0        // Flow prediction enable
    orr r0, r0, #(0x01 << 11)   // #0x8000
    mcr p15,0,r0,c1,c0,0

    mrc p15,0,r0,c1,c0,1        // Read Auxiliary Control Register
    orr r0, r0, #(0x1 << 2)     // Enable Dside prefetch
    orr r0, r0, #(0x1 << 1)     // Enable L2 Prefetch hint
    mcr p15,0,r0,c1,c0,1        // Write Auxiliary Control Register

    b boot_return

// invalidate_dcache - invalidate the entire d-cache by set/way
// Note: for Cortex-A9, there is no cp instruction for invalidating the whole
// D-cache. Need to invalidate each line.
// Need to re-check this code against
// DDI0406B_arm_architecture_reference_manual_errata_markup_10_0 (ARM ARM).pdf, page 1251.
invalidate_dcache:
    mrc p15, 1, r0, c0, c0, 1   // Read CLIDR

    mov r3, r3, lsr #23         // Cache level value (naturally aligned)
    beq finished
    mov r10, #0                 // Start with level 0
loop1:
    add r2, r10, r10, lsr #1    // Work out 3xcachelevel
    mov r1, r0, lsr r2          // Bottom 3 bits are the Cache type for this level
    and r1, r1, #7              // Get those 3 bits alone
    cmp r1, #2
    blt skip                    // No cache or only instruction cache at this level
    mcr p15, 2, r10, c0, c0, 0  // Write the Cache Size selection register
    isb                         // Isb to sync the change to the CacheSizeID reg
    mrc p15, 1, r1, c0, c0, 0   // Reads current Cache Size ID register

    and r2, r1, #7              // Extract the line length field
    add r2, r2, #4              // Add 4 for the line length offset (log2 16 bytes)
    ldr r4, =0x3ff

    ands r4, r4, r1, lsr #3     // R4 is the max number on the way size (right aligned)
    clz r5, r4                  // R5 is the bit position of the way size increment
    ldr r7, =0x7fff
    ands r7, r7, r1, lsr #13    // R7 is the max number of the index size (right aligned)
loop2:
    mov r9, r4                  // R9 working copy of the max way size (right aligned)
loop3:
    orr r11, r10, r9, lsl r5    // Factor in the way number and cache number into r11
    orr r11, r11, r7, lsl r2    // Factor in the index number

    mcr p15, 0, r11, c7, c14, 2 // Clean & invalidate by set/way
    subs r9, r9, #1             // Decrement the way number
    bge loop3
    subs r7, r7, #1             // Decrement the index
    bge loop2
skip:
    add r10, r10, #2            // Increment the cache number
    cmp r3, r10
    bgt loop1

finished:
    mov r10, #0                 // Swith back to cache level 0
    mcr p15, 2, r10, c0, c0, 0  // Select current cache level in cssr
    dsb
    isb

    bx  lr
