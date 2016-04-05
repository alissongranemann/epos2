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
    
    Wiegand(unsigned int num, GPIO * relay, GPIO * input0, GPIO * input1, TSTP::Sensor * sensor) : 
        _unit(num), _package_bit_count(0), _parity(0), _package_value(0), _data(0), _facility(0), _serial(0), _first_half(false), _relay(relay), _input0(input0), _input1(input1), _input0_handler(&handle_input<0>, this), _input1_handler(&handle_input<1>, this), _sensor(sensor) { 
            //_input0->clear();
            //_input1->clear();
            _input0->enable_interrupt(GPIO::FALLING_EDGE, &_input0_handler);
            _input1->enable_interrupt(GPIO::RISING_EDGE, &_input1_handler);
        }

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
            _first_half = !_first_half;
            if(_first_half){
                _parity -= (_package_value & 0x1000) >> 12;
                _data = _package_value & 0x0FFF;
                _facility = (_package_value & 0x0FF0) >> 4;
                _serial = (_package_value & 0x0F) << 12;
                // cout << "[0..13]";
                if ((_parity & 0x1) == ((_package_value >> 12) & 0x1)){
                } else {
                    //   cout << ":PERR"; //_parity error on the first half
                }
                // cout << ":pack="<<_package_value<<",facility="<<facility<<endl;
            } else {
                _parity -= (_package_value & 0x1);
                _data=(_package_value & 0x01FFE) >> 1;
                _serial |= _data;
                //cout<<"[14..25]";
                if ((_parity & 0x1) == (_package_value & 0x1)) {
                    //NIC nic;
                    //for(int i = 10; i-- and (nic.send(BRIDGE_ADDRESS, Wiegand::PROTOCOL_ID, ":PERR\n", sizeof(":PERR\n")) < sizeof(":PERR\n")););

                    //cout<<":PERR"; //_parity error on the second half
                }

                //  cout<<":pack="<<_package_value<<",serial="<<serial<<endl;

                //char msg[30];
                //char facilitystr[3];
                //char serialstr[5];

                //itoa(_facility, facilitystr);
                //itoa(_serial, serialstr);

                //strcpy(msg, reinterpret_cast<const char*>(facilitystr));
                //strcat(msg,":");
                //strcat(msg, reinterpret_cast<const char*>(serialstr));

                check_permission(_serial);

                auto id = _sensor->data<ID_Code_Msg>();
                id->facility = _facility;
                id->serial = _serial;
                _sensor->tstp()->event(*_sensor);
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
    
private:
    template<unsigned int N>
    static void handle_input(Wiegand * w) { w->receive_bit<N>(); }

    unsigned int _unit;
    unsigned int _package_bit_count;
    unsigned int _parity;
    unsigned int _package_value;
    unsigned int _data;
    unsigned int _facility;
    unsigned int _serial;
    
    bool _first_half;

    GPIO * _relay;
    GPIO * _input0;
    GPIO * _input1;
    Functor_Handler<Wiegand> _input0_handler;
    Functor_Handler<Wiegand> _input1_handler;
    TSTP::Sensor * _sensor;
};

__END_SYS

#endif
