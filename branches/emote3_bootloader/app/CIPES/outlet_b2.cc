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

const auto TENSION_PIN = ADC::SINGLE7;

// These two should be the same
const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'B','2'};
const unsigned char modbus_id = 0xB2;

class Power_Meter
{
private:
    static const int DEFAULT_FREQ = 2000;
    static const int N_S = 128;

    typedef ADC::Single Single;

public:
    Power_Meter(Single v_chan, Single i_chan, int i_bias = 2926):
        _v_chan(v_chan), _i_chan(i_chan), _i_bias(i_bias) {}

    ~Power_Meter() {}

    // Calculate the average electrical power of sinusoidal signals using the
    // following equation: (1/T)*(integral from j=0 to j=T)(v(j)*i(j)).
    int sample() {
        int j;
        int v[N_S], v_shift[N_S], i[N_S];
        long i_avg = 0, p_avg = 0, p[N_S];

        for(j = 0; j < N_S; j++) {
            // Sample V and I
            i[j] = ADC::get(_i_chan) - _i_bias;
            v[j] = ADC::get(_v_chan);

            // The measured voltage stored in v[] is sinusoidal signal being
            // rectified by a diode. v_shift[] is an approximation of the
            // blocked half signal based on the measured voltage and is used to
            // reconstruct the complete signal.
            v_shift[(j + DEFAULT_FREQ/(2*60))%N_S] = v[j];

            i_avg += i[j];

            eMote3_GPTM::delay(1000000/DEFAULT_FREQ);
        }

        i_avg = i_avg/N_S;

        for(j = 0; j < N_S; j++) {
            // Rebuild the complete voltage signal
            v[j] = v[j] - v_shift[j];
            // Remove DC bias from i[]
            i[j] = i[j] - i_avg;
            // Calculate instant power
            p[j] = v[j]*i[j];
        }

        // Remove glitch?
        for(j = 19; j < N_S; j++) {
            p_avg += p[j];
        }

        p_avg = p_avg/(N_S - 20);

        return p_avg;
    }

private:
    int _i_bias;
    Single _v_chan;
    Single _i_chan;
};

GPIO * switch_d2, * switch_d3;
Power_Meter * pm0, * pm1;

typedef unsigned short sensor_data_type;
sensor_data_type sense_power0()
{
    auto ret = pm0->sample();
    return ret;
}
sensor_data_type sense_power1()
{
    auto ret = pm1->sample();
    return ret;
}

void Receiver::update(Observed * o, Secure_NIC::Protocol p, Buffer * b)
{
    Frame * f = b->frame();
    char * _msg = f->data<char>();
    kout << "Received: " << b->size() << endl;
    for(int i=0; i<b->size(); i++)
        kout << _msg[i];
    kout << endl;
    Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg);
    _nic->free(b);
}

void Sender::send(const char * c, int len)
{
    memcpy(_msg, c, len);
//    kout << "Sending: " << len << endl;
//    for(int i=0; i<len; i++)
//        kout << c[i];
//    kout << endl;
    _nic->send(_nic->gateway_address, (const char *)_msg, len);
}

class Modbus : public Modbus_ASCII
{
public:
	Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr) { }

	void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
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
                            response[idx++] = htons(sense_power0());
                    default:
                    case 1:
                        if(idx < quantity_of_registers)
                            response[idx++] = htons(sense_power1());
                }
                send(myAddress(), READ_HOLDING_REGISTER, reinterpret_cast<unsigned char *>(response), quantity_of_registers * sizeof(response[0]));
                break;

            default:
                break;
		}
	}
private:
};

int main()
{
    cout << "CIPES Current sensor" << endl;
    cout << "ID: " << hex << modbus_id << endl;
    cout << "Pins:" << endl
         << "   power meters: PA5 and PA6" << endl
         << "   tension: PA7" << endl
         << "   switches: PD2 and PD3" << endl;

    pm0 = new Power_Meter(TENSION_PIN, ADC::SINGLE5);
    pm1 = new Power_Meter(TENSION_PIN, ADC::SINGLE6);
    switch_d2 = new GPIO('d',2, GPIO::OUTPUT);
    switch_d3 = new GPIO('d',3, GPIO::OUTPUT);
    switch_d2->set();
    switch_d3->set();

    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);
    cout << "Address: " << nic->address() << endl;
    Secure_NIC * s = new Secure_NIC(false, new AES(), new Poly1305(new AES()), nic);

    Thing<Modbus> sensor(modbus_id, s);

    while(true);

    return 0;
}
