// EPOS SHT11 Sensor Mediator Declarations

#ifndef __cortex_m_sht11_h
#define __cortex_m_sht11_h

#include <alarm.h>
#include "emote3_gpio.h"

// TODO: This sensor wasn't tested with eMote3

__BEGIN_SYS

class Cortex_M_SHT11
{
protected:
    // The SCK pin frequency is set to 1 kHz
    static const unsigned int T_SCK_US = 1000;

    // Sensor commands
    enum {
        MEASURE_TEMPERATURE = 0x03,
        MEASURE_HUMIDITY    = 0x05,
        WRITE_STATUS_REG    = 0x06,
        READ_STATUS_REG     = 0x07,
        RESET               = 0x1E
    };

    enum {
        NO_ACK  = 0,
        ACK     = 1
    };

protected:
    Cortex_M_SHT11(char data_port, int data_pin, char sck_port, int sck_pin) {
        _data = new GPIO(data_port, data_pin, GPIO::OUTPUT);
        _sck = new GPIO(sck_port, sck_pin, GPIO::OUTPUT);

        enable();
    }

    ~Cortex_M_SHT11() {
        delete _sck;
        delete _data;

        disable();
    }

    unsigned char write_byte(unsigned char value);
    unsigned char read_byte(unsigned char ack);

    void transmission_start();

    void connection_reset();

    unsigned char soft_reset();

    int measure(unsigned char mode);

public:
    bool enable();
    void disable();

    int sample() { return 0; }
    int get() { return 0; }

protected:
    bool _in_use;
    int _humidity;
    GPIO * _sck;
    GPIO * _data;
};

class Cortex_M_SHT11_Humidity: public Cortex_M_SHT11
{
public:
    Cortex_M_SHT11_Humidity(char data_port, int data_pin, char sck_port, int sck_pin):
        Cortex_M_SHT11(data_port, data_pin, sck_port, sck_pin) {};

    ~Cortex_M_SHT11_Humidity() {}

    int sample();
    int get() { return _humidity; }

private:
    int compensate(int value);
};

__END_SYS

#endif
