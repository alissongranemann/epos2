#include <machine.h>
#include <alarm.h>
#include <gpio.h>
#include <utility/ostream.h>
#include <utility/string.h>
#include <adc.h>
#include <uart.h>
#include <wiegandradiotstp.h>
#include <machine/cortex_m/emote3_gptm.h>
#include <network.h>

using namespace EPOS;

TSTP * tstp;
RFID_1 Wiegand::id; // TODO: unit

//OStream cout;

//GPIO * led;

//GPIO * relay[7];
GPIO * input[7];

Wiegand * wie[4];

GPIO wiegand0d0('a',4,GPIO::INPUT);
GPIO wiegand0d1('a',6,GPIO::INPUT);
GPIO wiegand1d0('d',2,GPIO::INPUT);
GPIO wiegand1d1('d',4,GPIO::INPUT);
GPIO wiegand2d0('c',5,GPIO::INPUT);
GPIO wiegand2d1('c',6,GPIO::INPUT);
GPIO wiegand3d0('b',1,GPIO::INPUT);
GPIO wiegand3d1('b',3,GPIO::INPUT);

char lastmessage[6];

Door_State_1 door_1;
Door_State_2 door_2;
Door_State_3 door_3;
Door_State_4 door_4;

void handlerwiegand0d0(GPIO * pin) //handler for the zeroes from the wiegand0.
{
    wie[0]->receiveBit(0);
}

void handlerwiegand1d0(GPIO * pin) //handler for the zeroes from the wiegand1.
{
    wie[1]->receiveBit(0);
}

void handlerwiegand2d0(GPIO * pin) //handler for the zeroes from the wiegand2.
{
    led->set(true);
    wie[2]->receiveBit(0);
    led->set(false);
}

void handlerwiegand3d0(GPIO * pin) //handler for the zeroes from the wiegand3.
{
    wie[3]->receiveBit(0);
}

void handlerwiegand0d1(GPIO * pin)//handler for the ones from the wiegand0.
{
    wie[0]->receiveBit(1);
}

void handlerwiegand1d1(GPIO * pin) //handler for the ones from the wiegand1.
{
    wie[1]->receiveBit(1);
}

void handlerwiegand2d1(GPIO * pin) //handler for the ones from the wiegand2.
{
    wie[2]->receiveBit(1);
}

void handlerwiegand3d1(GPIO * pin) //handler for the ones from the wiegand3.
{
    wie[3]->receiveBit(1);
}

void handlerinput0(GPIO * pin) //handler for the input0
{
    relay[1]->set(true);
    led->set(true);
    eMote3_GPTM::delay(500000);
    relay[1]->set(false);
    led->set(false);
}

void handlerinput1(GPIO * pin) //handler for the input1
{
    led->set(true);
    bool val = pin->read();
    door_1 = val;
    tstp->event(door_1);
    eMote3_GPTM::delay(500000);
    led->set(false);
}

void handlerinput2(GPIO * pin) //handler for the input2
{
    relay[3]->set(true);
    //messagemount('R', 3, 'W', 1);
    eMote3_GPTM::delay(500000);
    relay[3]->set(false);
    //messagemount('R', 3, 'W', 0);
}

void handlerinput3(GPIO * pin) //handler for the input3
{
    relay[4]->set(true);
    //messagemount('R', 4, 'W', 1);
    eMote3_GPTM::delay(500000);
    relay[4]->set(false);
    //messagemount('R', 4, 'W', 0);
}

void handlerinput4(GPIO * pin) //handler for the input4
{
    relay[5]->set(true);
    //messagemount('R', 5, 'W', 1);
    eMote3_GPTM::delay(500000);
    relay[5]->set(false);
    //messagemount('R', 5, 'W', 0);
}

void handlerinput5(GPIO * pin) //handler for the input5
{
    relay[6]->set(true);
    //messagemount('R', 6, 'W', 1);
    eMote3_GPTM::delay(500000);
    relay[6]->set(false);
    //messagemount('R', 6, 'W', 0);
}


void handlerinput6(GPIO * pin) //handler for the input6
{
    relay[7]->set(true);
    //messagemount('R', '7', 'W', 1);
    eMote3_GPTM::delay(500000);
    relay[7]->set(false);
    //messagemount('R', '7', 'W', 0);
}

