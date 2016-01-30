// EPOS PC_NIC Test Program

#include <utility/ostream.h>
#include <nic.h>
#include <alarm.h>

using namespace EPOS;

void generate_nic_interrupt();

OStream cout;

class NIC_Observer: public NIC::Observer
{
public:
    void update(NIC::Observed * obs, NIC::Protocol prot, NIC::Buffer * buf)
    {
        cout << "  Data: " << (char *) buf->frame()->data<void>() << endl;
    }
};


int main()
{
    cout << "  NIC Zero-Copy Receiver Application" << endl;

    NIC nic;
    NIC_Observer nic_observer;
    nic.attach(&nic_observer, 0x8888);
    Function_Handler handler(&generate_nic_interrupt);

    NIC::Address self = nic.address();
    cout << "  MAC: " << self << endl;

    Alarm alarm(1000000, &handler, Alarm::INFINITE);

    while(true);
}

void generate_nic_interrupt()
{
    ASM("int $42");
}
