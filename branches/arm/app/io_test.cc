#include <machine.h>
#include <alarm.h>
#include <smart_data.h>
#include <iac_serial_port_communication.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

IF<Traits<USB>::enabled, USB, UART>::Result io;
Iac_Serial_Port_Communication * serial_port;

const unsigned int PERIOD = 100000;

int send_msgs(int index){
    IAC::New_Node new_node(5, 10, 15);
    IAC::New_Node_Message msg1(IAC_Common::NEW_NODE, new_node);

    IAC::New_Interest new_interest(15, 10, 5, 15, PERIOD, PERIOD * 3);
    IAC::New_Interest_Message msg2(IAC_Common::NEW_INTEREST, new_interest);

    IAC_Common::Config config(116);
    Iac_Serial_Port_Communication::Message<IAC_Common::Config> msg3(IAC_Common::CONFIG, config);

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
    serial_port = new Iac_Serial_Port_Communication();

    Thread * thread_1 = new Thread(&send_msgs, 0);
    Thread * thread_2 = new Thread(&send_msgs, 1);

    cout << "threads created." << endl;
    thread_1->join();
    thread_2->join();

    return 0;
}
