#include <iac_serial_port_communication.h>

__BEGIN_SYS

Iac_Serial_Port_Communication::Observed Iac_Serial_Port_Communication::_observed;

Iac_Serial_Port_Communication::Iac_Serial_Port_Communication(){

}

Iac_Serial_Port_Communication::~Iac_Serial_Port_Communication(){

}

void Iac_Serial_Port_Communication::epoch() {
    //TSTP::Time epoch = 0;
    char c = io.get();
    if(c != 'X') {
        //epoch += c - '0';
        c = io.get();
        while(c != 'X') {
            //epoch *= 10;
            //epoch += c - '0';
            c = io.get();
            db<TSTP>(TRC) << "Serial_Port::epoch:" << endl;
        }
        //TSTP::epoch(epoch);
    }
}

void Iac_Serial_Port_Communication::handle_rx_message() {
    db<TSTP>(TRC) << "Serial_Port::handle_rx_message:" << endl;
    char c = io.get();
    notify(new bool(c - '0'));
}

__END_SYS
