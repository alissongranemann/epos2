/* Specification: see io_interf_v2.cc
 *
 * Implementation notes:
 * This file implements Domain 2.
 * Assuming Domain 0 is implemented in pc_loader.cc
 * Assuming Domain 1 is implemented in io_interf_v2.cc
 *
 * */

#include <utility/ostream.h>
#include <communicator.h>

using namespace EPOS;

typedef _SYS::Domain_2 Domain_2;

// static TSC::Time_Stamp guest_task_2_reception_begin = 0;
// static TSC::Time_Stamp guest_task_2_reception_end = 0;
// static TSC::Time_Stamp guest_task_2_reception_total = 0;
static TSC::Time_Stamp guest_task_2_begin = 0;
static TSC::Time_Stamp guest_task_2_end = 0;

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_2_TASK_ITERATIONS;
const int PDU = 500;

int main()
{
    if (_SYS::Traits<Application>::DONT_RUN_GUEST_OS_2_TASK) {
        return 0;
    }

    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "I/O interference - Version 2" << endl;
        db<Domain_2>(WRN) << "This is Domain 2" << endl;
        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) starting..." << endl;

        db<Domain_2>(WRN) << "TCP Test" << endl;
    }

    char data[PDU];
    TCP_Link * com;

    IP * ip = IP::get_by_nic(0);
    // IP::Address my_ip_address = ip->address();

    if (_SYS::Traits<Domain_2>::DBLV1) {
        // db<Domain_2>(WRN) << "  IP: " << my_ip_address << endl;
        db<Domain_2>(WRN) << "  MAC: " << ip->nic()->address() << endl;

        db<Domain_2>(WRN) << "Receiver:" << endl;
    }

    if (_SYS::Traits<Domain_2>::DBLV1) {
        // db<Domain_2>(WRN) << "Will listen at: " << my_ip_address << ":8000" << endl;
    }

    if (_SYS::Traits<Domain_2>::DBLV0) {
        db<Domain_2>(WRN) << "GT2 Iterations:" << ITERATIONS << endl;
    }

    db<Domain_2>(WRN) << "@RTS" << endl; /// Signals to t4exp that Auxiliar Experiment Runner: TCP/IP client can start

    com = new TCP_Link(TCP::Port(8000)); // listen

    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "Some client has connected" << endl;
    }

    guest_task_2_begin = TSC::time_stamp();

    for(int i = 0; i < ITERATIONS; i++) {
        // guest_task_2_reception_begin = TSC::time_stamp();

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

        // guest_task_2_reception_end = TSC::time_stamp();
        // guest_task_2_reception_total += (guest_task_2_reception_end - guest_task_2_reception_begin);
    }

    guest_task_2_end = TSC::time_stamp();

    delete com;


    NIC::Statistics stat = ip->nic()->statistics();
    if (_SYS::Traits<Domain_2>::DBLV1) {
        db<Domain_2>(WRN) << "Statistics\n"
         << "Tx Packets: " << stat.get_tx_packets() << "\n"
         << "Tx Bytes:   " << stat.get_tx_bytes() << "\n"
         << "Rx Packets: " << stat.get_rx_packets() << "\n"
         << "Rx Bytes:   " << stat.get_rx_bytes() << endl;

        db<Domain_2>(WRN) << "Task on Guest OS 2 (Domain 2) finishing..." << endl;
    }

    if (_SYS::Traits<Domain_2>::DBLV0) {
        // db<Domain_2>(WRN) << "gt2_total_reception_us = " << Chronometer_Aux::micro(guest_task_2_reception_total) << endl;
        db<Domain_2>(WRN) << "gt2_total_execution_us = " << Chronometer_Aux::micro(guest_task_2_end - guest_task_2_begin) << endl;
    }

    return stat.get_tx_bytes() + stat.get_rx_bytes();
}
