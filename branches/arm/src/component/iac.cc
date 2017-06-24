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

void IAC::new_interest(Serial_Port::Observer * obs, TSTP::Interest * interest){
    db<TSTP>(TRC) << "IAC::new_interest()" << endl;
    serial_port->attach(obs);
    TSTP::Region region = interest->region();
    Message msg;
    msg.type = 1;
    msg.x = region.center.x;
    msg.y = region.center.y;
    msg.z = region.center.z;
    msg.r = region.radius;
    msg.period = interest->period();
    msg.expiry = interest->expiry();
    serial_port->handle_tx_message(msg);
    serial_port->handle_rx_message();
}

//new node
void IAC::update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
    db<TSTP>(TRC) << "IAC::update: obs(" << obs << ",buf=" << buf << ")" << endl;
    TSTP::Header * packet = buf->frame()->data<TSTP::Header>();
    TSTP::Coordinates coord = packet->origin();
    Message msg;
    msg.type = 0;
    msg.x = coord.x;
    msg.y = coord.y;
    msg.z = coord.z;
    serial_port->handle_tx_message(msg);
}

__END_SYS
