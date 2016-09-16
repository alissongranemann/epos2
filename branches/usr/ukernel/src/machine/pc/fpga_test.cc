#include <utility/ostream.h>

using namespace EPOS;

static TSC::Time_Stamp transaction_begin;
static TSC::Time_Stamp transaction_end;

static OStream cout;

int main()
{
    cout << "FPGA Test" << endl;
    // UART * uart = new UART();
    // uart->rts_down();

    FPGA::print_configuration();

    transaction_begin = TSC::time_stamp();
    // uart->rts_up();

    FPGA::run();
    // FPGA::wait_for_transaction();

    // uart->rts_down();
    transaction_end = TSC::time_stamp();

    cout << "transaction_cycles = " << (transaction_end - transaction_begin) << endl;
    cout << "transaction_us = " << Chronometer_Aux::elapsed_micro(transaction_begin, transaction_end) << endl;

    FPGA::report();

    // delete uart;
    cout << "Bye!" << endl;

    return 0;
}
