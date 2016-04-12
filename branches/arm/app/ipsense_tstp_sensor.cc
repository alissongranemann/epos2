#include <machine.h>
#include <alarm.h>
#include <gpio.h>
#include <utility/ostream.h>
#include <utility/string.h>
#include <adc.h>
#include <uart.h>
#include <timer.h>
#include <wiegand.h>
#include <network.h>

using namespace EPOS;

TSTP * tstp;

OStream cout;

GPIO * led;

GPIO * relay[8];
GPIO * input[7];

Wiegand * wie[4];

Wiegand::Door_State_1 door_1;
Wiegand::Door_State_2 door_2;
Wiegand::Door_State_3 door_3;
Wiegand::Door_State_4 door_4;

struct Handler_Input0 : public Handler {
    void operator()() { 
        relay[1]->set(true);
        led->set(true);
        //User_Timer_3::delay(500000);
        relay[1]->set(false);
        led->set(false);
    }
} handlerinput0;

struct Handler_Input1 : public Handler {
    void operator()() { 
        led->set(true);
        auto val = door_1.data<bool>();
        *val = pin->read();
        tstp->event(door_1);
        //User_Timer_3::delay(500000);
        led->set(false);
    }
    GPIO * pin;
};

struct Handler_Input2 : public Handler {
    void operator()() { 
        relay[3]->set(true);
        //messagemount('R', 3, 'W', 1);
        //User_Timer_3::delay(500000);
        relay[3]->set(false);
        //messagemount('R', 3, 'W', 0);
    }
} handlerinput2;

struct Handler_Input3 : public Handler {
    void operator()() { 
        relay[4]->set(true);
        //messagemount('R', 4, 'W', 1);
        //User_Timer_3::delay(500000);
        relay[4]->set(false);
        //messagemount('R', 4, 'W', 0);
    }
} handlerinput3;

struct Handler_Input4 : public Handler {
    void operator()() { 
        relay[5]->set(true);
        //messagemount('R', 5, 'W', 1);
        //User_Timer_3::delay(500000);
        relay[5]->set(false);
        //messagemount('R', 5, 'W', 0);
    }
} handlerinput4;

struct Handler_Input5 : public Handler {
    void operator()() { 
        relay[6]->set(true);
        //messagemount('R', 6, 'W', 1);
        //User_Timer_3::delay(500000);
        relay[6]->set(false);
        //messagemount('R', 6, 'W', 0);
    }
} handlerinput5;


struct Handler_Input6 : public Handler {
    void operator()() { 
        relay[7]->set(true);
        //messagemount('R', '7', 'W', 1);
        //User_Timer_3::delay(500000);
        relay[7]->set(false);
        //messagemount('R', '7', 'W', 0);
    }
} handlerinput6;

int main () {
    /*
    int n = 25000;
    while(n--) {
        cout << n << endl;
    }
    */
    Network::init();
    tstp = TSTP::get_by_nic(0);
    tstp->bootstrap();

    /*
    {
        Wiegand::RFID_1 id_1;
        Wiegand::RFID_2 id_2;
        Wiegand::RFID_3 id_3;
        Wiegand::RFID_4 id_4;
        TSTP::Sensor sd1(tstp, &door_1, 0, 0, 0);
        TSTP::Sensor sd2(tstp, &door_2, 0, 0, 0);
        TSTP::Sensor sd3(tstp, &door_3, 0, 0, 0);
        TSTP::Sensor sd4(tstp, &door_4, 0, 0, 0);
        TSTP::Sensor s2(tstp, &id_1, 0, 0, 0);
        TSTP::Sensor s3(tstp, &id_2, 0, 0, 0);
        TSTP::Sensor s4(tstp, &id_3, 0, 0, 0);
        TSTP::Sensor s5(tstp, &id_4, 0, 0, 0);

        while(true);
    }
    */

    led = new GPIO('c',3, GPIO::OUTPUT);

    //begin at [1] because of khomp standards

    relay[1] = new GPIO('d',1,GPIO::OUTPUT);
    relay[2] = new GPIO('d',3,GPIO::OUTPUT);
    relay[3] = new GPIO('d',5,GPIO::OUTPUT);
    relay[4] = new GPIO('b',6,GPIO::OUTPUT);
    relay[5] = new GPIO('b',0,GPIO::OUTPUT);
    relay[6] = new GPIO('b',2,GPIO::OUTPUT);
    relay[7] = new GPIO('b',4,GPIO::OUTPUT);

    //for (int i=0; i<7;i++)
    //    relay[i]->clear();

    Wiegand::RFID_1 id_1;
    Wiegand::RFID_2 id_2;
    Wiegand::RFID_3 id_3;
    Wiegand::RFID_4 id_4;

    TSTP::Sensor s1(tstp, &door_1, 0, 0, 0);
    TSTP::Sensor s2(tstp, &id_1, 0, 0, 0);
    TSTP::Sensor s3(tstp, &id_2, 0, 0, 0);
    TSTP::Sensor s4(tstp, &id_3, 0, 0, 0);
    TSTP::Sensor s5(tstp, &id_4, 0, 0, 0);

    wie[0] = new Wiegand(1, relay[1], new GPIO('a',4,GPIO::INPUT), new GPIO('a',6,GPIO::INPUT), &s2);
    wie[1] = new Wiegand(2, relay[2], new GPIO('d',2,GPIO::INPUT), new GPIO('d',4,GPIO::INPUT), &s3);
    wie[2] = new Wiegand(3, relay[3], new GPIO('c',5,GPIO::INPUT), new GPIO('c',6,GPIO::INPUT), &s4);
    wie[3] = new Wiegand(4, relay[4], new GPIO('b',1,GPIO::INPUT), new GPIO('b',3,GPIO::INPUT), &s5);

    input[0] = new GPIO('a',2,GPIO::INPUT);
    input[1] = new GPIO('a',3,GPIO::INPUT);
    input[2] = new GPIO('a',5,GPIO::INPUT);
    input[3] = new GPIO('a',7,GPIO::INPUT);
    input[4] = new GPIO('c',0,GPIO::INPUT);
    input[5] = new GPIO('c',1,GPIO::INPUT);
    input[6] = new GPIO('c',4,GPIO::INPUT);

    //for (int j=0; j<7;j++)
    //    input[j]->clear();

    Handler_Input1 handlerinput1;
    handlerinput1.pin = input[1];

    input[0]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput0);
    input[1]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput1);
    input[2]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput2);
    input[3]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput3);
    input[4]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput4);
    input[5]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput5);
    input[6]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput6);

    while (1) {
    }
}
