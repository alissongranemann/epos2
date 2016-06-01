// EPOS Cortex-A GPIO Mediator Declarations

#ifndef __cortex_a_gpio_h
#define __cortex_a_gpio_h

#include <gpio.h>
#include <machine.h>

__BEGIN_SYS

// Based on Linux's zynq-gpio device driver
class Cortex_A_GPIO: public GPIO_Common
{
public:
    // The GPIOs have no alternate functions in Zynq
    enum GPIO_Functions {
        FUNC_DFLT = 0
    };

public:
    Cortex_A_GPIO(unsigned int pin, GPIO_Functions func = FUNC_DFLT);

    ~Cortex_A_GPIO() {};

    void put(bool value);
    bool get();

    void input();
    void output();
    void function(GPIO_Functions func) { }

private:
    typedef CPU::Reg32 Reg32;

    static const unsigned int BANK_NUM = 4;

    static const unsigned int PIN_TABLE[BANK_NUM];

    enum {
        BASE = 0xE000A000
    };

    // Register offsets for bank 0
    enum {
        MASK_DATA_0_LSW = 0x000,
        MASK_DATA_0_MSW = 0x004,
        DATA_0          = 0x040,
        DATA_0_RO       = 0x060,
        DIRM_0          = 0x204,
        OEN_0           = 0x208,
        INT_MASK_0      = 0x20C,
        INT_EN_0        = 0x210,
        INT_DIS_0       = 0x214,
        INT_STAT_0      = 0x218,
        INT_TYPE_0      = 0x21C,
        INT_POLARITY_0  = 0x220,
        INT_ANY_0       = 0x224
    };

private:
    Reg32 reg(Reg32 addr) { return CPU::in32(BASE + addr); }
    void reg(Reg32 addr, Reg32 value) { CPU::out32(BASE + addr, value); }

    Reg32 data() { return reg(DATA_0 + 4*_bank); }
    void data(Reg32 value) { reg(DATA_0 + 4*_bank, value); }

    Reg32 data_ro() { return reg(DATA_0_RO + 4*_bank); }
    void data_ro(Reg32 value) { reg(DATA_0_RO + 4*_bank, value); }

    Reg32 dirm() { return reg(DIRM_0 + 0x40*_bank); }
    void dirm(Reg32 value) { reg(DIRM_0 + 0x40*_bank, value); }

    Reg32 oen() { return reg(OEN_0 + 0x40*_bank); }
    void oen(Reg32 value) { reg(OEN_0 + 0x40*_bank, value); }

private:
    unsigned int _bank_pin;
    unsigned int _bank;
};

__END_SYS

#endif
