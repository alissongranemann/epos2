#include <modbus_ascii.h>
#include <utility/string.h>
#include <nic.h>
#include <usb.h>
#include <gpio.h>

using namespace EPOS;

USB io;

class Receiver : public NIC::Observer
{
public:
    typedef NIC::Buffer Buffer;
    typedef NIC::Frame Frame;
    typedef NIC::Observed Observed;

    Receiver(NIC * nic, GPIO * led) : _nic(nic), _led(led)
    {
        _nic->attach(this, Traits<Modbus_ASCII>::PROTOCOL_ID);
        _led_value = true;
        _led->set(_led_value);
    }

    void update(Observed * o, NIC::Protocol p, Buffer * b)
    {
        _led_value = !_led_value;
        _led->set(_led_value);
        Frame * f = b->frame();
        char * msg = f->data<char>();
        unsigned int size = b->size();

        if(Modbus_ASCII::check_format(msg, size)) {
            for(unsigned int i = 0; (i < size) and (msg[i] != '\r') and (msg[i+1] != '\n'); i++ ) {
                io.put(msg[i]);
            }
            io.put('\r');
            io.put('\n');
        }

        _nic->free(b);
    }

private:
    NIC * _nic;
    GPIO * _led;
    bool _led_value;
};

class Sender
{
public:
	Sender(NIC * nic) : _nic(nic) {}
	virtual ~Sender() {}

	virtual int run()
	{
		while(true) {
            unsigned int i = 0;
            _msg[i++] = io.get();
            _msg[i++] = io.get();
            while(!(_msg[i-2] == '\r' and _msg[i-1] == '\n')) {
                _msg[i++] = io.get();
            }
            auto len = i;

            memset(_msg + len, 0x00, Modbus_ASCII::MSG_LEN - len);

            _nic->send(_nic->broadcast(), Traits<Modbus_ASCII>::PROTOCOL_ID, (const char *)_msg, len);
        }

		return 0;
	}

private:
    char _msg[Traits<Modbus_ASCII>::MSG_LEN];
    NIC * _nic;    
};

int main()
{
    NIC nic;
    GPIO led('c', 3, GPIO::OUTPUT);

    nic.address(NIC::Address::RANDOM);

    Receiver receiver(&nic, &led);
    Sender sender(&nic);

    sender.run();

    while(true);

    return 0;
}
