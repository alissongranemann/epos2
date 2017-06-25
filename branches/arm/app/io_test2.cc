#include <machine.h>
#include <alarm.h>
#include <smart_data.h>
#include <serial_port.h>
#include <tstp.h>

using namespace EPOS;

typedef Serial_Port::Message Message;
typedef TSTP::Coordinates Coordinates;
typedef TSTP::Region Region;

template<typename Message>
struct MessageTest
{

	MessageTest(int _type, Message _msg) : type(_type), length(sizeof(Message)), msg(_msg){}

    unsigned int type;
    unsigned int length;
    Message msg;

}__attribute__((packed));

OStream cout;

IF<Traits<USB>::enabled, USB, UART>::Result io;

const unsigned int INTEREST_PERIOD = 1000000;

int main()
{

    Serial_Port * serial_port = new Serial_Port();

    Coordinates center_sensor(10,10,0);
    Region region(center_sensor, 0, TSTP::now(), -1);

    TSTP::Interest interest(region, TSTP::Unit::I32, TSTP::Mode::SINGLE, 0, 0, 0);

    // char data[64];
    //memcpy(data, &interest, sizeof(TSTP::Interest)); // "Serialize"

    Message msg1;
    msg1.type = 0;
    msg1.x = 10;
    msg1.y = 10;
    msg1.z = 10;

    Message msg2;
    msg2.type = 0;
    msg2.x = 10;
    msg2.y = 10;
    msg2.z = 10;
    msg2.r = 5;
    msg2.period = 100;
    msg2.expiry = 250;

    MessageTest<Message> msgTest(0, msg2);

    while(true){
    	//cout << sizeof(msgTest) << endl;
        for(unsigned int i = 0; i < sizeof(msgTest); i++)
            io.put(reinterpret_cast<const char *>(&msgTest)[i]);
    }
    return 0;
}
