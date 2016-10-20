// EPOS PC Interrupt Controller Mediator Declarations

#ifndef __pc_ic_h
#define __pc_ic_h

#include <cpu.h>
#include <ic.h>
#include <machine/pc/memory_map.h>

__BEGIN_SYS

// Intel 8259A Interrupt Controller (master and slave are seen as a unit)
class i8259A
{
    friend class APIC;

private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;

    static const unsigned int IRQS = 16;
    static const unsigned int HARD_INT = 32;
    static const unsigned int SOFT_INT = HARD_INT + IRQS;

public:
    // I/O Ports
    enum {
        MASTER      = 0x20,
        MASTER_CMD  = MASTER,
        MASTER_DAT  = MASTER + 1,
        SLAVE       = 0xa0,
        SLAVE_CMD   = SLAVE,
        SLAVE_DAT   = SLAVE + 1
    };

    // Commands
    enum {
        SELECT_IRR  = 0x0a,
        SELECT_ISR  = 0x0b,
        ICW1        = 0x11, // flank, cascaded, more ICWs
        ICW4        = 0x01,
        EOI         = 0x20
    };

    // IRQs
    typedef unsigned int IRQ;
    enum {
        IRQ_TIMER       = 0,
        IRQ_KEYBOARD    = 1,
        IRQ_CASCADE     = 2,
        IRQ_SERIAL24    = 3,
        IRQ_SERIAL13    = 4,
        IRQ_PARALLEL23  = 5,
        IRQ_FLOPPY      = 6,
        IRQ_PARALLEL1   = 7,
        IRQ_RTC         = 8,
        IRQ_MOUSE       = 12,
        IRQ_MATH        = 13,
        IRQ_DISK1       = 14,
        IRQ_DISK2       = 15,
        IRQ_LAST        = IRQ_DISK2
    };

    // Interrupts
    static const unsigned int INTS = 50;
    enum {
        INT_FIRST_HARD  = HARD_INT,
        INT_TIMER       = HARD_INT + IRQ_TIMER,
        INT_KEYBOARD    = HARD_INT + IRQ_KEYBOARD,
        INT_LAST_HARD   = HARD_INT + IRQ_LAST,
        INT_RESCHEDULER = SOFT_INT,
        INT_SYSCALL
    };

public:
    i8259A() {}

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }

    static void enable() { imr(1 << IRQ_CASCADE); }
    static void enable(int i) { imr(imr() & ~(1 << int2irq(i))); }
    static void disable() { imr(~(1 << IRQ_CASCADE)); }
    static void disable(int i) { imr(imr() | (1 << int2irq(i))); }

    static void remap(Reg8 base = HARD_INT) {
        Reg8 m_imr = CPU::in8(MASTER_DAT);      // save IMRs
        Reg8 s_imr = CPU::in8(SLAVE_DAT);

        // Configure Master PIC
        CPU::out8(MASTER_CMD, ICW1);
        CPU::out8(MASTER_DAT, base);            // ICW2 is the base
        CPU::out8(MASTER_DAT, 1 << IRQ_CASCADE);// ICW3 = IRQ2 cascaded
        CPU::out8(MASTER_DAT, ICW4);

        // Configure Slave PIC
        CPU::out8(SLAVE_CMD, ICW1);
        CPU::out8(SLAVE_DAT, base + 8);         // ICW2 is the base
        CPU::out8(SLAVE_DAT, 0x02);             // ICW3 = cascaded from IRQ1
        CPU::out8(SLAVE_DAT, ICW4);

        CPU::out8(MASTER_DAT, m_imr);           // restore saved IMRs
        CPU::out8(SLAVE_DAT, s_imr);
    }

    static void reset() { remap(); disable(); }

    static Reg16 irr() { // Pending interrupts
        CPU::out8(MASTER_CMD, SELECT_IRR);
        CPU::out8(SLAVE_CMD, SELECT_IRR);
        return CPU::in8(MASTER_CMD) | (CPU::in8(SLAVE_CMD) << 8);
    }

    static Reg16 isr() { // In-service interrupts
        CPU::out8(MASTER_CMD, SELECT_ISR);
        CPU::out8(SLAVE_CMD, SELECT_ISR);
        return CPU::in8(MASTER_CMD) | (CPU::in8(SLAVE_CMD) << 8);
    }

    static Reg16 imr() { // Interrupt mask
        return CPU::in8(MASTER_DAT) | (CPU::in8(SLAVE_DAT) << 8);
    }

    static void imr(Reg16 mask) {
        CPU::out8(MASTER_DAT, mask & 0xff);
        CPU::out8(SLAVE_DAT, mask >> 8);
    }

    static bool eoi(unsigned int i) {
        int irq = int2irq(i);

        if(!(isr() & (1 << irq))) {           // spurious interrupt?
            if(isr() & (1 << IRQ_CASCADE))    // cascade?
                CPU::out8(MASTER_CMD, EOI);   // send EOI to master
            return false;
        }

        if(irq >= 8)                    // slave interrupt?
            CPU::out8(SLAVE_CMD, EOI);  // send EOI to slave
        CPU::out8(MASTER_CMD, EOI);     // always send EOI to master

        return true;
    }

    static void ipi_send(int dest, int interrupt) {}
};

