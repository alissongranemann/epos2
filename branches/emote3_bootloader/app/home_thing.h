#include <secure_nic.h>

__USING_SYS
const unsigned char Diffie_Hellman::default_base_point_x[] = 
{       
    0x86, 0x5B, 0x2C, 0xA5,
    0x7C, 0x60, 0x28, 0x0C,
    0x2D, 0x9B, 0x89, 0x8B,
    0x52, 0xF7, 0x1F, 0x16
};
const unsigned char Diffie_Hellman::default_base_point_y[] =
{
    0x83, 0x7A, 0xED, 0xDD,
    0x92, 0xA2, 0x2D, 0xC0,
    0x13, 0xEB, 0xAF, 0x5B,
    0x39, 0xC8, 0x5A, 0xCF
};
const unsigned char Bignum::default_mod[] = 
{
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFD, 0xFF, 0xFF, 0xFF
};
const unsigned char Bignum::default_barrett_u[] = 
{
    17, 0, 0, 0, 
    8, 0, 0, 0, 
    4, 0, 0, 0, 
    2, 0, 0, 0, 
    1, 0, 0, 0
};

class Receiver : public Secure_NIC::Observer, public Modbus_ASCII::Modbus_ASCII_Feeder
{
public:
    typedef Secure_NIC::Buffer Buffer;
    typedef Secure_NIC::Frame Frame;
    typedef Secure_NIC::Observed Observed;

    Receiver(Secure_NIC * nic) : _nic(nic)
    { _nic->attach(this, Traits<Secure_NIC>::PROTOCOL_ID); }   

    void update(Observed * o, Secure_NIC::Protocol p, Buffer * b);
private:
    Secure_NIC * _nic;
};

class Sender : public Modbus_ASCII::Modbus_ASCII_Sender
{
public:
	Sender(Secure_NIC * nic) : _nic(nic) {}
	virtual ~Sender() {}

	void send(const char * c, int len);

private:
    unsigned char _msg[Modbus_ASCII::MSG_LEN];
    Secure_NIC * _nic;
};

void busy_wait(int limit = 0x3fffff) { for(volatile int i=0; i<limit; i++); }

template<typename Modbus>
class Thing
{
    public:
    Thing(int modbus_id, Secure_NIC * s)
    {
        OStream cout;
        _snic = s;
        GPIO led('c',3, GPIO::OUTPUT);

        for(int i=0;i<10;i++)
        {
            led.set(true);
            busy_wait(0xffff);
            led.set(false);
            busy_wait(0xffff);
        }

        _snic->set_id(Traits<Build>::ID);

        do
        {
            cout << "Delaying..." << endl;
            busy_wait();
            cout << "Up!" << endl;
            if(!_snic->authenticated())
            {
                cout << "Trying to negotiate key\n";
                // Try to authenticate
                _snic->send_key_request(_snic->broadcast());
            }
        }
        while(!_snic->authenticated());

        cout << "Key set!" << endl;

        for(int i=0;i<10;i++)
        {
            led.set(true);
            busy_wait(0xffff);
            led.set(false);
            busy_wait(0xffff);
        }

        Receiver * receiver = new Receiver(s);
        Sender * sender = new Sender(s);
        Modbus * modbus = new Modbus(sender, modbus_id);
        receiver->registerModbus(modbus);
    }

private:
    Secure_NIC * _snic;
};
