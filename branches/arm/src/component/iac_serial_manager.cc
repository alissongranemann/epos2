#include "../../include/iac_serial_manager.h"

__BEGIN_SYS

Data_Observed<bool, int> IAC_Serial_Manager::_observed;
IAC_Serial_Manager::IO IAC_Serial_Manager::io;


IAC_Serial_Manager::IAC_Serial_Manager(){

}

IAC_Serial_Manager::~IAC_Serial_Manager(){

}

unsigned long long IAC_Serial_Manager::epoch() {
    int epoch = 0;
    char c = io.get();
    if(c != 'X') {
        epoch += c - '0';
        c = io.get();
        while(c != 'X') {
            epoch *= 10;
            epoch += c - '0';
            c = io.get();
            db<TSTP>(TRC) << "Serial_Port::epoch:" << endl;
        }
    }
    return 0;
}

__END_SYS
