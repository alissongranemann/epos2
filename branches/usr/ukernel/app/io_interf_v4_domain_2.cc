THIS IS INCOMPLETE. TODO.

#include <utility/ostream.h>

using namespace EPOS;

typedef _SYS::Domain_2 Domain_2;

static const unsigned int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_2_TASK_ITERATIONS;
static const unsigned int PDU = 1500; // NIC_MTU


static TSC::Time_Stamp guest_task_2_begin = 0;
static TSC::Time_Stamp guest_task_2_end = 0;

int main()
{
    if (_SYS::Traits<Application>::DONT_RUN_GUEST_OS_2_TASK) {
        return 0;
    }

    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "I/O interference - Version 4" << endl;
        db<Domain_2>(WRN) << "This is Domain 2" << endl;
        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) starting..." << endl;
    }

    NIC nic;
    Ether_Channel ether_channel;
    nic.attach(&nic_observer, 0x8888);

    char data[PDU];
    Ether_Channel_Link * com;

    if (_SYS::Traits<Domain_2>::DBLV0) {
        db<Domain_2>(WRN) << "GT2 Iterations:" << ITERATIONS << endl;
    }

    db<Domain_2>(WRN) << "@RTS" << endl; /// Signals to t4exp that Auxiliar Experiment Runner: TCP/IP client can start

    com = new Ether_Channel_Link();

    guest_task_2_begin = TSC::time_stamp();

    for(int i = 0; i < ITERATIONS; i++) {

        int received = com->read(&data, sizeof(data));
        if(received == sizeof(data)) {
            if (_SYS::Traits<Domain_2>::DBLV1) {
                db<Domain_2>(WRN) << "  Data: " << data << endl;
            }
        }
        else {
            if (_SYS::Traits<Domain_2>::DBLV0) {
                db<Domain_2>(WRN) << "  Data was not correctly received. It was " << sizeof(data) << " bytes long, but " << received << " bytes were received!"<< endl;
            }
        }

    }

    guest_task_2_end = TSC::time_stamp();

    delete com;


    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) finishing..." << endl;
    }

    if (_SYS::Traits<Domain_2>::DBLV0) {
        db<Domain_2>(WRN) << "gt2_total_execution_us = " << Chronometer_Aux::micro(guest_task_2_end - guest_task_2_begin) << endl;
    }

    return 0;
}