// Intel IA-32 APIC (internal, not tested with 82489DX)
class APIC
{
private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Reg64 Reg64;
    typedef CPU::Log_Addr Log_Addr;

    static const unsigned int HARD_INT = i8259A::HARD_INT;

public:
    // Interrupts
    static const unsigned int INTS = i8259A::INTS;
    enum {
        INT_FIRST_HARD  = i8259A::INT_FIRST_HARD,
        INT_TIMER       = i8259A::INT_TIMER,
        INT_KEYBOARD    = i8259A::INT_KEYBOARD,
        INT_RESCHEDULER = i8259A::INT_RESCHEDULER, // in multicores, reschedule goes via IPI, which must be acknowledged just like hardware
        INT_SYSCALL     = i8259A::INT_SYSCALL,
        INT_LAST_HARD   = INT_RESCHEDULER
    };

    // Default mapping addresses
    enum {
        LOCAL_APIC_PHY_ADDR = 0xfee00000,
        LOCAL_APIC_LOG_ADDR = Memory_Map::APIC,
        LOCAL_APIC_SIZE     = Memory_Map::IO_APIC - Memory_Map::APIC,
        IO_APIC_PHY_ADDR    = 0xfec00000,
        IO_APIC_LOG_ADDR    = Memory_Map::IO_APIC,
        IO_APIC_SIZE        = Memory_Map::VGA - Memory_Map::IO_APIC
    };

    // Memory-mapped registers
    // Values added with _base (originaly 0xfee00000). See Figure 10-1 of Intel Arch Dev Manual Vol 3A, 2014. Pg 354 PDF.
    enum {
        ID            = 0x020,  // Task priority
        VERSION       = 0x030,  // Task priority
        TPR           = 0x080,  // Task priority
        APR           = 0x090,  // Arbitration priority
        PPR           = 0x0a0,  // Processor priority
        EOI           = 0x0b0,  // End of interrupt
        RRR           = 0x0c0,  // Remote read
        LDR           = 0x0d0,  // Logical destination
        DFR           = 0x0e0,  // Destination format
        SVR           = 0x0f0,  // Spurious interrupt vector
        ISR0_31       = 0x100,  // In-service
        ISR32_63      = 0x110,  // In-service
        ISR64_95      = 0x120,  // In-service
        ISR96_127     = 0x130,  // In-service
        ISR128_159    = 0x140,  // In-service
        ISR160_191    = 0x150,  // In-service
        ISR192_223    = 0x160,  // In-service
        ISR224_255    = 0x170,  // In-service
        TMR0_31       = 0x180,  // Trigger mode
        TMR32_63      = 0x190,  // Trigger mode
        TMR64_95      = 0x1a0,  // Trigger mode
        TMR96_127     = 0x1b0,  // Trigger mode
        TMR128_159    = 0x1c0,  // Trigger mode
        TMR160_191    = 0x1d0,  // Trigger mode
        TMR192_223    = 0x1e0,  // Trigger mode
        TMR224_255    = 0x1f0,  // Trigger mode
        IRR0_31       = 0x200,  // Interrupt request
        IRR32_63      = 0x210,  // Interrupt request
        IRR64_95      = 0x220,  // Interrupt request
        IRR96_127     = 0x230,  // Interrupt request
        IRR128_159    = 0x240,  // Interrupt request
        IRR160_191    = 0x250,  // Interrupt request
        IRR192_223    = 0x260,  // Interrupt request
        IRR224_255    = 0x270,  // Interrupt request
        ESR           = 0x280,  // Error status
        LVT_CMCI      = 0x2f0,  // LVT CMCI
        ICR0_31       = 0x300,  // Interrupt command
        ICR32_63      = 0x310,  // Interrupt command
        LVT_TIMER     = 0x320,  // LVT timer
        LVT_THERMAL   = 0x330,  // LVT thermal sensor
        LVT_PERF      = 0x340,  // LVT performance monitor
        LVT_LINT0     = 0x350,  // LVT interrupt line 0
        LVT_LINT1     = 0x360,  // LVT interrupt line 1
        LVT_ERROR     = 0x370,  // LVT error
        TIMER_INITIAL = 0x380,  // Timer's initial count
        TIMER_CURRENT = 0x390,  // Timer's current count
        TIMER_PRESCALE = 0x3e0   // Timer's BUS CLOCK prescaler
    };

