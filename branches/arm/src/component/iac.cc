#include "iac.h"

__BEGIN_SYS

IAC::Observed IAC::_observed;
IAC_Handler IAC::_handler;

IAC::IAC(){
    db<TSTP>(TRC) << "IAC()" << endl;
}

IAC::~IAC() {
    db<TSTP>(TRC) << "~IAC()" << endl;
    TSTP::detach(this, reinterpret_cast<void *>(0));
}

void IAC::init(){
    db<TSTP>(TRC) << "IAC::init()" << endl;
	IAC * iac = new IAC();
    TSTP::attach(iac, reinterpret_cast<void *>(0));
    //_handler = new IAC_IO_Handler();
}

void IAC::new_interest(Observer * obs){
    db<TSTP>(TRC) << "IAC::new_interest()" << endl;
    _observed.attach(obs);
    //write usb
    //bool * result = new bool(true);
    //notify(result);
    _handler.new_interest();
}

//new node
void IAC::update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
    db<TSTP>(TRC) << "IAC::update: obs(" << obs << ",buf=" << buf << ")" << endl;
    TSTP::Header * packet = buf->frame()->data<TSTP::Header>();
    TSTP::Coordinates coord = packet->origin();
    _handler.new_node();
}

__END_SYS
