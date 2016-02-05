#include <modbus_ascii.h>
#include <alarm.h>
#include <chronometer.h>
#include <secure_nic.h>
#include <utility/string.h>
#include <utility/key_database.h>
#include <nic.h>
#include <gpio.h>
#include <aes.h>
#include <uart.h>
#include <adc.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;
UART uart;

GPIO * led, * coil0;
bool led_state = false;

TSTP::Data sense_presence()
{
    return coil0->read() ? 1 : 0;
}

// These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','5'};
const unsigned char modbus_id = 0xA5;

class Receiver : public NIC::Observer, public Modbus_ASCII::Modbus_ASCII_Feeder
{
public:
    typedef NIC::Buffer Buffer;
    typedef NIC::Frame Frame;
    typedef NIC::Observed Observed;

    Receiver(NIC * nic) : _nic(nic)
    { _nic->attach(this, Traits<Modbus_ASCII>::PROTOCOL_ID); }   

    void update(Observed * o, NIC::Protocol p, Buffer * b);
private:
    NIC * _nic;
};

class Sender : public Modbus_ASCII::Modbus_ASCII_Sender
{
public:
	Sender(NIC * nic) : _nic(nic) {}
	virtual ~Sender() {}

	void send(const char * c, int len);

private:
    unsigned char _msg[Modbus_ASCII::MSG_LEN];
    NIC * _nic;
};

void Receiver::update(Observed * o, NIC::Protocol p, Buffer * b)
{
    int i=0;
    Frame * f = b->frame();
    char * _msg = f->data<char>();
    Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg, b->size());
    _nic->free(b);
}

void Sender::send(const char * c, int len)
{
    memcpy(_msg, c, len);
    cout << "Sending: ";
    for(int i=0; i<len; i++)
        cout << c[i];
    cout << endl;
    cout << _nic->send(_nic->broadcast(), Traits<Modbus_ASCII>::PROTOCOL_ID, (const char *)_msg, len) << endl;
}

class User_Modbus : public Modbus_ASCII
{
public:
	User_Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr) { }

	void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		cout << "received command: " << hex << (int)cmd;
		for (int i = 0; i < data_len; ++i)
			cout << " " << (int)data[i];
		cout << dec << endl;
        unsigned short starting_address, quantity_of_registers;
        unsigned char coil_response;
        unsigned short register_response;
        unsigned short sensor_data;
        unsigned short output_address;
        unsigned short output_value;
		switch(cmd)
		{
            case READ_COILS:
                starting_address = (((unsigned short)data[0]) << 8) | data[1];
                //quantity_of_registers = (((unsigned short)data[2]) << 8) | data[3];
                coil_response = sense_presence();
                coil_response >>= starting_address;
                send(myAddress(), READ_COILS, &coil_response, 1);
                break;

            case WRITE_SINGLE_COIL:
                output_address = (((unsigned short)data[0]) << 8) | data[1];
                output_value = (((unsigned short)data[2]) << 8) | data[3];
                ack();
                if(output_address == 9)
					Machine::reboot();
                break;
            default:
                break;
		}
	}
private:
};


int main()
{
    cout << "Presence sensor" << endl;
    cout << "ID: " << hex << modbus_id << endl;

    led = new GPIO('c',3, GPIO::OUTPUT);
    coil0 = new GPIO('b',0, GPIO::INPUT);

    led->set(false);

    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;

    Receiver * receiver = new Receiver(nic);
    Sender * sender = new Sender(nic);
    User_Modbus * modbus = new User_Modbus(sender, modbus_id);
    receiver->registerModbus(modbus);

    TSTP::Unit presence_unit(TSTP::Unit::PRESENCE);
    TSTP::Sensor s(presence_unit, &sense_presence, 1, 500000);

    while(true) {
        led->set(sense_presence());
    }

    return 0;
}