    // MSR registers
    enum {
        IA32_APIC_BASE_MSR = 0x1b
    };

    // MSR and masks
    enum {
        BSP_MASK = 0x100
    };

    // Flags
    enum {
        // Local APIC ID Register
        ID_SHIFT                = 24,
        ID_MASK                 = 0xff000000,

        // Spurious Interrupt Vector Register
        SVR_VECTOR              = 0x000000ff,
        SVR_APIC_ENABLED        = (1 << 8),
        SVR_FOCUS_DISABLED      = (1 << 9),

        // Error Status Register
        ESR_SEND_CHECK          = (1 << 0),
        ESR_RECV_CHECK          = (1 << 1),
        ESR_SEND_ACCEPT         = (1 << 2),
        ESR_RECV_ACCEPT         = (1 << 3),
        ESR_SEND_ILLEGAL_VECTOR = (1 << 5),
        ESR_RECV_ILLEGAL_VECTOR = (1 << 6),
        ESR_ILLEGAL_REGISTER    = (1 << 7),

        // Interrupt Command Register (64 bits)
        ICR_DEST                = (3 << 18),
        ICR_SELF                = (1 << 18),
        ICR_ALL                 = (1 << 19),
        ICR_OTHERS              = (3 << 18),
        ICR_TRIGMOD             = (1 << 15),
        ICR_EDGE                = (0 << 15),
        ICR_LEVEL               = (1 << 15),
        ICR_ASSERTED            = (1 << 14),
        ICR_DEASSERT            = (0 << 14),
        ICR_ASSERT              = (1 << 14),
        ICR_STATUS              = (1 << 12),
        ICR_IDLE                = (0 << 12),
        ICR_PENDING             = (1 << 12),
        ICR_DESTMODE            = (1 << 11),
        ICR_PHY                 = (0 << 11),
        ICR_LOG                 = (1 << 11),
        ICR_DELMODE             = (7 <<  8),
        ICR_FIXED               = (0 <<  8),
        ICR_LOWPRI              = (1 <<  8),
        ICR_SMI                 = (2 <<  8),
        ICR_NMI                 = (4 <<  8),
        ICR_INIT                = (5 <<  8),
        ICR_STARTUP             = (6 <<  8),

        // Local Vector Table
        LVT_MASKED              = (1 << 16),
        LVT_EDGE                = (0 << 15),
        LVT_LEVEL               = (1 << 15),
        LVT_FIXED               = (0 << 8),
        LVT_SMI                 = (2 << 8),
        LVT_NMI                 = (4 << 8),
        LVT_EXTINT              = (7 << 8),
        LVT_INIT                = (5 << 8),

