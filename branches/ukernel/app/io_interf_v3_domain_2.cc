#include <utility/ostream.h>

using namespace EPOS;

typedef _SYS::Domain_2 Domain_2;

static TSC::Time_Stamp guest_task_2_begin = 0;
static TSC::Time_Stamp guest_task_2_end = 0;

static const unsigned int NIC_MTU = 1500;
static const unsigned int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_2_TASK_ITERATIONS;
static unsigned int received_frames = 0;

int main()
{
    if (_SYS::Traits<Application>::DONT_RUN_GUEST_OS_2_TASK) {
        return 0;
    }

    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "I/O interference - Version 3" << endl;
        db<Domain_2>(WRN) << "This is Domain 2" << endl;
        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) starting..." << endl;
    }

    NIC nic;
    // NIC::Address src;
    // NIC::Protocol prot;
    char data[NIC_MTU];

    // NIC::Address self = nic.address();
    // db<Domain_2>(WRN) << "  MAC: " << self << endl;

    if (_SYS::Traits<Domain_2>::DBLV0) {
        db<Domain_2>(WRN) << "GT2 Iterations:" << ITERATIONS << endl;
    }

    db<Domain_2>(WRN) << "@RTS" << endl; /// Signals to t4exp that Auxiliar Experiment Runner: Ethernet client can start

    guest_task_2_begin = TSC::time_stamp();

    for(unsigned int i = 0; i < ITERATIONS; i++) {
        // nic.receive(&src, &prot, data, NIC_MTU);
        nic.receive(data, NIC_MTU);
        if (_SYS::Traits<Domain_2>::DBLV1) {
            db<Domain_2>(WRN) << "  Data: " << data << endl;
        }
    }

    guest_task_2_end = TSC::time_stamp();

    if (_SYS::Traits<Domain_2>::DBLV0) {
        db<Domain_2>(WRN) << "gt2_total_execution_us = " << Chronometer_Aux::micro(guest_task_2_end - guest_task_2_begin) << endl;
    }


    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) finishing..." << endl;
    }
}