void create_interrupts()
{
    wiegand0d0.clear();
    wiegand0d1.clear();
    wiegand1d0.clear();
    wiegand1d1.clear();
    wiegand2d0.clear();
    wiegand2d1.clear();
    wiegand3d0.clear();
    wiegand3d1.clear();

    //Enabling interrupts
    wiegand0d0.enable_interrupt(GPIO::FALLING_EDGE, &handlerwiegand0d0);
    wiegand0d1.enable_interrupt(GPIO::RISING_EDGE, &handlerwiegand0d1);
    wiegand1d0.enable_interrupt(GPIO::FALLING_EDGE, &handlerwiegand1d0);
    wiegand1d1.enable_interrupt(GPIO::RISING_EDGE, &handlerwiegand1d1);
    wiegand2d0.enable_interrupt(GPIO::FALLING_EDGE, &handlerwiegand2d0);
    wiegand2d1.enable_interrupt(GPIO::RISING_EDGE, &handlerwiegand2d1);
    wiegand3d0.enable_interrupt(GPIO::FALLING_EDGE, &handlerwiegand3d0);
    wiegand3d1.enable_interrupt(GPIO::RISING_EDGE, &handlerwiegand3d1);

    input[0]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput0);
    input[1]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput1);
    input[2]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput2);
    input[3]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput3);
    input[4]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput4);
    input[5]->enable_interrupt(GPIO::BOTH_EDGES, &handlerinput5);
    input[6]->enable_interrupt(GPIO::FALLING_EDGE, &handlerinput6);
}

/*void uartbuilder (){ // Function that receives the messages from UART1 and save it in lastmessage.

    UART uart1{115200, 8, 0, 1, 0};
    //cout << "UART_builder" << endl;

    //cout << "Estou em UART Builder" << endl;


    auto c = '\0';
    int i = 0;
    char data[5];


    //cout << "UART has data" << endl;
    while (((c = uart1.get()) !='\n') and (i != 5))
    {
        lastmessage[i] = c;
        //cout << "lastmessage:" << lastmessage[i] << endl;
        i++;

    }
    data[i]='\0';
}*/

int main () {
    Network::init();
    tstp = TSTP::get_by_nic(0);
    if(!tstp) {
        while(1) {
            cout << "Waaaahhh" << endl;
        }
    }

    led = new GPIO('c',3, GPIO::OUTPUT);
    //begin at [1] because of khomp standards

    relay[1] = new GPIO('d',1,GPIO::OUTPUT);
    relay[2] = new GPIO('d',3,GPIO::OUTPUT);
    relay[3] = new GPIO('d',5,GPIO::OUTPUT);
    relay[4] = new GPIO('b',6,GPIO::OUTPUT);
    relay[5] = new GPIO('b',0,GPIO::OUTPUT);
    relay[6] = new GPIO('b',2,GPIO::OUTPUT);
    relay[7] = new GPIO('b',4,GPIO::OUTPUT);


    wie[0] = new Wiegand(1, relay [1]);
    wie[1] = new Wiegand(2, relay [2]);
    wie[2] = new Wiegand(3, relay [3]);
    wie[3] = new Wiegand(4, relay [4]);

    for (int i=0; i<7;i++)
        relay[i]->clear();

    input[0] = new GPIO('a',2,GPIO::INPUT);
    input[1] = new GPIO('a',3,GPIO::INPUT);
    input[2] = new GPIO('a',5,GPIO::INPUT);
    input[3] = new GPIO('a',7,GPIO::INPUT);
    input[4] = new GPIO('c',0,GPIO::INPUT);
    input[5] = new GPIO('c',1,GPIO::INPUT);
    input[6] = new GPIO('c',4,GPIO::INPUT);

    for (int j=0; j<7;j++)
        input[j]->clear();

    TSTP::Sensor s1(tstp, &door_1, 0, 0, 0);
    TSTP::Sensor s2(tstp, &wie[0]->id, 0, 0, 0);
    TSTP::Sensor s3(tstp, &wie[1]->id, 0, 0, 0);
    TSTP::Sensor s4(tstp, &wie[2]->id, 0, 0, 0);
    TSTP::Sensor s5(tstp, &wie[3]->id, 0, 0, 0);
    create_interrupts();

    while (1) {
    }
}