        // Local Timer (32 bits)
        TIMER_PERIODIC          = (1 << 17),
        TIMER_ONE_SHOT          = (0 << 17),
        TIMER_MASKED            = LVT_MASKED,
        TIMER_PRESCALE_BY_1     = 0xb,
        TIMER_PRESCALE_BY_2     = 0x0,
        TIMER_PRESCALE_BY_4     = 0x8,
        TIMER_PRESCALE_BY_8     = 0x2,
        TIMER_PRESCALE_BY_16    = 0xa,
        TIMER_PRESCALE_BY_32    = 0x1,
        TIMER_PRESCALE_BY_64    = 0x9,
        TIMER_PRESCALE_BY_128   = 0x3,
    };

public:
    APIC() {}

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }

    static void remap(Log_Addr addr = LOCAL_APIC_LOG_ADDR) {
        _base = addr;
    }

    static void enable() {
        Reg32 v = read(SVR);
        v |= SVR_APIC_ENABLED;
        write(SVR, v);
    }
    static void enable(int i) { enable(); }

    static void disable() {
        Reg32 v  = read(SVR);
        v &= ~SVR_APIC_ENABLED;
        write(SVR, v);
    }
    static void disable(int i) { disable(); }

    static Reg32 read(unsigned int reg) {
        return *static_cast<volatile Reg32 *>(_base + reg);
    }
    static void write(unsigned int reg, Reg32 v) {
        *static_cast<volatile Reg32 *>(_base + reg) = v;
    }

    static int id() {
        return (read(ID) & ID_MASK) >> ID_SHIFT;
    }
    static int version() {
        return read(VERSION);
    }

    static void ipi_send(unsigned int cpu, unsigned int interrupt);
    static void ipi_init(volatile int * status);
    static void ipi_start(Log_Addr entry, volatile int * status);

    static void reset(Log_Addr addr = LOCAL_APIC_LOG_ADDR) {
        // APIC must be on very early in the boot process, so it is
        // subject to memory remappings. We also cannot be sure about
        // global constructors here
        remap(addr);
        if(Traits<System>::multicore) {
            clear();
            enable();
            connect();
        } else
            disable();
    }

    static int eoi(unsigned int i) { // End of interrupt
        write(APIC::EOI, 0);
        return true;
    }

    static void config_timer(Reg32 count, bool interrupt, bool periodic) {
        Reg32 v = INT_TIMER;
        v |= (interrupt) ? 0 : TIMER_MASKED;
        v |= (periodic) ? TIMER_PERIODIC : 0;
        write(TIMER_INITIAL, count / 16);
        write(TIMER_PRESCALE, TIMER_PRESCALE_BY_16);
        reset_timer();
        write(LVT_TIMER, v);
    }

    static void enable_timer() {
        write(LVT_TIMER, read(LVT_TIMER) & ~TIMER_MASKED);
    }
    static void disable_timer() {
        write(LVT_TIMER, read(LVT_TIMER) | TIMER_MASKED);
    }

    static Reg32 read_timer() {
        return read(TIMER_CURRENT);
    }

    static void reset_timer() {
        disable();
        write(TIMER_CURRENT, read(TIMER_INITIAL));
        enable();
    }

private:
    static int maxlvt() {
        Reg32 v = read(VERSION);
        // 82489DXs do not report # of LVT entries
        return (v & 0xf) ? (v >> 16) & 0xff : 2;
    }

    static void clear() {
        int lvts = maxlvt();

        // Masking an LVT entry on a P6 can trigger a local APIC error
        // if the vector is zero. Mask LVTERR first to prevent this
        if(lvts >= 3)
            write(LVT_ERROR, 1 | LVT_MASKED); // any non-zero vector

        // Careful: we have to set masks only first to deassert
        // any level-triggered sources
        write(LVT_TIMER, read(LVT_TIMER) | LVT_MASKED);
        write(LVT_LINT0, read(LVT_LINT0) | LVT_MASKED);
        write(LVT_LINT1, read(LVT_LINT1) | LVT_MASKED);
        if(lvts >= 4) {
            write(LVT_PERF, read(LVT_PERF) | LVT_MASKED);
            write(LVT_THERMAL, read(LVT_THERMAL) | LVT_MASKED);
        }

        // Clean APIC state
        write(LVT_TIMER, LVT_MASKED);
        write(LVT_LINT0, LVT_MASKED);
        write(LVT_LINT1, LVT_MASKED);
        if(lvts >= 3)
            write(LVT_ERROR, LVT_MASKED);
        if(lvts >= 4) {
            write(LVT_PERF, LVT_MASKED);
            write(LVT_THERMAL, LVT_MASKED);
        }
        if(read(VERSION) & 0xf) { // !82489DX
            if(lvts > 3)
                write(ESR, 0);
            read(ESR);
        }
    }

    static void connect() {
        CPU::out8(0x22, 0x70);
        CPU::out8(0x23, 0x01);
    }

    static void disconnect() {
        CPU::out8(0x22, 0x70);
        CPU::out8(0x23, 0x00);
    }

