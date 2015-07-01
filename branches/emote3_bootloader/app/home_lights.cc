#include <modbus_ascii.h>
#include <nic.h>
#include <aes.h>
#include <uart.h>
#include "home_thing.h"

__USING_SYS

OStream cout;
UART uart;

// These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','2'};
const unsigned char modbus_id = 0xA2;

void Receiver::update(Receiver::Observed * o, Secure_NIC::Protocol p, Receiver::Buffer * b)
{
    int i=0;
    Frame * f = b->frame();
    char * _msg = f->data<char>();

    cout << "Receiver::update: " << endl;
    uart.put(_msg[i++]);
    uart.put(_msg[i++]);
    while(i<b->size() && !(_msg[i-2] == '\r' && _msg[i-1] == '\n')) {
        uart.put(_msg[i++]);
    }
    cout << "size:" << i << endl;

    Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg);
    _nic->free(b);
}

void Sender::send(char * c, int len)
{
    memcpy(_msg, c, len);
    memset(_msg+len, 0x00, Modbus_ASCII::MSG_LEN-len);
    _nic->send(_nic->gateway_address, (const char *)_msg, Modbus_ASCII::MSG_LEN);
}

class Modbus : public Modbus_ASCII
{
public:
	Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr)
     { 
         _led = new GPIO('c',3, GPIO::OUTPUT);
         _gpio0 = new GPIO('b',0, GPIO::OUTPUT);
         _gpio1 = new GPIO('b',1, GPIO::OUTPUT);
         _led->set(false);
         _gpio0->set(false);
         _gpio1->set(false);
     }

	void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		kout << "received command: " << hex << (int)cmd << " ";
		for (int i = 0; i < data_len; ++i)
			kout << (int)data[i] << " ";
		kout << dec;
		unsigned short coil, value;
		switch(cmd)
		{
            case WRITE_SINGLE_COIL:
                coil = (((unsigned short)data[0]) << 8) | data[1];
                value = (((unsigned short)data[2]) << 8) | data[3];
                write(coil, value);
                break;
		}
	}

	void write(unsigned short output, unsigned short value)
	{
		kout << "write: " << output << " , " << value << endl;
        if(output == 0)
        {
            _gpio0->set((value != 0));
            _led->set((value != 0));
        }
        else if(output == 1)
        {
            _gpio1->set((value != 0));
            _led->set((value != 0));
        }
	}
private:
    GPIO * _gpio0, * _gpio1, * _led;
};

int main()
{
    cout << "Home Lights" << endl;
    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;
    Secure_NIC * s = new Secure_NIC(false, new AES(), new Poly1305(new AES()), nic);       

    Thing<Modbus> lights(modbus_id, s);

    while(true);

    return 0;
}
