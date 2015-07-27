#include <modbus_ascii.h>
#include <alarm.h>
#include <chronometer.h>
#include <secure_nic.h>
#include <utility/string.h>
#include <utility/key_database.h>
#include <nic.h>
#include <aes.h>
#include <uart.h>
#include "home_thing.h"

__USING_SYS

OStream cout;
UART uart;

unsigned char status;
        
// These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','7'};
const unsigned char modbus_id = 0xA7;

void Receiver::update(Observed * o, Secure_NIC::Protocol p, Buffer * b)
{
    int i=0;
    Frame * f = b->frame();
    char * _msg = f->data<char>();

    //         cout << "Receiver::update: " << endl;
    //         uart.put(_msg[i++]);
    //         uart.put(_msg[i++]);
    //         while(i<b->size() && !(_msg[i-2] == '\r' && _msg[i-1] == '\n')) {
    //             uart.put(_msg[i++]);
    //         }
    //         cout << "size:" << i << endl;

    Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg);
    _nic->free(b);
}

void Sender::send(char * c, int len)
{
    memcpy(_msg, c, len);
    memset(_msg+len, 0x00, Modbus_ASCII::MSG_LEN-len);
    kout << "Sending" << endl;
    kout << _nic->send(_nic->gateway_address, (const char *)_msg, Modbus_ASCII::MSG_LEN) << endl;
}

class Modbus : public Modbus_ASCII
{
public:
	Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr) { }

	void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		kout << "received command: " << hex << (int)cmd;
		for (int i = 0; i < data_len; ++i)
			kout << (int)data[i];
		kout << dec;
		unsigned short coil, value;
		switch(cmd)
		{
            case READ_HOLDING_REGISTER:
                send(myAddress(), READ_HOLDING_REGISTER, &status, 1);
                break;
            default:
                break;
		}
	}
private:
};

int main()
{
    cout << "Home Presence Sensor" << endl;
    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;
    Secure_NIC * s = new Secure_NIC(false, new AES(), new Poly1305(new AES()), nic);       

    Thing<Modbus> presence_sensor(modbus_id, s);

    GPIO led('c',3, GPIO::OUTPUT);
    GPIO sensor('d',5, GPIO::INPUT);
    led.set(false);

    while(true)
    {
        status = sensor.read();
        led.set(status);
    }

    return 0;
}