private:
    static Log_Addr _base;
};

class IO_APIC // Intel 82093AA
{
private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Reg64 Reg64;
    typedef CPU::Log_Addr Log_Addr;

public:
    class IO_Redirection_Table_Entry
    {
    public:
        IO_Redirection_Table_Entry(Log_Addr base) {
            _base = base;
        }

    private:
        enum {
            HIGH = 1,
            LOW  = 0
        };
        // Whether field lies on the high[63:32] = 1 or low[31:0] part of the entry
        enum {
            DESTINATION_PART = HIGH,
            INTMASK_PART     = LOW,
            TRGMOD_PART      = LOW,
            RIRR_PART        = LOW,
            INTPOL_PART      = LOW,
            DELIVS_PART      = LOW,
            DESTMOD_PART     = LOW,
            DELMOD_PART      = LOW,
            INTVEC_PART      = LOW
        };

        // Left-shifts for writing into fields
        enum {
            DESTINATION_SHIFT = 24,     // Destination Field (R/W) Bit[63:56]
            INTMASK_SHIFT     = 16,     // Interrupt Mask (R/W) Bit[16]
            TRGMOD_SHIFT      = 15,     // Trigger Mode (R/W) Bit[15]
            RIRR_SHIFT        = 14,     // Remote IRR (RO) Bit[14]
            INTPOL_SHIFT      = 13,     // Interrupt Input Pin Polarity (R/W) Bit[13]
            DELIVS_SHIFT      = 12,     // Delivery Status (RO) Bit[12]
            DESTMOD_SHIFT     = 11,     // Destination Mode (R/W) Bit[11]
            DELMOD_SHIFT      = 8       // Delivery Mode (R/W) Bit[10:8]
        };

        // 32-bit AND-masks (use as-it-is for reading and negate for writing)
        enum {
            DESTINATION       =   0xff000000, // Destination Field (R/W) Bit[63:56]
            INTMASK           =   0x00010000, // Interrupt Mask (R/W) Bit[16]
            TRGMOD            =   0x00008000, // Trigger Mode (R/W) Bit[15]
            RIRR              =   0x00004000, // Remote IRR (RO) Bit[14]
            INTPOL            =   0x00002000, // Interrupt Input Pin Polarity (R/W) Bit[13]
            DELIVS            =   0x00001000, // Delivery Status (RO) Bit[12]
            DESTMOD           =   0x00000800, // Destination Mode (R/W) Bit[11]
            DELMOD            =   0x00000700, // Delivery Mode (R/W) Bit[10:8]
            INTVEC            =   0x000000ff,  // Interrupt Vector (R/W) Bit[7:0]
        };


    public:
        // Field Values: FIELDNAME_VALUEDESCRIPTION
        enum Trigger_Mode {
            TRGMOD_EDGE_SENSITIVE      = 0,
            TRGMOD_LEVEL_SENSITIVE     = 1
        };

        enum Destination_Mode {
            DESTMOD_PHY                = 0,
            DESTMOD_LOG                = 1
        };

        enum Delivery_Mode {
            DELMOD_FIXED               = 0x0, // Fixed
            DELMOD_LSP                 = 0x1, // Lowest Priority
            DELMOD_SMI                 = 0x2, // System Management Interrupt
            DELMOD_NMI                 = 0x4, // Non-Maskable Interrupt
            DELMOD_INT                 = 0x5, // INT
            DELMOD_EXTINT              = 0x7  // ExtINT
        };

        enum {
            INTMASK_MASKED             = 1,
            INTPOL_LOW_ACTIVE          = 1
        };

    public:
        /*! If Destination Mode is 0, Physical Mode, dest is an APIC ID
         * (4-bit long), if Destination Mode is 1, Logical Mode, dest is an
         * set of processors (8-bit long)
         * */
        void destination(Reg8 dest) {
            ioapic_write(_base + DESTINATION_PART, ioapic_read(_base + DESTINATION_PART) & ~DESTINATION);
            ioapic_write(_base + DESTINATION_PART, ioapic_read(_base + DESTINATION_PART) | (dest << DESTINATION_SHIFT));
        }
        Reg8 destination() const {
            return (ioapic_read(_base + DESTINATION_PART) & DESTINATION) >> DESTINATION_SHIFT;
        }

