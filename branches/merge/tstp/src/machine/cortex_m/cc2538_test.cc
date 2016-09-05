#include <utility/ostream.h>
#include <nic.h>
#include <gpio.h>
#include <alarm.h>
#include <periodic_thread.h>

using namespace EPOS;

OStream cout;

const static unsigned int delay_time = 2000000;

bool led_value;
GPIO * led;

int sender()
{
    char data[] = "0 Hello, World!";
    NIC * nic = new NIC();
    cout << "Hello, I am the sender." << endl;
    cout << "I will send a message every " << delay_time << " microseconds." << endl;
    while(1)
    {
        Periodic_Thread::wait_next();
        led_value = !led_value;
        led->set(led_value);
        cout << "Sending message: " << data << endl;
        cout << "send result = ";
        cout << nic->send(nic->broadcast(), NIC::PTP, data, 16) << endl;
        data[0] = ((data[0] - '0' + 1) % 10) + '0';
    }
    return 0;
}

class Receiver : public IEEE802_15_4::Observer
{
public:
    typedef char data_type;

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
        cout << "Received buffer " << reinterpret_cast<void *>(b) << endl;
        if(p == _prot)
        {
            led_value = !led_value;
            led->set(led_value);
            Frame * f = reinterpret_cast<Frame *>(b->frame());
            auto d = f->data<data_type>();
            cout << endl << "=====================" << endl;
            cout << "Received " << b->size() << " bytes of payload from " << f->src() << " :" << endl;
            for(int i=0; i<b->size()/sizeof(data_type); i++)
                cout << d[i] << " ";
            cout << endl << "=====================" << endl;
            _nic->free(b);
        }
    }

private:
    Protocol _prot;
    NIC * _nic;
};

int receive_alarm(NIC * nic)
{
    Receiver::data_type data[128];
    NIC::Protocol prot;
    NIC::Address from;

    while(true) {
        cout << "Waiting..." << endl;
        Periodic_Thread::wait_next();
        cout << "Woke up!" << endl;
        int size = nic->receive(&from, &prot, data, 128);
        cout << endl << "=====================" << endl;
        cout << "Received " << size << " bytes of payload from " << from << " :" << endl;
        for(int i=0; i<size/sizeof(Receiver::data_type); i++)
            cout << data[i] << " ";
        cout << endl << "=====================" << endl;
    }
}

int receive(NIC * nic)
{
    Receiver::data_type data[128];
    NIC::Protocol prot;
    NIC::Address from;

    while(true) {
        cout << "Waiting..." << endl;
        Periodic_Thread::wait_next();
        cout << "Woke up!" << endl;
        int size = nic->receive(&from, &prot, data, 128);
        cout << endl << "=====================" << endl;
        cout << "Received " << size << " bytes of payload from " << from << " :" << endl;
        for(int i=0; i<size/sizeof(Receiver::data_type); i++)
            cout << data[i] << " ";
        cout << endl << "=====================" << endl;
    }
}

void receiver()
{
    cout << "Hello, I am the receiver." << endl;
    cout << "I will attach myself to the NIC and print every message I get." << endl;
    NIC * nic = new NIC();
    if(Traits<ELP>::synchronous) {
        //receive_alarm(nic);
        new Periodic_Thread(2700000, receive, nic);
    }
    else
        Receiver * r = new Receiver(NIC::PTP, nic);
}

int main()
{
    cout << "Hello main" << endl;

    led = new GPIO('C',3, GPIO::OUTPUT);
    led_value = true;
    led->set(led_value);

    receiver();
    Periodic_Thread * sender_thread = new Periodic_Thread(delay_time, sender);

    sender_thread->join();

    return 0;
}
