#include <machine.h>
#include <alarm.h>
#include <utility/ostream.h>
#include <i2c.h>
#include <gpio.h>
#include <cpu.h>
#include <machine/cortex_m/emote3_gptm.h>

using namespace EPOS;

OStream cout;
I2C *i2c;

int main()
{
    cout << "I2C Test\n";
    GPIO g('c',3, GPIO::OUTPUT);
    char received_data[5] = "0000";
    i2c =  new I2C(0, 'b', 1, 'b', 0); //master, SDA = PB1, SCL = PB0
    while(1)
    {
        g.set(1);
        eMote3_GPTM::delay(1000000);
        cout << "Hello" << endl;
        g.set(0);
        eMote3_GPTM::delay(1000000);
        auto ret = i2c->put(0x40, 0xE7);
        cout << "put = " << ret << endl;
        //ret = i2c->get(0x40, received_data, 2);
        ret = i2c->get(0x40, &received_data[0]);
        cout << "get = " << ret << endl;
        cout << "received_data[0]=" << received_data[0] << " received_data[1]=" << received_data[1] << endl;
    }
    return 0;
}