        void mask_interrupt() {
            ioapic_write(_base + INTMASK_PART, ioapic_read(_base + INTMASK_PART) | (INTMASK_MASKED << INTMASK_SHIFT));
        }
        void unmask_interrupt() {
            ioapic_write(_base + INTMASK_PART, ioapic_read(_base + INTMASK_PART) & (~(INTMASK_MASKED << INTMASK_SHIFT)));
        }
        bool interrupt_masked() const {
            return (ioapic_read(_base + INTMASK_PART) & INTMASK) >> INTMASK_SHIFT;
        }

        void interrupt_low_active() {
            ioapic_write(_base + INTPOL_PART, ioapic_read(_base + INTPOL_PART) | (INTPOL_LOW_ACTIVE << INTPOL_SHIFT));
        }
        void interrupt_high_active() {
            ioapic_write(_base + INTPOL_PART, ioapic_read(_base + INTPOL_PART) & (~(INTPOL_LOW_ACTIVE << INTPOL_SHIFT)));
        }
        /*! Returns true if interrupt input pin polarity is low active.
         * Returns false otherwise
         */
        bool low_actived_interrupt() const {
            return (ioapic_read(_base + INTPOL_PART) & INTPOL) >> INTPOL_SHIFT;
        }

        void trigger_mode(Trigger_Mode mode) {
            if (mode == TRGMOD_LEVEL_SENSITIVE) {
                ioapic_write(_base + TRGMOD_PART, ioapic_read(_base + TRGMOD_PART) | (TRGMOD_LEVEL_SENSITIVE << TRGMOD_SHIFT));
            } else { // TRGMOD_EDGE_SENSITIVE
                ioapic_write(_base + TRGMOD_PART, ioapic_read(_base + TRGMOD_PART) & (~(TRGMOD_EDGE_SENSITIVE << TRGMOD_SHIFT)));
            }
        }
        /*! Returns true if trigger mode is Level Sensitive Mode.
         * Returns and false otherwise
         */
        bool level_sensitive_trigger_mode() const {
            return (ioapic_read(_base + TRGMOD_PART) & TRGMOD) >> TRGMOD_SHIFT;
        }

        void destination_mode(Destination_Mode mode) {
            if (mode == DESTMOD_LOG) {
                ioapic_write(_base + DESTMOD_PART, ioapic_read(_base + DESTMOD_PART) | (DESTMOD_LOG << DESTMOD_SHIFT));
            } else { // DESTMOD_PHY
                ioapic_write(_base + DESTMOD_PART, ioapic_read(_base + DESTMOD_PART) & (~(DESTMOD_LOG << DESTMOD_SHIFT)));
            }
        }
        /*! Returns true if destination mode is Logical Mode.
         *  Returns false otherwise.
         */
        bool logical_destination_mode() const {
            return (ioapic_read(_base + DESTMOD_PART) & DESTMOD) >> DESTMOD_SHIFT;
        }

        void delivery_mode(Delivery_Mode mode) {
            ioapic_write(_base + DELMOD_PART, ioapic_read(_base + DELMOD_PART) & (~DELMOD));
            ioapic_write(_base + DELMOD_PART, ioapic_read(_base + DELMOD_PART) | (mode << DELMOD_SHIFT));
        }
        Reg8 delivery_mode() const {
            return (ioapic_read(_base + DELMOD_PART) & DELMOD) >> DELMOD_SHIFT;
        }

        void interrupt_vector(Reg8 vector) {
            ioapic_write(_base + INTVEC_PART, ioapic_read(_base + INTVEC_PART) & (~INTVEC));
            ioapic_write(_base + INTVEC_PART, ioapic_read(_base + INTVEC_PART) | vector);
        }
        Reg8 interrupt_vector() const {
            return ioapic_read(_base + INTVEC_PART) & INTVEC;
        }

        Reg8 delivery_status() const {
            return (ioapic_read(_base + DELIVS_PART) & DELIVS) >> DELIVS_SHIFT;
        }

        Reg8 remote_irr() const {
            return (ioapic_read(_base + RIRR_PART) & RIRR) >> RIRR_SHIFT;
        }

