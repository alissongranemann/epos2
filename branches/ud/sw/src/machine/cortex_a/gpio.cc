// EPOS Cortex-A GPIO Mediator Implementation

#include <machine/cortex_a/gpio.h>

__BEGIN_SYS

const unsigned int Cortex_A_GPIO::PIN_TABLE[BANK_NUM] = {
    31, // 0 - 31
    53, // 32 - 53
    85, // 54 - 85
    117 // 86 - 117
};

Cortex_A_GPIO::Cortex_A_GPIO(unsigned int pin, GPIO_Functions func) {
    // Discover pin's bank number
    for (_bank = 0; _bank < BANK_NUM; _bank++)
        if (pin <= PIN_TABLE[_bank])
            break;

    // Discover the pin number within that bank
    if (!_bank)
        _bank_pin = pin;
    else
        _bank_pin = pin % (PIN_TABLE[_bank - 1] + 1);
}

// Set pin direction as input
void Cortex_A_GPIO::input() {
    Reg32 reg = dirm();

    reg &= ~(1<<_bank_pin);
    dirm(reg);
}

// Set pin direction as output
void Cortex_A_GPIO::output() {
    // Set the GPIO pin as output
    Reg32 reg = dirm();
    reg |= 1<<_bank_pin;
    dirm(reg);

    // Configure the output enable register for the pin
    reg = oen();
    reg |= 1<<_bank_pin;
    oen(reg);
}

// Set pin status
void Cortex_A_GPIO::put(bool value) {
    Reg32 reg = data();

    if(value)
        reg |= 1<<_bank_pin;
    else
        reg &= ~(1<<_bank_pin);

    data(reg);
}

// Read pin status
bool Cortex_A_GPIO::get() {
    return (data_ro()&(1<<_bank_pin) ? true : false);
}

__END_SYS
