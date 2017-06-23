#include "iac.h"

__BEGIN_SYS

Serial_Port * IAC::serial_port;

IAC::IAC(){
    db<TSTP>(TRC) << "IAC()" << endl;
    serial_port = new Serial_Port();
    serial_port->epoch();
}

IAC::~IAC() {
    db<TSTP>(TRC) << "~IAC()" << endl;
    TSTP::detach(this, reinterpret_cast<void *>(0));
}

void IAC::init(){
    db<TSTP>(TRC) << "IAC::init()" << endl;
    IAC * iac = new IAC();
    TSTP::attach(iac, reinterpret_cast<void *>(0));
}

void IAC::new_interest(Observer * obs, TSTP::Interest * interest){
    db<TSTP>(TRC) << "IAC::new_interest()" << endl;
    Message * msg = new Message();
    msg->type = 1;
    serial_port->handle_tx_message(msg);
    serial_port->handle_rx_message();
}

//new node
void IAC::update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
    db<TSTP>(TRC) << "IAC::update: obs(" << obs << ",buf=" << buf << ")" << endl;
    TSTP::Header * packet = buf->frame()->data<TSTP::Header>();
    TSTP::Coordinates coord = packet->origin();
    Message * msg = new Message();
    serial_port->handle_tx_message(msg);
}

__END_SYS
