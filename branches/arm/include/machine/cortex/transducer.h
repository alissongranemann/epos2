// EPOS ARM Cortex Smart Transducer Declarations

#ifndef __cortex_transducer_h
#define __cortex_transducer_h

#include <smart_data.h>

#include <keyboard.h>
#include <smart_plug.h>
#include <hydro_board.h>
#include <gpio.h>
#include <tstp.h>
#include <tsc.h>

__BEGIN_SYS

typedef TSTP::Region Region;
typedef TSTP::Coordinates Coordinates;

class Keyboard_Sensor: public Keyboard
{
public:
    static const unsigned int UNIT = TSTP::Unit::Acceleration;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef Keyboard::Observer Observer;
    typedef Keyboard::Observed Observed;

public:
    Keyboard_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Keyboard_Sensor> * data) {
        if(ready_to_get())
            data->_value = get();
        else
            data->_value = -1;
    }

    static void actuate(unsigned int dev, Smart_Data<Keyboard_Sensor> * data, void * command) {}
};

#ifdef __mmod_emote3__

class Water_Level_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Level_Sensor> * data) {
        data->_value = level(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Level_Sensor> * data, void * command) {}
};

class Water_Turbidity_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data) {
        data->_value = turbidity(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data, void * command) {}
};

class Water_Flow_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = (TSTP::Unit::SI) | (TSTP::Unit::DIR) | ((4 + 3) * TSTP::Unit::M) | ((4 - 1) * TSTP::Unit::S); // m^3/s
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data) {
        data->_value = water_flow();
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data, void * command) {}
};

class Pluviometer: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::DIV | TSTP::Unit::Length; // TODO: we want mm, or mm/m^2
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Pluviometer> * data) {
        data->_value = rain();
    }

    static void actuate(unsigned int dev, Smart_Data<Pluviometer> * data, void * command) {}
};

class Current_Sensor: public Smart_Plug
{
public:
    static const unsigned int UNIT = TSTP::Unit::Current;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Smart_Plug::Observer Observer;
    typedef Smart_Plug::Observed Observed;

public:
    static void sense(unsigned int dev, Smart_Data<Current_Sensor> * data) {
        data->_value = current(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Current_Sensor> * data, void * command) {
        act(dev, 0); // TODO
    }
};

class ADC_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Luminous_Intensity;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    ADC_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<ADC_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.read();
    }

    static void actuate(unsigned int dev, Smart_Data<ADC_Sensor> * data, void * command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class Temperature_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Temperature;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    Temperature_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Temperature_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.read();
    }

    static void actuate(unsigned int dev, Smart_Data<Temperature_Sensor> * data, void * command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class GPIO_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::DIGITAL + 1;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    GPIO_Sensor() {}

    GPIO_Sensor(unsigned int dev, char port, unsigned int pin) {
        _dev[dev] = new GPIO(port, pin, GPIO::IN, GPIO::Pull::UP, &int_handler, GPIO::Edge::BOTH);
    }

    static void sense(unsigned int dev, Smart_Data<GPIO_Sensor> * data) {
        data->_value = _dev[dev]->get();
    }

    static void actuate(unsigned int dev, Smart_Data<GPIO_Sensor> * data, void * command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static void int_handler(const unsigned int & id) { notify(); }

    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
    static GPIO * _dev[32];
};

typedef Smart_Data<Current_Sensor> Current;
typedef Smart_Data<ADC_Sensor> Luminous_Intensity;
typedef Smart_Data<Temperature_Sensor> Temperature;
typedef Smart_Data<GPIO_Sensor> Presence;
typedef Smart_Data<Water_Flow_Sensor> Water_Flow;
typedef Smart_Data<Water_Level_Sensor> Water_Level;
typedef Smart_Data<Water_Turbidity_Sensor> Water_Turbidity;
typedef Smart_Data<Pluviometer> Rain;

#endif

typedef Smart_Data<Keyboard_Sensor> Acceleration;

__END_SYS

#endif