    friend Debug & operator<<(Debug & db, const IO_Redirection_Table_Entry & entry) {
        db << "{dst = " << entry.destination()
           << ", intvec = " << entry.interrupt_vector()
           << ", masked = " << (entry.interrupt_masked() ? "Y" : "N")
           << ", destmod = " << (entry.logical_destination_mode() ? "L" : "P")
           << ", trgmod = " << (entry.level_sensitive_trigger_mode() ? "L" : "E")
           << ", delmod = " << reinterpret_cast<void *>(entry.delivery_mode())
           << ", delst = " << reinterpret_cast<void *>(entry.delivery_status())
           << ", rirr = " << reinterpret_cast<void *>(entry.remote_irr())
           << ", intpol = " << (entry.low_actived_interrupt() ? "L" : "H")
           << "}";
        return db;
    }

    private:
        Log_Addr _base;
    };

    typedef IO_Redirection_Table_Entry IRT_Entry;

private:
    /* Like the local APIC, the IOAPIC is controlled via memory-mapped
     * registers.
     * Unlike the local APIC, however, the IOAPIC only has two 32-bit
     * registers; an index register (IOREGSEL) and a data register (IOWIN).
     * To access a register i, you write the value i into the index register and
     * read/write from/to the data register.
     * That can be accomplished by using the ioapic_read and ioapic_write
     * methods.
     */
    // Memory-mapped registers for reading IO_APIC registers
    enum {
        IOREGSEL        = 0x00,         // I/O Register Select "INDEX_REG"  (R/W)
        IOWIN           = 0x10          // I/O Window Register "DATA_REG"   (R/W)
    };

    // IOAPIC registers
    enum {
        IOAPICID        = 0x00,         // IOAPIC ID (R/W)
        IOAPICVER       = 0x01,         // IOAPIC Version (RO)
        IOAPICARB       = 0x02,         // IOAPIC Arbitration ID (RO)
        IOREDTBL_BASE   = 0x10          // Base of I/O Redirection Table (IRT), Entries 0-23, 64 bits each (R/W)
    };

    // AND-masks for reading bits of the IOAPICID register
    enum {
        IOAPICID_ID     = 0x0f000000    // IOAPIC identification (R/W) Bit[27:24]
    };

    // AND-masks and shifts for reading bits of the IOAPICVER register
    enum {
        IOAPICVER_MRE   = 0x00ff0000,   // Maximum Redirection Entry in IRT (RO) Bit[23:16]
        IOAPICVER_VER   = 0x000000ff,   // Version (RO) Bit[7:0]
        IOAPICVER_MRE_SHIFT = 16
    };

    // AND-masks and shifts for reading bits of IOAPICARB register
    enum {
        IOAPICARB_ID    = 0x0f000000,    // IOAPIC Arbitration identification (R/W) Bit[27:24]
        IOAPICARB_ID_SHIFT = 24
    };

public:

    /// Returns IO_APIC ID
    static Reg32 id() {
        return (ioapic_read(IOAPICID) & IOAPICID_ID) >> IOAPICARB_ID_SHIFT;
    }

    /// Returns Implementation Version of IO_APIC
    static Reg32 version() {
        return ioapic_read(IOAPICVER) & IOAPICVER_VER;
    }

    /// Returns Maximum Redirection Entry in IRT
    /*! Returns the entry number (0 being the lowest entry) of the highest entry
     *  in the I/O Redirection Table (IRT).
     *  The value is equal to the number of interrupt input pins for the
     *  IOAPIC minus one.
     *  The range of values is 0 through 239.
     *  For 82093AA, the value is 0x17 (23).
     * */
    static unsigned int maximum_redirection_entry() {
        return (ioapic_read(IOAPICVER) & IOAPICVER_MRE) >> IOAPICVER_MRE_SHIFT;
    }

    /// Returns the entry i of IRT
    static IRT_Entry irt_entry(unsigned int i) {
        IRT_Entry entry = IRT_Entry(IOREDTBL_BASE + (i * 2));
        return entry;
    }

    /// Returns IOAPIC arbitration ID
    static Reg32 arbitration_id() {
        return (ioapic_read(IOAPICARB) & IOAPICARB_ID) >> 24;
    }

    static void remap(Log_Addr addr) {
        db<IC>(TRC) << "IO_APIC::remap, addr = " << addr << endl;
        _base = addr;
        db<IC>(TRC) << "IO_APIC::remap, _base = " << _base << endl;
    }


