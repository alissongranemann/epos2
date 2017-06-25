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
    IAC_Common::New_Interest new_interest(region.center.x, region.center.y, region.center.z, region.radius, interest->period(), interest->expiry());
    Serial_Port::Message<IAC_Common::New_Interest> msg(IAC_Common::NEW_INTEREST, new_interest);
    serial_port->handle_tx_message(msg);
    serial_port->handle_rx_message();
}

//new node
void IAC::update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
    db<TSTP>(TRC) << "IAC::update: obs(" << obs << ",buf=" << buf << ")" << endl;
    TSTP::Header * packet = buf->frame()->data<TSTP::Header>();
    TSTP::Coordinates coord = packet->origin();
    IAC_Common::New_Node new_node(coord.x, coord.y, coord.z);
    Serial_Port::Message<IAC_Common::New_Node> msg(IAC_Common::NEW_NODE, new_node);
    serial_port->handle_tx_message(msg);
}

__END_SYS
