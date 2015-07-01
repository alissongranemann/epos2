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
#include <machine/cortex_m/emote3.h>

using namespace EPOS;

OStream cout;
UART uart;

// ======= Edit these =======
const auto ADC_PIN = ADC::SINGLE4;
typedef int sensor_data_type;
sensor_data_type sensor_data;
sensor_data_type convert_from_adc(int adc_measurement)
{
    return adc_measurement;
}
sensor_data_type sense()
{
    auto adc_measurement = ADC::get(ADC_PIN);
    return convert_from_adc(adc_measurement);
}

// These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'0','0'};
const unsigned char modbus_id = 0x00;
// ==========================

void Receiver::update(Observed * o, Secure_NIC::Protocol p, Buffer * b)
{
    int i=0;
    Frame * f = b->frame();
    char * _msg = f->data<char>();
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
                send(myAddress(), READ_HOLDING_REGISTER, 0, reinterpret_cast<unsigned char *>(&sensor_data), sizeof(sensor_data_type));
                break;
            default:                
                break;
		}
	}
private:
};


int main()
{
    cout << "Home ADC Sensor" << endl;
    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;
    Secure_NIC * s = new Secure_NIC(false, new AES(), new Poly1305(new AES()), nic);       

    Thing<Modbus> sensor(modbus_id, s);

    while(true)
    { 
        sensor_data = sense();
        cout << sensor_data << endl;
    }

    return 0;
}
