#include <usb.h>
#include <nic.h>

using namespace EPOS;

const NIC::Protocol BOOTLOADER_PROTOCOL = Traits<Cortex_M_Bootloader>::NIC_PROTOCOL;
const unsigned int MESSAGE_SIZE = 11;

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
            auto d = f->data<char>();
            for(int i=0; i<b->size(); i++)
                eMote3_USB::put(d[i]);
            eMote3_USB::flush();
            _nic->free(b);
        }
    }

private:
    Protocol _prot;
    NIC * _nic;
};

int main()
{
    GPIO led('c',3,GPIO::OUTPUT);
    led.set(false);
    while(!eMote3_USB::initialized());
    for(unsigned int i=0; i<10; i++)
    {
        led.set(true);
        for(volatile unsigned int j=0;j<0xffff;j++);
        led.set(false);
        for(volatile unsigned int j=0;j<0xffff;j++);
    }
    led.set(true);

    NIC nic;
    Receiver * r = new Receiver(BOOTLOADER_PROTOCOL, &nic);
    char buffer[MESSAGE_SIZE];
    while(true)
    {
        while(eMote3_USB::get_data(buffer, MESSAGE_SIZE) != MESSAGE_SIZE);
        nic.send(nic.broadcast(), BOOTLOADER_PROTOCOL, buffer, MESSAGE_SIZE);
    }
    
    return 0;
}
