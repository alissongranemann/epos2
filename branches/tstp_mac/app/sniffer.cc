#include <utility/ostream.h>
#include <nic.h>
#include <machine/cortex_m/cc2538_radio.h>

using namespace EPOS;

OStream cout;

const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {4,2};//Whatever

class Receiver : public IEEE802_15_4::Observer, private CC2538RF
{
public:
    typedef IEEE802_15_4::Protocol Protocol;
    typedef IEEE802_15_4::Buffer Buffer;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Observed Observed;

    Receiver(NIC * nic) : _nic(nic)
    {        
        xreg(FRMFILT0) &= ~FRAME_FILTER_EN; // Disable frame filtering
        _nic->attach(this, Traits<Secure_NIC>::PROTOCOL_ID);        
    }   

    void update(Observed * o, Protocol p, Buffer * b)
    {
        Frame * f = b->frame();
        char * d = f->data<char>();
        cout << endl << "=====================" << endl;
        cout << "Received " << b->size() << " bytes of payload from " << f->src() << " :" << endl;
        for(int i=0; i<b->size(); i++)
            cout << d[i];
        cout << endl << "=====================" << endl;
        _nic->free(b);
        //            _nic->stop_listening();
    }

private:
    NIC * _nic;
};

int main()
{
    cout << "Sniffing Secure_NIC packets" << endl;
    NIC * nic = new NIC();
    Receiver * r = new Receiver(nic);

    while(1);
    return 0;
}
