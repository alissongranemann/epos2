#include <utility/ostream.h>
#include <machine.h>
#include <gpio.h>
#include <usb.h>
#include <tsc.h>

using namespace EPOS;

OStream cout;

void handler(GPIO * pin)
{
    return;

    GPIO led('c',3,GPIO::OUTPUT);
    while(1)
    {
        led.set(true);
        eMote3_GPTM::delay(1000000);
        led.set(false);
        eMote3_GPTM::delay(1000000);
    }
}

void timer_handler(const unsigned int & id)
{
    eMote3_GPTM::delay(1000000);
    cout << "timer handler" << endl;
    return;
}

void test_timer()
{
    unsigned int t1,t2,start;
    cout << "Counting 3 seconds" << endl;
    start = TSC::time_stamp();
    do
    {
        t1 = (TSC::time_stamp()-start) / TSC::frequency();
        t2 = (TSC::time_stamp()-start) / TSC::frequency();
        while(t1 == t2)
            t2 = (TSC::time_stamp()-start) / TSC::frequency();
        cout << t2 << endl;
    } while(t2 < 3);

    eMote3::wake_up_on(eMote3::WAKE_UP_EVENT::SLEEP_MODE_TIMER);
    TSC::wake_up_at(TSC::time_stamp() + (5*TSC::frequency()), &timer_handler);
}

volatile TSC::Time_Stamp wakeup;

// For more accurate timing, put this function directly 
// in IRQ32 in the vector table (src/arch/armv7/armv7_crt0.S)
extern "C" { void timestamp_handler_direct(); }
void timestamp_handler_direct()
{
    // This value may be outdated if it is sampled too quickly
    wakeup = TSC::time_stamp();
    return;
}
void timestamp_handler(const unsigned int & id)
{ 
    // This value may be outdated if it is sampled too quickly
    wakeup = TSC::time_stamp();
    return;
}

void test_wake_up_timings()
{
    while(1)
    {
        eMote3::power_mode(eMote3::POWER_MODE_1);

        cout << "PM1 delay" << endl;
        eMote3_GPTM::delay(500000);
        auto scheduled_wakeup = TSC::time_stamp() + (5*TSC::frequency());
        TSC::wake_up_at(scheduled_wakeup, &timestamp_handler);

        ASM("wfi");

        eMote3_GPTM::delay(500000);
        cout << scheduled_wakeup << " " << wakeup << endl;

        eMote3::power_mode(eMote3::POWER_MODE_2);

        cout << "PM2 delay" << endl;
        eMote3_GPTM::delay(500000);
        scheduled_wakeup = TSC::time_stamp() + (5*TSC::frequency());
        TSC::wake_up_at(scheduled_wakeup, &timestamp_handler);

        ASM("wfi");

        eMote3_GPTM::delay(500000);
        cout << scheduled_wakeup << " " << wakeup << endl;
    }
}

int main()
{
    GPIO led('c',3,GPIO::OUTPUT);
    {
        led.set(true);
        for(volatile int t=0;t<0xfffff;t++);
        led.set(false);
        for(volatile int t=0;t<0xfffff;t++);
    }

    test_wake_up_timings();
    return 0;
    //test_timer();
    //test_gpio();

//    GPIO a('d',5,GPIO::INPUT);
//
//    a.enable_interrupt(GPIO::BOTH_EDGES, &handler);

    eMote3::power_mode(eMote3::POWER_MODE_2); 
    ASM("wfi");
    //eMote3::power_mode(eMote3::ACTIVE); 
//    a.disable_interrupt();
//    CPU::int_disable();
    while(1)
    {
        led.set(true);
        for(volatile int t=0;t<0xfffff;t++);
        led.set(false);
        for(volatile int t=0;t<0xfffff;t++);
    }

    return 0;
}
