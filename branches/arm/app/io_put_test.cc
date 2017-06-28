#include <machine.h>
#include <alarm.h>
#include <smart_data.h>
#include <tstp.h>
#include "../include/iac_serial_manager.h"

using namespace EPOS;

OStream cout;

IF<Traits<USB>::enabled, USB, UART>::Result io;
IAC_Serial_Manager * serial_port;

const unsigned int PERIOD = 100000;

int send_msgs(int index){
    IAC::New_Node new_node(5, 10, 15);
    IAC::New_Node_Message msg1(IAC_Common::NEW_NODE, new_node);

    IAC::New_Interest new_interest(15, 10, 5, 15, PERIOD, PERIOD * 3, 35356);
    IAC::New_Interest_Message msg2(IAC_Common::NEW_INTEREST, new_interest);

    IAC_Common::Config config(116);
    IAC_Serial_Manager::Message<IAC_Common::Config> msg3(IAC_Common::CONFIG, config);

    while(true){
        Alarm::delay(PERIOD);
        cout << "send_msg thread=" << index << endl;
        serial_port->handle_tx_message(msg1);
        serial_port->handle_tx_message(msg2);
        serial_port->handle_tx_message(msg3);
    }
}

int main()
{
    serial_port = new IAC_Serial_Manager();

    Thread * thread_1 = new Thread(&send_msgs, 0);
    Thread * thread_2 = new Thread(&send_msgs, 1);

    cout << "threads created." << endl;
    thread_1->join();
    thread_2->join();

    return 0;
}
