// EPOS Cortex_M I2C Mediator Declarations

#include __MODEL_H

#ifndef __cortex_m_i2c_h__
#define __cortex_m_i2c_h__

#include <cpu.h>
#include <i2c.h>
#include <machine/cortex_m/emote3.h>
#include <machine/cortex_m/emote3_i2c.h>
#include <system/traits.h>

__BEGIN_SYS

class Cortex_M_I2C: public I2C_Common, public Cortex_M_Model_I2C
{
private:
    typedef Cortex_M_Model_I2C Engine;

public:

    Cortex_M_I2C() : Engine(0) {
    }

    Cortex_M_I2C(Reg32 master, char port_sda, unsigned int pin_sda, char port_scl, unsigned int pin_scl) :
        Engine(master, port_sda, pin_sda, port_scl, pin_scl)
    {
    }

    void config(char port_sda, unsigned int pin_sda, char port_scl, unsigned int pin_scl) {
        kout << "Port SDA=" << port_sda << " PIN SDA=" << pin_sda << " Port SCL=" << port_scl << " PIN SCL=" << pin_scl << endl;
        Engine::config(port_sda, pin_sda, port_scl, pin_scl);
    }

    void config() {
        Engine::config();
    }

    void disable() { Engine::disable(); }
    void enable() { Engine::enable(); }
    bool is_busy() { return Engine::is_busy(); }
};

__END_SYS

#endif
