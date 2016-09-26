// EPOS ARM Cortex GPIO Mediator Declarations

#if !defined(__cortex_gpio_h_) && !defined(__mmod_zynq__)
#define __cortex_gpio_h_

#include <machine.h>
#include <ic.h>
#include <gpio.h>

__BEGIN_SYS

class GPIO: public GPIO_Common, private Machine_Model
{
private:
    static const bool supports_power_up = Machine_Model::supports_gpio_power_up;

public:
    GPIO(char port, unsigned int pin, const Direction & dir, const Pull & p = UP, const IC::Interrupt_Handler & handler = 0)
    : _port(port - 'A'), _pin(pin), _pin_bit(1 << pin), _data(&gpio(_port, _pin_bit << 2)), _handler(handler) {
        assert((port >= 'A') && (port <= 'A' + GPIO_PORTS));
        gpio(_port, AFSEL) &= ~_pin_bit; // Set pin as software controlled
        direction(dir);
        pull(p);
        clear_interrupt();
        if(handler) {
            _devices[_port][_pin] = this;
            int_enable();
        }
    }

    bool get() const {
        assert(_direction == IN);
        return *_data;
    }

    void set(bool value) {
        assert(_direction == INOUT || _direction == OUT);
        if(_direction == INOUT) {
            gpio(_port, DIR) |= _pin_bit;
            *_data = 0xff * value;
            gpio(_port, DIR) &= ~_pin_bit;
        } else
            *_data = 0xff * value;
    }

    void direction(const Direction & dir) {
        _direction = dir;
        switch(dir) {
            case OUT:
                gpio(_port, DIR) |= _pin_bit;
                break;
            case IN:
            case INOUT:
                gpio(_port, DIR) &= ~_pin_bit;
                break;
        }
    }

    void pull(const Pull & p) {
        switch(p) {
            case UP:
                gpio_pull_up(_port, _pin);
                break;
            case DOWN:
                gpio_pull_down(_port, _pin);
                break;
            case FLOATING:
                gpio_floating(_port, _pin);
                break;
        }
    }

    void int_enable() { gpio(_port, IM) |= _pin_bit; }
    void int_enable(const Level & level, bool power_up = false, const Level & power_up_level = HIGH);
    void int_enable(const Edge & edge, bool power_up = false, const Edge & power_up_edge = RISING);
    void int_disable() { gpio(_port, IM) &= ~_pin_bit; }

private:
    void clear_interrupt() {
        gpio(_port, ICR) = _pin_bit;
        gpio(_port, IRQ_DETECT_ACK) &= ~(_pin_bit << (8 * _port));
    }

    static void handle_int(const IC::Interrupt_Id & i);

private:
    unsigned char _port;
    unsigned char _pin;
    unsigned int _pin_bit;
    Direction _direction;
    volatile Reg32 * _data;
    IC::Interrupt_Handler _handler;

    static GPIO * _devices[GPIO_PORTS][8];
};

__END_SYS

#endif
