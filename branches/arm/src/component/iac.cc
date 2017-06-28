#include "iac.h"

__BEGIN_SYS

IAC::IAC() {
    db<TSTP>(TRC) << "IAC()" << endl;
    TSTP::Time epoch = IAC_Serial_Manager::epoch();
    TSTP::epoch(epoch);
    unsigned int PERIOD =  Tr * 1000000ull / Traits<System>::DUTY_CYCLE;
    Config config(PERIOD / 1000);//PERIOD in ms
    Config_Message msg(IAC_Common::CONFIG, config);
    IAC_Serial_Manager::handle_tx_message(msg);
}

IAC::~IAC() {
    db<TSTP>(TRC) << "~IAC()" << endl;
    TSTP::detach(this, reinterpret_cast<void *>(IAC_Common::NEW_NODE));
}

void IAC::init() {
    db<TSTP>(TRC) << "IAC::init()" << endl;
    IAC * iac = new IAC();
    TSTP::attach(iac, reinterpret_cast<void *>(IAC_Common::NEW_NODE));
}

void IAC::new_interest(IAC::Observer * obs, TSTP::Interested * interested) {
    db<TSTP>(TRC) << "IAC::new_interest()" << endl;
    TSTP::Region region = interested->region();
    TSTP::Coordinates coord = region.center;
    IAC_Common::New_Interest new_interest(coord.x, coord.y, coord.z, region.radius, interested->period(), interested->expiry(), interested);
    IAC_Serial_Manager::Message<IAC_Common::New_Interest> msg(IAC_Common::NEW_INTEREST, new_interest);
    db<TSTP>(TRC) << "IAC::new_interest() msg = " << new_interest << endl;
    IAC_Serial_Manager::attach(obs, new_interest.ref);
    IAC_Serial_Manager::handle_tx_message(msg);
    IAC_Serial_Manager::handle_rx_message();
}

//new node
void IAC::update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
    db<TSTP>(TRC) << "IAC::new_node: obs(" << obs << ",buf=" << buf << ")" << endl;
    TSTP::Header * packet = buf->frame()->data<TSTP::Header>();
    TSTP::Coordinates coord = packet->origin();
    New_Node new_node(coord.x, coord.y, coord.z);
    New_Node_Message msg(IAC_Common::NEW_NODE, new_node);
    IAC_Serial_Manager::handle_tx_message(msg);
    IAC_Serial_Manager::handle_rx_message();
}

__END_SYS
