#include <utility/ostream.h>
#include <nic.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','0'};

void sender()
{
    // These are frames built by hand while implementing IEEE802.15.4
    // These headers are correctly handled by the radio and IEEE802.15.4
    // const char data[] = {2, 0, 0}; // Valid ACK frame
    // const char data[] = {0, 192, 0, 0xff, 0xff, 0xee, 0xee,0xee,0xee,0xee,0xee,0xee,0xee, 'H', 'i'}; // Valid BEACON frame
    // const char data[] = {0, 128, 0, 0xff, 0xff, 0xff, 0xff, 'H', 'i'}; // Valid BEACON frame
    // const char data[] = {1, 128, 0, 0xff, 0xff, 0x10, 0x10, 0xde, 0xde, 'H', 'i', '\0'}; // Valid DATA frame
    // const char data[] = {1, 136, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x10, 0xde, 0xde, 'H', 'i', '\0'}; // Valid DATA frame
    // const char data[] = {65, 136, 0, 0xff, 0xff, 0xff, 0xff, 0xff,0xff,0x10, 0x10, 'H', 'i', '\0'}; // Valid DATA frame

    char data[] = "0 Hello, World!";
    NIC * nic = new NIC();
    const unsigned int delay_time = 3000000;
    cout << "Hello, I am the sender." << endl;
    cout << "I will send a message every " << delay_time << " microseconds." << endl;
    while(1)
    {
        cout << "Sending message: " << data << endl;
        nic->send(nic->broadcast(), 0x1010, data, sizeof data);
        cout << "Sent" << endl;
        data[0] = ((data[0] - '0' + 1) % 10) + '0';
        Alarm::delay(delay_time);
    }
}

bool led_value;
GPIO * led;

class Receiver : public IEEE802_15_4::Observer
{
    typedef short data_type;

public:
    typedef IEEE802_15_4::Protocol Protocol;
    typedef IEEE802_15_4::Buffer Buffer;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Observed Observed;

    Receiver(const Protocol & p, NIC * nic) : _prot(p), _nic(nic)
    {        
        _nic->attach(this, _prot);        
    }   

    void update(Observed * o, Protocol p, Buffer * b)
    {
        if(p == _prot)
        {
            led_value = !led_value;
            led->set(led_value);
            Frame * f = b->frame();
            auto d = f->data<data_type>();
            cout << endl << "=====================" << endl;
            cout << "Received " << b->size() << " bytes of payload from " << f->src() << " :" << endl;
            for(int i=0; i<b->size()/sizeof(data_type); i++)
                cout << d[i] << " ";
            cout << endl << "=====================" << endl;
            _nic->free(b);
//            _nic->stop_listening();
        }
    }

private:
    Protocol _prot;
    NIC * _nic;
};

void receiver()
{
    led = new GPIO('c',3, GPIO::OUTPUT);
    led_value = true;
    led->set(led_value);
    cout << "Hello, I am the receiver." << endl; 
    cout << "I will attach myself to the NIC and print every message I get." << endl;
    NIC * nic = new NIC();
    Receiver * r = new Receiver(0x1010, nic);
    while(1);
}

int main()
{
    cout << "Hello main" << endl;
//     sender();
   receiver();

    return 0;
}
