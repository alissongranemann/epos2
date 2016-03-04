#include <tstp.h>
#include <machine.h>
#include <uart.h>
#include <utility/string.h>

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
        bool o0, o1, l0, l1;
        o0 = !strcmp(d.name, "outlet0");
        o1 = !strcmp(d.name, "outlet1");
        l0 = !strcmp(d.name, "lights0");
        l1 = !strcmp(d.name, "lights1");

        if(o1) os << "\t";
        if(l1) os << "\t";

        auto time_diff = (d.local_time - d.h.last_hop_time());
        os << d.name << " [" << d.h.origin_time() << " , " << d.h.last_hop_time() << " , " << d.local_time << " , " << time_diff << "] : " << "data = " << d.d << ", count = " << d.count << (d.h.origin_time() < d.local_time ? "" : " <= ") << endl; 

        if(o1) os << "\t";
        if(l1) os << "\t";
        auto oa = d.h.origin_address();
        auto lha = d.h.last_hop_address();
        os << "origin = " << oa << ", last_hop = " << lha;
        //if(oa != lha) { os << ", last_hop = " << lha; }
    }

    static void update(const Data & d, const Header * h, Data_Handler & data) {
        data.local_time = TSTP::time_now();
        data.h = *h;
        data.d = d;
        ++data.count;
        data.updated = true;
    }

    volatile unsigned int count;
    volatile Time local_time;
    volatile Data d;
    volatile bool updated;
    char name[MAX_NAME_SIZE];
    Header h;
};

Data_Handler outlet0_data("outlet0"), outlet1_data("outlet1"), lights0_data("lights0"), lights1_data("lights1"), desk0_data("desk0");

void outlet0_update(const Data & d, const Header * h) { Data_Handler::update(d,h,outlet0_data); }
void outlet1_update(const Data & d, const Header * h) { Data_Handler::update(d,h,outlet1_data); }
void lights0_update(const Data & d, const Header * h) { Data_Handler::update(d,h,lights0_data); }
void lights1_update(const Data & d, const Header * h) { Data_Handler::update(d,h,lights1_data); }
void desk0_update(const Data & d, const Header * h) { Data_Handler::update(d,h,desk0_data); }

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

    Region desk0_region(745, 300, 10);

    Time period(1000000);
    Time t0(TSTP::time_now() + period * 2);
    Time dt(10 * period);

    cout << "t0 = " << t0 << " , period = " << period << " , tend = " << t0 + dt << endl;

    TSTP::RESPONSE_MODE response_mode(TSTP::RESPONSE_MODE::SINGLE);

    TSTP::Interest outlet1_interest(&outlet1_update, outlet1_region, t0, dt, period * 5, W, 100, response_mode);
    //TSTP::Interest lights0_interest(&lights0_update, lights0_region, t0, dt, period * 3, W, 100, response_mode);
    //TSTP::Interest lights1_interest(&lights1_update, lights1_region, t0, dt, period, W, 100, response_mode);
    //TSTP::Interest outlet0_interest(&outlet0_update, outlet0_region, t0, dt, period * 2, W, 100, response_mode);
    //TSTP::Interest desk0_interest(&desk0_update, desk0_region, t0, dt, period, W, 100, response_mode);

    while(TSTP::time_now() <= t0+dt+period) {
        outlet0_data.process();
        outlet1_data.process();
        lights0_data.process();
        lights1_data.process();
        desk0_data.process();
//        if(uart1.has_data()) { Machine::reboot(); }
    }

    cout << endl << "=====================" << endl;
    cout << TSTP_MAC::statistics() << endl;
    cout << outlet0_data << endl;
    cout << outlet1_data << endl;
    cout << lights0_data << endl;
    cout << lights1_data << endl;
    cout << desk0_data << endl;
    cout << "Total data messages processed: " << lights0_data.count + lights1_data.count + outlet0_data.count + outlet1_data.count + desk0_data.count<< " " << outlet0_data.count << " " << outlet1_data.count << " " << lights0_data.count << " " << lights1_data.count << " " << desk0_data.count << endl;

    while(true) {
        if(uart1.has_data()) {
 //           Machine::reboot();        
        }
    }

    return 0;
}
