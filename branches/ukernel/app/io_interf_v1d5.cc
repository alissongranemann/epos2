/* Specification:
 * There are two domains
 * Domain 0.
 * The VCPU of Domain 0 runs on PCPU (core) 0.
 * It receives interrupts from NIC (because of TCP/IP packets that come from the network).
 *
 * Domain 1.
 * The VCPU of Domain 1 runs on PCPU 1 and executes a hard real-time task.
 *
 * Assumed that
 * Task on Guest OS == VCPU == EPOS Thread at private task
 *
 *
 * Implementation notes:
 * This file implements Domain 1.
 * Assuming Domain 0 is implemented in pc_loader.cc
 *
 * */

#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;


const int ITERATIONS = 3;

OStream cout;

int main()
{
    cout << "I/O interference - Version 1.5" << endl;
    cout << "This is Domain 1" << endl;
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        cout << "+";
        Periodic_Thread::wait_next();
    }

    cout << "Task on Guest OS 1 (Domain 1) finishing..." << endl;

    return 0;
}
