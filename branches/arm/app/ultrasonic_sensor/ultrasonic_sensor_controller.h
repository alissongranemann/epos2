#ifndef ultrasonic_sensor_controller_hdr
#define ultrasonic_sensor_controller_hdr

#include "ultrasonic_sensor_hcsr04.h"
#include <alarm.h>

using namespace EPOS;

#define HCSR04_RELAY //comment this line if no relays are used with the sensor for power management

class Ultrasonic_Sensor_Controller{
protected:

    enum Sample_strategy{
        NUMBER_OF_READS = 5, // arbitrary
        INTERVAL_BETWEEN_READS = 60000, //60ms recommended by datasheet

        #ifdef HCSR04_RELAY
        RELAY_DELAY = 2000000 //2s
        #endif
    };

    Ultrasonic_Sensor_HC_SR04 _sensor;

public:
    typedef void (*Sense_Callback)(int) ;
    typedef int (*Sense_Callback_Dynamic)(int);

    #ifdef HCSR04_RELAY // if using relays

    Ultrasonic_Sensor_Controller(GPIO relay,GPIO trigger,GPIO echo) : _sensor(relay,trigger,echo){}

    #else
    Ultrasonic_Sensor_Controller(GPIO trigger,GPIO echo) : _sensor(trigger,echo){}
    #endif

    int sense(){
        return evaluate_strategy();
    }

    void sense(unsigned int interval, unsigned int times, int * const values){
        for(int i = 0; i < times; ++i){
            values[i] = evaluate_strategy();
            Alarm::delay(interval);
        }
    }

    void sense(unsigned int interval, Sense_Callback f){
        while(1){
            f(evaluate_strategy());
            Alarm::delay(interval);
        }
    }

    void sense(Sense_Callback_Dynamic f){
        while(1){
            Alarm::delay(f(evaluate_strategy()));
        }
    }

protected:

    //The simplest and naive sample strategy is a simple forward to the sensor method.
    /*int evaluate_strategy(){
        return _sensor.sense();
    }*/

    int evaluate_strategy(){
        int *samples, ret;

        #ifdef HCSR04_RELAY // if using relays
        _sensor.enable();
        Alarm::delay(Sample_strategy::RELAY_DELAY);
        #endif

        ret = 0;
        samples = new int[Sample_strategy::NUMBER_OF_READS];
        for(int i = 0; i < Sample_strategy::NUMBER_OF_READS; ++i){
            int c = _sensor.sense();
            samples[i] = c;
            Alarm::delay(Sample_strategy::INTERVAL_BETWEEN_READS);
        }

        #ifdef HCSR04_RELAY // if using relays
        _sensor.disable();
        #endif

        for(int i = 0; i < Sample_strategy::NUMBER_OF_READS; ++i){
            ret += (samples[i] == -1)? 0 : samples[i];
        }

        ret /= Sample_strategy::NUMBER_OF_READS;
        delete[] samples;
        return ret;
    }
};

#endif
