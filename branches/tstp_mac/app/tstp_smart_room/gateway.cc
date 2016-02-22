#include <tstp.h>
#include <machine.h>
#include <uart.h>

using namespace EPOS;

OStream cout;
UART uart1(Traits<UART>::DEF_BAUD_RATE, Traits<UART>::DEF_DATA_BITS, Traits<UART>::DEF_PARITY, Traits<UART>::DEF_STOP_BITS, 1);

typedef TSTP::Data Data;
typedef TSTP::Region Region;
typedef TSTP::Time Time;

volatile bool o0u = false, o1u = false, l0u = false, l1u = false;
volatile Data o0, o1, l0, l1;

void outlet0_update(const Data & d) { o0 = d; o0u = true; }
void outlet1_update(const Data & d) { o1 = d; o1u = true; }
void lights0_update(const Data & d) { l0 = d; l0u = true; }
void lights1_update(const Data & d) { l1 = d; l1u = true; }

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

    TSTP::Interest lights0_interest(&lights0_update, lights0_region, t0, dt, period, W, 100, response_mode);
    TSTP::Interest outlet0_interest(&outlet0_update, outlet0_region, t0, dt, period, W, 100, response_mode);
    TSTP::Interest lights1_interest(&lights1_update, lights1_region, t0, dt, period, W, 100, response_mode);
    TSTP::Interest outlet1_interest(&outlet1_update, outlet1_region, t0, dt, period, W, 100, response_mode);

    while(TSTP::time_now() <= t0+dt+3*period) {
        if(o0u) { o0u = false; cout << "o0 = " << o0 << ", o0c = " << ++o0c << endl; }
        if(o1u) { o1u = false; cout << "o1 = " << o1 << ", o1c = " << ++o1c << endl; }
        if(l0u) { l0u = false; cout << "l0 = " << l0 << ", l0c = " << ++l0c << endl; }
        if(l1u) { l1u = false; cout << "l1 = " << l1 << ", l1c = " << ++l1c << endl; }
        if(uart1.has_data()) { Machine::reboot(); }
    }

    cout << TSTP_MAC::statistics() << endl;
    cout << "o0 = " << o0 << ", o0c = " << o0c << endl;
    cout << "o1 = " << o1 << ", o1c = " << o1c << endl;
    cout << "l0 = " << l0 << ", l0c = " << l0c << endl;
    cout << "l1 = " << l1 << ", l1c = " << l1c << endl;

    while(true) {
        if(uart1.has_data()) {
            Machine::reboot();        
        }
    }

    return 0;
}
