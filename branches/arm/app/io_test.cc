#include <machine.h>
#include <alarm.h>
#include <smart_data.h>

using namespace EPOS;

  struct Message {
        long x;
        long y;
        long z;

        friend OStream & operator<<(OStream & os, const Message & d) {
            os << d.x << d.y << d.z;
            return os;
        }
    }__attribute__((packed));

IF<Traits<USB>::enabled, USB, UART>::Result io;

OStream cout;

const unsigned int INTEREST_PERIOD = 1000000;

void print(const Message & d)
{
    CPU::int_disable();
    for(unsigned int i = 0; i < sizeof(Message); i++)
        io.put(reinterpret_cast<const char *>(&d)[i]);
    CPU::int_enable();
}

int main()
{
    // Get epoch time from serial
    TSTP::Time epoch = 0;
    char c = io.get();
    if(c != 'X') {
        epoch += c - '0';
        c = io.get();
        while(c != 'X') {
            epoch *= 10;
            epoch += c - '0';
            c = io.get();
        }
        TSTP::epoch(epoch);
    }

    Message msg;
    msg.x = -10;
    msg.y = 10;
    msg.z = 5;

    while(true) {
        Alarm::delay(INTEREST_PERIOD);
        print(msg);
    }

    return 0;
}