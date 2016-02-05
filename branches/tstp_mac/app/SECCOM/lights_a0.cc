#include <modbus_ascii.h>
#include <nic.h>
#include <gpio.h>
#include <utility/ostream.h>
#include <machine/cortex_m/emote3_power_meter.h>
#include <machine/cortex_m/emote3_pwm.h>

using namespace EPOS;

typedef unsigned short sensor_data_type;

//These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','0'};
const unsigned char MODBUS_ID = 0xA0;
const unsigned short PROTOCOL_ID = 83;

const unsigned int PWM_FREQUENCY = 10000; // 10 KHz

OStream cout;
GPIO * led, * coil;
eMote3_PWM * pwm;
bool led_state = false;
bool coil_state = false;

Power_Meter * pm;

sensor_data_type sense_power()
{
    auto ret = pm->average();
    return ret;
}

class Receiver : public NIC::Observer, public Modbus_ASCII::Modbus_ASCII_Feeder
{
public:
    typedef NIC::Buffer Buffer;
    typedef NIC::Frame Frame;
    typedef NIC::Observed Observed;

    Receiver(NIC * nic) : _nic(nic)
    { _nic->attach(this, PROTOCOL_ID); }   

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
    Frame * f = b->frame();
    char * _msg = f->data<char>();
    cout << "Received: " << b->size() << endl;
    for(int i=0; i<b->size(); i++)
        cout << _msg[i];
    cout << endl;
    Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg, b->size());
    _nic->free(b);
}

void Sender::send(const char * c, int len)
{
    memcpy(_msg, c, len);
    cout << "Sending: " << len << endl;
    for(int i=0; i<len; i++)
        cout << c[i];
    cout << endl;
    cout << _nic->send(_nic->broadcast(), PROTOCOL_ID, (const char *)_msg, len) << endl;
}

class Modbus: public Modbus_ASCII
{
public:
    Modbus(Modbus_ASCII_Sender * sender, unsigned char addr):
            Modbus_ASCII(sender, addr) { }

    void handle_command(unsigned char cmd, unsigned char * data, int data_len)
    {
        cout << "received command: " << hex << (int)cmd;
        for (int i = 0; i < data_len; ++i)
            cout << " " << (int)data[i];
        cout << dec << endl;
        unsigned short starting_address, quantity_of_registers;
        unsigned char coil_response;
        unsigned short register_response;
        unsigned short output_address;
        unsigned short output_value;
        short value;
        sensor_data_type response[2];
        int idx = 0;
        switch(cmd)
        {
            case READ_COILS:
                starting_address = (((unsigned short)data[0]) << 8) | data[1];
                coil_response = coil_state;
                coil_response >>= starting_address;
                send(myAddress(), READ_COILS, reinterpret_cast<unsigned char *>(&coil_response), sizeof (unsigned char));
                break;
            case READ_HOLDING_REGISTER:
                memset(response, 0, 2*sizeof(sensor_data_type));
                starting_address = (((unsigned short)data[0]) << 8) | data[1];
                quantity_of_registers = (((unsigned short)data[2]) << 8) | data[3];
                if(quantity_of_registers > 2)
                    break;
                switch(starting_address)
                {
                    // There are intentionally no breaks
                    case 0:
                        if(idx < quantity_of_registers)
                            response[idx++] = htons(sense_power());
                    default:
                    case 1:
                        if(idx < quantity_of_registers)
                            response[idx++] = htons(pwm->duty_cycle());
                }
                send(myAddress(), READ_HOLDING_REGISTER, reinterpret_cast<unsigned char *>(response), quantity_of_registers * sizeof(response[0]));
                break;
            case WRITE_SINGLE_REGISTER:
                starting_address = (((unsigned short)data[0]) << 8) | data[1];
                if(starting_address != 1)
                    break;
                ack();
                value = (((short)data[2]) << 8) | data[3];
                pwm->set(PWM_FREQUENCY, value);
                break;
            case WRITE_SINGLE_COIL:
                output_address = (((unsigned short)data[0]) << 8) | data[1];
                output_value = (((unsigned short)data[2]) << 8) | data[3];
                ack();
                if(output_address == 0)
                    coil_state = output_value;
                else if(output_address == 9)
                    Machine::reboot();
                led_state = output_value;

                coil->set(coil_state);
                led->set(led_state);
                break;
            default:
                break;
        }
    }
};

int main()
{
    cout << "LISHA Smart Room Lights actuator / Power meter" << endl;
    cout << "ID: " << hex << MODBUS_ID << endl;
    cout << "Pins" << endl
         << "    LED: PC3" << endl
         << "    Coil: PD3" << endl
         << "    PWM: PD2" << endl
         << "    Power meter: PA7 and PA5" << endl;

    pm = new Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC5, ADC::GND);
    led = new GPIO('c',3, GPIO::OUTPUT);
    coil = new GPIO('d',3, GPIO::OUTPUT);

    led->set(led_state);
    coil->set(coil_state);

    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;

    pwm = new eMote3_PWM(1, PWM_FREQUENCY, 50, 'd', 2);

    Receiver * receiver = new Receiver(nic);
    Sender * sender = new Sender(nic);
    Modbus * modbus = new Modbus(sender, MODBUS_ID);
    receiver->registerModbus(modbus);

    while(true);

    return 0;
}