    static Reg32 ioapic_read(Reg32 index) {
        db<IC>(TRC) << "ioapic_read, index: " << reinterpret_cast<void *>(index) << endl;

        *reinterpret_cast<volatile Reg32 *>(_base + IOREGSEL) = index;

        db<IC>(TRC) << "indexed, will return data..." << endl;

        return *reinterpret_cast<volatile Reg32 *>(_base + IOWIN);
    }


    static void ioapic_write(Reg32 index, Reg32 data) {
        db<IC>(TRC) << "ioapic_write, index: " << reinterpret_cast<void *>(index) << " , data: " << data << endl;

        *reinterpret_cast<volatile Reg32 *>(_base + IOREGSEL) = index;
        *reinterpret_cast<volatile Reg32 *>(_base + IOWIN) = data;
    }


    /*! Set the given IRT entry with '''apic_id''' as destination
     * (assuming physical destination mode), sets the interrupt vector with
     * '''vector''', set delivery mode to Fixed, and unmasks the interrupt.
     * */
    static void set_irt_entry(Reg8 entry_index, Reg64 apic_id, Reg8 vector) {
        db<IC>(TRC) << "IO_APIC::set_irt_entry, entry_index: " << entry_index << " , apic_id: " << apic_id << ", vector: " << vector << ", _base: " << _base << endl;

        const Reg32 low_index = 0x10 + entry_index * 2;
        const Reg32 high_index = 0x10 + entry_index * 2 + 1;

        Reg32 high = ioapic_read(high_index);
        // set APIC ID
        high &= ~0xff000000;
        high |= apic_id << 24;
        ioapic_write(high_index, high);

        Reg32 low = ioapic_read(low_index);

        // unmask the IRQ
        low &= ~(1<<16);

        // set to physical destination mode
        low &= ~(1<<11);

        // set to fixed delivery mode
        low &= ~0x700;

        // set delivery vector
        low &= ~0xff;
        low |= vector;

        ioapic_write(low_index, low);

        db<IC>(TRC) << "IO_APIC::set_irq, irq set done." << endl;
    }

private:
    static Log_Addr _base;
};

// IC uses i8259A on single-processor machines and the APIC timer on MPs
class IC: private IC_Common, private IF<Traits<System>::multicore, APIC, i8259A>::Result
{
    friend class Machine;

private:
    typedef IF<Traits<System>::multicore, APIC, i8259A>::Result Engine;

    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_RESCHEDULER;
    using Engine::INT_SYSCALL;
    using Engine::INT_TIMER;
    using Engine::INT_KEYBOARD;

    using Engine::ipi_send;

public:
    IC() {}

    static Interrupt_Handler int_vector(const Interrupt_Id & i) {
        assert(i < INTS);
        return _int_vector[i];
    }

    static void int_vector(const Interrupt_Id & i, const Interrupt_Handler & h) {
        db<IC>(TRC) << "IC::int_vector(int=" << i << ",h=" << reinterpret_cast<void *>(h) <<")" << endl;
        assert(i < INTS);
        _int_vector[i] = h;
    }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        assert(i < INTS);
        Engine::enable();
    }

    static void enable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::enable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        assert(i < INTS);
        Engine::disable();
    }

    static void disable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::disable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::disable(i);
    }

    using Engine::irq2int;

private:
    static void dispatch(unsigned int i) {
        bool not_spurious = true;
        if((i >= INT_FIRST_HARD) && (i <= INT_LAST_HARD))
            not_spurious = eoi(i);
        if(not_spurious) {
            if((i != INT_TIMER) || Traits<IC>::hysterically_debugged)
                db<IC>(TRC) << "IC::dispatch(i=" << i << ")" << endl;
            _int_vector[i](i);
        } else {
            if(i != INT_LAST_HARD)
                db<IC>(TRC) << "IC::spurious interrupt (" << i << ")" << endl;
        }
    }

    // Logical handlers
    static void int_not(const Interrupt_Id & i);

    // Physical handlers
    static void entry();
    static void exc_not(Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 error);
    static void exc_pf (Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 error);
    static void exc_gpf(Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 error);
    static void exc_fpu(Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 error);

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
};

__END_SYS

#endif
