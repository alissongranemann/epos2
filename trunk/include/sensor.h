// EPOS Smart Sensor Declarations

#ifndef __sensor_h
#define __sensor_h

#include <smart_data.h>

#include <keyboard.h>

__BEGIN_SYS

typedef TSTP::Region Region;
typedef TSTP::Coordinates Coordinates;

class Keyboard_Sensor: public PC_Keyboard
{
public:
    static const unsigned int UNIT = TSTP::Unit::Acceleration;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef PC_Keyboard::Observer Observer;
    typedef PC_Keyboard::Observed Observed;

public:
    Keyboard_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Keyboard_Sensor> * data) {
        data->_value = try_getc();
    }

    static void actuate(unsigned int dev, Smart_Data<Keyboard_Sensor> * data, void * command) {}
};

typedef Smart_Data<Keyboard_Sensor> Acceleration;

__END_SYS

#endif
