#include <tstp.h>
#include <machine.h>
#include <uart.h>

using namespace EPOS;

OStream cout;
UART uart1(Traits<UART>::DEF_BAUD_RATE, Traits<UART>::DEF_DATA_BITS, Traits<UART>::DEF_PARITY, Traits<UART>::DEF_STOP_BITS, 1);

typedef TSTP::Data Data;
typedef TSTP::Region Region;
typedef TSTP::Time Time;
typedef TSTP::Header Header;

struct Data_Handler {
    static const unsigned int MAX_NAME_SIZE = 16;

    Data_Handler(const char * n) : count(0), updated(false) { 
        for(auto i = 0u; (i < MAX_NAME_SIZE) and n[i]; name[i] = n[i++]);
    }
    
    void process() {
        if(updated) {
            updated = false;
            cout << *this << endl;
        }
    }

    friend OStream & operator<<(OStream & os, const Data_Handler & d) {
        os << d.name << " [" << d.h.origin_time() << " , " << d.local_time << " , " << d.local_time - d.h.origin_time() << "] : " << "data = " << d.d << ", count = " << d.count << (d.h.origin_time() < d.local_time ? "" : " <= "); 
        auto oa = d.h.origin_address();
        auto lha = d.h.last_hop_address();
        if(oa != lha) { os << ", last_hop = " << lha; }
    }

    static void update(const Data & d, const Header * h, Data_Handler & data) {
        data.local_time = TSTP::time_now();
        data.h = *h;
        data.d = d;
        ++data.count;
        data.updated = true;
    }

    volatile unsigned int count;
    Header h;
    volatile Time local_time;
    volatile Data d;
    volatile bool updated;
    char name[MAX_NAME_SIZE];
};

Data_Handler outlet0_data("outlet0"), outlet1_data("outlet1"), lights0_data("lights0"), lights1_data("lights1");

void outlet0_update(const Data & d, const Header * h) { Data_Handler::update(d,h,outlet0_data); }
void outlet1_update(const Data & d, const Header * h) { Data_Handler::update(d,h,outlet1_data); }
void lights0_update(const Data & d, const Header * h) { Data_Handler::update(d,h,lights0_data); }
void lights1_update(const Data & d, const Header * h) { Data_Handler::update(d,h,lights1_data); }

int main()
{
    int o0c = 0, o1c = 0, l0c = 0, l1c = 0;
    cout << "Home Gateway" << endl;
    TSTP_MAC::address(TSTP_MAC::Address(0,0,0));
    
    TSTP::Watt W;

    Region outlet0_region(444, -200, -40);
    Region outlet1_region(-30, -50, -40);
    Region lights0_region(320, 0, 220);
    Region lights1_region(320, -110, 220);

    Time period(1000000);
    Time t0(TSTP::time_now() + period * 5);
    Time dt(10 * period);

    TSTP::RESPONSE_MODE response_mode(TSTP::RESPONSE_MODE::SINGLE);

    //TSTP::Interest lights0_interest(&lights0_update, lights0_region, t0, dt, period, W, 100, response_mode);
    TSTP::Interest outlet0_interest(&outlet0_update, outlet0_region, t0, dt, period, W, 100, response_mode);
    //TSTP::Interest lights1_interest(&lights1_update, lights1_region, t0, dt, period, W, 100, response_mode);
    TSTP::Interest outlet1_interest(&outlet1_update, outlet1_region, t0, dt, period, W, 100, response_mode);

    while(TSTP::time_now() <= t0+dt+3*period) {
        outlet0_data.process();
        outlet1_data.process();
        lights0_data.process();
        lights1_data.process();
        if(uart1.has_data()) { Machine::reboot(); }
    }

    cout << TSTP_MAC::statistics() << endl;
    cout << outlet0_data << endl;
    cout << outlet1_data << endl;
    cout << lights0_data << endl;
    cout << lights1_data << endl;

    while(true) {
        if(uart1.has_data()) {
            Machine::reboot();        
        }
    }

    return 0;
}
