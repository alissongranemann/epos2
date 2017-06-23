#include <machine.h>
#include <alarm.h>
#include <smart_data.h>
#include <serial_port.h>

using namespace EPOS;

typedef Serial_Port::Message Message;

const unsigned int INTEREST_PERIOD = 1000000;

int main()
{

    Serial_Port * serial_port = new Serial_Port();

    Message * msg = new Message();
    msg->type = 1;

    while(true) {
        Alarm::delay(INTEREST_PERIOD);
        serial_port->handle_tx_message(msg);
    }

    return 0;
}
