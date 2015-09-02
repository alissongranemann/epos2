#include <utility/ostream.h>
#include <nic.h>
#include <gpio.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

bool led_value;
GPIO * led;

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
        led_value = !led_value;
        led->set(led_value);
        cout << "Sending message: " << data << endl;
        nic->send(nic->broadcast(), 0x1010, data, sizeof data);
        cout << "Sent" << endl;
        data[0] = ((data[0] - '0' + 1) % 10) + '0';
        Alarm::delay(delay_time);
    }
}

class Receiver : public IEEE802_15_4::Observer
{
    typedef char data_type;

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
       //     led_value = !led_value;
            led_value = true;
            led->set(led_value);
//            Frame * f = b->frame();
//            auto d = f->data<data_type>();
//            cout << endl << "=====================" << endl;
//            cout << "Received " << b->size() << " bytes of payload from " << f->src() << " :" << endl;
//            for(int i=0; i<b->size()/sizeof(data_type); i++)
//                cout << d[i] << " ";
//            cout << endl << "=====================" << endl;
            _nic->free(b);
    //        _nic->off();
        }
    }

private:
    Protocol _prot;
    NIC * _nic;
};

void receiver()
{
    //cout << "Hello, I am the receiver." << endl; 
    //cout << "I will attach myself to the NIC and print every message I get." << endl;
    NIC * nic = new NIC();
    Receiver * r = new Receiver(0x1010, nic);
    //nic->listen();
    led->set(false);
    led_value = false;
    bool send = true;
    while(true)
    {
        const char data[] = "0 Hello, World!";
//        led_value = !led_value;
//        led->set(led_value);
//        kout << "Going to sleep" << endl;
//        cout << "woke up at:\t" << CC2538::woke_up_at << endl;
        if(send)
        {
            nic->send(nic->broadcast(), 0x1010, data, sizeof data);
        }
        send=false;
        if(Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::uart)
            eMote3_GPTM::delay(2515); 
        CPU::halt();
    }
    //if(Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::uart)
    //    eMote3_GPTM::delay(1415); 
    //Thread::self()->suspend();
    while(1)
    {
        //cout << "Delaying" << endl;
        //cout << "Delayed" << endl;
        auto scheduled_time = TSC::time_stamp() + (2 * TSC::frequency());
        cout << "scheduled_time: " << scheduled_time << endl;
        //cout << "schedule: " << scheduled_time << endl;
        nic->off();
        nic->schedule_listen(scheduled_time);
        //eMote3::save_clock_dividers();        

        cout << "wfi" << endl;
        // If UART is being used, it can mess up everything when changing power modes.
        // One must wait for it to finish printing and polling tx_done() doesn't help,
        // so I worked out by tests the minimum delay necessary (1415) for the UART to finish printing.
        if(Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::uart)
            eMote3_GPTM::delay(1415); 
        CPU::halt();
        cout << "/wfi" << endl;
        auto now = TSC::time_stamp();
        //eMote3_GPTM::delay(1000000);
        //cout << "/wfi" << endl;
        //eMote3::restore_clock_dividers();
        eMote3_GPTM::delay(6600000);
        //cout << MAC_Timer::read() << endl;
        cout << "woke up at:\t" << CC2538::woke_up_at << "\nscheduled:\t" << scheduled_time << "\nnow:\t\t" << now << endl;
    }
}

int main()
{
    if(Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::usb)
        while(!eMote3_USB::initialized());

    led = new GPIO('c',3, GPIO::OUTPUT);
    led_value = true;
    led->set(led_value);
//    cout << "Hello main" << endl;
//    cout << "=====================" << endl;
//    cout << "TSTP_MAC parameters: " << endl;
//    cout << "checking_interval: " << Traits<TSTP_MAC>::checking_interval << endl;
//    cout << "Tu: " << Traits<TSTP_MAC>::Tu << endl;
//    cout << "ts: " << Traits<TSTP_MAC>::ts << endl;
//    cout << "n_microframes: " << Traits<TSTP_MAC>::n_microframes << endl;
//    cout << "time_between_microframes: " << Traits<TSTP_MAC>::time_between_microframes << endl;
//    cout << "microframe_listening_time: " << Traits<TSTP_MAC>::microframe_listening_time << endl;
//    cout << "sleep_time: " << Traits<TSTP_MAC>::sleep_time << endl;
//    cout << "=====================" << endl;
//     sender();
   receiver();

    return 0;
}
