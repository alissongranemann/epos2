#include <utility/ostream.h>
#include <nic.h>

using namespace EPOS;

OStream cout;

void sender()
{
    
    const char data[] = "Hello, World!";
//    const char data[] = {2, 0, 0}; // Valid ACK frame
//    const char data[] = {0, 192, 0, 0xff, 0xff, 0xee, 0xee,0xee,0xee,0xee,0xee,0xee,0xee, 'H', 'i'}; // Valid BEACON frame
//      const char data[] = {0, 128, 0, 0xff, 0xff, 0xff, 0xff, 'H', 'i'}; // Valid BEACON frame
//      const char data[] = {1, 128, 0, 0xff, 0xff, 0x10, 0x10, 0xde, 0xde, 'H', 'i', '\0'}; // Valid DATA frame
//      const char data[] = {1, 136, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x10, 0xde, 0xde, 'H', 'i', '\0'}; // Valid DATA frame
//      const char data[] = {65, 136, 0, 0xff, 0xff, 0xff, 0xff, 0xff,0xff,0x10, 0x10, 'H', 'i', '\0'}; // Valid DATA frame

    NIC * nic = new NIC();
    while(1)
    {
        cout << "Sending..." << endl;
        nic->send(nic->broadcast(), 0x1010, data, sizeof data);
        cout << "Sent" << endl;
    }
}

class Receiver : public IEEE802_15_4::Observer
{
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
            Frame * f = b->frame();
            char * d = f->data<char>();
            cout << "Data received from " << f->src() << " :" << endl;
            for(int i=0; i<b->size(); i++)
                cout << d[i];
            cout << endl << "==============" << endl;
            _nic->free(b);
//            _nic->stop_listening();
        }
    }

private:
    Protocol _prot;
    NIC * _nic;
};

int main()
{
    kout << "Hello main" << endl;
    NIC * nic = new NIC();
    Receiver * r = new Receiver(0x1010, nic);
//    nic->listen();
    kout << "Hello again" << endl;
//   sender();
    while(1);


    return 0;
}
