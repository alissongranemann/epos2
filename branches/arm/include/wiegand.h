#ifndef __wiegand_h_
#define __wiegand_h_

#include <system/config.h>
#include <gpio.h>
#include <tstp.h>
#include <utility/string.h>
#include <utility/handler.h>
#include <timer.h>

__BEGIN_SYS

class Wiegand 
{
public:
    typedef TSTP::User_Unit<1001, 1> Door_State_1;
    typedef TSTP::User_Unit<1002, 1> Door_State_2;
    typedef TSTP::User_Unit<1003, 1> Door_State_3;
    typedef TSTP::User_Unit<1004, 1> Door_State_4;

    typedef TSTP::User_Unit<2001, 3> RFID_1;
    typedef TSTP::User_Unit<2002, 3> RFID_2;
    typedef TSTP::User_Unit<2003, 3> RFID_3;
    typedef TSTP::User_Unit<2004, 3> RFID_4;

    class ID_Code_Msg {
        public:
            unsigned char facility;
            unsigned short serial;
    }__attribute__((packed));

    static const unsigned int MAX_MSG_SIZE = 15;
    static const unsigned int PROTOCOL_ID = 15;

    static const unsigned int BRIDGE_ADDRESS = 12;
    static const unsigned int WIEGAND_ADDRESS = 13;
    
    Wiegand(unsigned int num, GPIO * relay, GPIO * input0, GPIO * input1) : 
        _unit(num), _package_bit_count(0), _parity(0), _package_value(0), _first_half(false), _relay(relay), _input0(input0), _input1(input1), _input0_handler(&handle_input<0>, this), _input1_handler(&handle_input<1>, this) { 
            //_input0->clear();
            //_input1->clear();
            _input0->enable_interrupt(GPIO::FALLING_EDGE, &_input0_handler);
            _input1->enable_interrupt(GPIO::RISING_EDGE, &_input1_handler);
        }

    char message_mount(char device, unsigned int element, char cmd, char* wiemsg);

    void check_permission(unsigned int lastmsg) {
        if (lastmsg == 59220) {
            set_doorstate();
        } else if (lastmsg == 33060) {
            set_doorstate();
        } else if (lastmsg == 50020) {
            set_doorstate();
        }
    }

    void process() {
        if(_package_bit_count==13) {
            unsigned int data;
            unsigned int facility;
            unsigned int serial;
            _first_half = !_first_half;
            if(_first_half){
                _parity -= (_package_value & 0x1000) >> 12;
                data = _package_value & 0x0FFF;
                facility = (_package_value & 0x0FF0) >> 4;
                serial = (_package_value & 0x0F) << 12;
                // cout << "[0..13]";
                if ((_parity & 0x1) == ((_package_value >> 12) & 0x1)){
                } else {
                    //   cout << ":PERR"; //_parity error on the first half
                }
                // cout << ":pack="<<_package_value<<",facility="<<facility<<endl;
            } else {
                _parity -= (_package_value & 0x1);
                data=(_package_value & 0x01FFE) >> 1;
                serial |= data;
                //cout<<"[14..25]";
                if ((_parity & 0x1) == (_package_value & 0x1)) {
                    //NIC nic;
                    //for(int i = 10; i-- and (nic.send(BRIDGE_ADDRESS, Wiegand::PROTOCOL_ID, ":PERR\n", sizeof(":PERR\n")) < sizeof(":PERR\n")););

                    //cout<<":PERR"; //_parity error on the second half
                }

                //  cout<<":pack="<<_package_value<<",serial="<<serial<<endl;

                char msg[30];
                char facilitystr[3];
                char serialstr[5];

                itoa(facility, facilitystr);
                itoa(serial, serialstr);

                strcpy (msg, reinterpret_cast<const char*>(facilitystr));
                strcat (msg,":");
                strcat (msg, reinterpret_cast<const char*>(serialstr));

                check_permission(serial);

                //id = (facility << 16) + serial;
                //TSTP::get_by_nic(0)->event(id);
                //message_mount('W', _unit, 'R', msg);

                //cout << ":W" <<  facility << ":" << serial << endl; //this cout it's the only one that is necessary to ScadaBR, besides of the Error cout. It uses the following format:
                // :WFacilityCodeUserCode.
            }

            _package_bit_count=0; //resets
            _parity=0;
            _package_value=0;

        }
    }

    template<bool BIT>
    void receive_bit() {
        _package_value = (_package_value << 1) + BIT;
        _parity += BIT;
        _package_bit_count++;
        process();
    }
    
    void set_doorstate() {
        GPIO led('c',3, GPIO::OUTPUT);  
        _relay->set(true);
        led.set(true);
        User_Timer_3::delay(500000);
        _relay->set(false);
        led.set(false);
    }
    
    static RFID_1 id_1;
    static RFID_2 id_2;
    static RFID_3 id_3;
    static RFID_4 id_4;
    static Door_State_1 ds1;
    static Door_State_2 ds2;
    static Door_State_3 ds3;
    static Door_State_4 ds4;

private:
    template<unsigned int N>
    static void handle_input(Wiegand * w) { w->receive_bit<N>(); }

    unsigned int _unit;
    unsigned int _package_bit_count;
    unsigned int _parity;
    unsigned int _package_value;
    
    bool _first_half;

    GPIO * _relay;
    GPIO * _input0;
    GPIO * _input1;
    Functor_Handler<Wiegand> _input0_handler;
    Functor_Handler<Wiegand> _input1_handler;
};

char Wiegand::message_mount(char device, unsigned int element, char cmd, char* wiemsg){
    /*
    char msg[Wiegand::MAX_MSG_SIZE];
    memset(msg, 0, Wiegand::MAX_MSG_SIZE);
    msg[0] = ':';
    msg[1] = device;
    msg[2] = '0' + element;
    msg[3] = cmd;
    unsigned int idx = 4;
    while((*wiemsg) and (idx < Wiegand::MAX_MSG_SIZE)) {
        msg[idx++] = *wiemsg++;
    }

    if(idx < Wiegand::MAX_MSG_SIZE) {
        msg[idx++] = '\n';
        NIC nic;
        for(int i = 50; i-- and (nic.send(BRIDGE_ADDRESS, Wiegand::PROTOCOL_ID, msg, idx) < idx););
    }
    */

    //function that mounts the messages on the following format ":DCV", where:
    //cout << ":" << device << element << cmd << wiemsg << endl;
}


__END_SYS

#endif
