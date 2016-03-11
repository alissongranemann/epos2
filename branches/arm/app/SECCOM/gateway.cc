#include <modbus_ascii.h>
#include <utility/string.h>
#include <nic.h>
#include <uart.h>

using namespace EPOS;

OStream cout;
UART uart;

bool debugging_mode = false;
const char debug_mode_start[] = "DEBUG";
const char debug_mode_end[] = "/DEBUG";
const unsigned short PROTOCOL_ID = 83;

const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'F','F'};

class Receiver : public NIC::Observer
{
public:
    typedef NIC::Buffer Buffer;
    typedef NIC::Frame Frame;
    typedef NIC::Observed Observed;

    Receiver(NIC * nic) : _nic(nic)
    {
        _nic->attach(this, PROTOCOL_ID);
    }   

    void update(Observed * o, NIC::Protocol p, Buffer * b)
    {
        int i=0;
        //             led_value = !led_value;
        //             led->set(led_value);
        Frame * f = b->frame();
        char * _msg = f->data<char>();

        if(!Modbus_ASCII::check_format(_msg, b->size()))
        {
            _nic->free(b);
            return;
        }

        while((_msg[i] != '\r') && (_msg[i+1] != '\n')) 
        {
            uart.put(_msg[i++]);
        }
        uart.put('\r');
        uart.put('\n');

//         cout << endl << "=====================" << endl;
//         cout << "Received " << b->size() << " bytes of payload from " << f->src() << " :" << endl;
//         for(int i=0; i<b->size(); i++)
//             cout << _msg[i];
//         cout << endl << "=====================" << endl;
        _nic->free(b);
        //            _nic->stop_listening();
    }

private:
    NIC * _nic;
};

class Sender
{
public:
	Sender(NIC * nic) : _nic(nic) {}
	virtual ~Sender() {}

	virtual int run()
	{
		while(true)
		{
            int i = 0;
			_msg[i++] = uart.get();
			_msg[i++] = uart.get();
	        while(!(_msg[i-2] == '\r' && _msg[i-1] == '\n'))
	        	_msg[i++] = uart.get();
            auto len = i;

            // Account for \0
            if(((len-1) == sizeof(debug_mode_start)) && (!strncmp(debug_mode_start, _msg, sizeof(debug_mode_start)-1)))
            {
                debugging_mode = true;
                cout << "Entered debugging mode" << endl;
            }
            else if(((len-1) == sizeof(debug_mode_end)) && (!strncmp(debug_mode_end, _msg, sizeof(debug_mode_end)-1)))
            {
                debugging_mode = false;
                cout << "Exited debugging mode" << endl;
            }
            else
            {
                memset(_msg+i, 0x00, Modbus_ASCII::MSG_LEN-i);
                char id[3];
                id[0] = _msg[1];
                id[1] = _msg[2];
                id[2] = 0;

                if(debugging_mode)
                {
                    unsigned char lrc = 0;
                    for(int i=1; i<len-2; i+=2)
                        lrc += Modbus_ASCII::decode(_msg[i], _msg[i+1]);

                    lrc = ((lrc ^ 0xff) + 1) & 0xff;
                    Modbus_ASCII::encode(lrc, &_msg[len-2], &_msg[len-1]);

                    _msg[len] = '\r';
                    _msg[len+1] = '\n';
                    len+=2;

                    cout << "Sending: ";
                    for(int i=0;!(_msg[i-2] == '\r' && _msg[i-1] == '\n');i++)
                        cout << (char)_msg[i];
                    cout << endl;
                }

                _nic->send(_nic->broadcast(), PROTOCOL_ID, (const char *)_msg, len);
            }
		}

		return 0;
	}

private:
    char _msg[Modbus_ASCII::MSG_LEN];
    NIC * _nic;
};

int main()
{
    cout << "Home Gateway" << endl;
    NIC * nic = new NIC();
    nic->address(NIC::Address::RANDOM);

    Receiver receiver(nic);
    Sender sender(nic);

    sender.run();

    return 0;
}
