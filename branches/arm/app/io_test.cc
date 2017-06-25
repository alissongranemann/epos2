#include <machine.h>
#include <alarm.h>
#include <smart_data.h>
#include <iac_serial_port_communication.h>
#include <tstp.h>

using namespace EPOS;

typedef IAC_Common::New_Interest New_Interest;
typedef IAC_Common::New_Node New_Node;
typedef Iac_Serial_Port_Communication::Message<IAC_Common::New_Interest> New_Interest_Message;
typedef Iac_Serial_Port_Communication::Message<IAC_Common::New_Node> New_Node_Message;
typedef TSTP::Coordinates Coordinates;
typedef TSTP::Region Region;

OStream cout;

IF<Traits<USB>::enabled, USB, UART>::Result io;

const unsigned int PERIOD = 1000000;

int main()
{

    Iac_Serial_Port_Communication * serial_port = new Iac_Serial_Port_Communication();

    Coordinates center_sensor(10,10,0);
    Region region(center_sensor, 0, TSTP::now(), -1);

    TSTP::Interest interest(region, TSTP::Unit::I32, TSTP::Mode::SINGLE, 0, 0, 0);

    New_Node new_node(5, 10, 15);
    New_Node_Message msg1(IAC_Common::NEW_NODE, new_node);

    New_Interest new_interest(15, 10, 5, region.radius, PERIOD, PERIOD * 3);
    New_Interest_Message msg2(IAC_Common::NEW_INTEREST, new_interest);

    while(true){
        Alarm::delay(PERIOD);
        serial_port->handle_tx_message(msg1);
        serial_port->handle_tx_message(msg2);
    }
    return 0;
}
