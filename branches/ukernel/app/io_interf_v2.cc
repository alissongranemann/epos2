/* Specification:
 * There are three domains
 *
 * Domain 0.
 * The VCPU of Domain 0 runs on PCPU (core) 0.
 * It receives interrupts from NIC because of TCP/IP packets that come from
 * the network.
 *
 * Domain 1.
 * The VCPU of Domain 1 runs on PCPU 1 and executes a hard real-time task.
 *
 * Domain 2
 * The VCPU of Domain 2 runs on PCPU 2 and executes an software real-time /
 * best-effort task printing data from the received TCP/IP packets.
 *
 * Assumed that
 * Task on Guest OS == VCPU == EPOS Thread at private task
 *
 * Variations from the specification:
 * For now, Domain 0 and Domain 2 are kind of mixed together since Domain 2 is
 * getting its packets using the kernel and not communicating with Domain 0
 * (through IPC) and Domain 0 is doing nothing.
 *
 * Implementation notes:
 * This file implements Domain 1.
 * Assuming Domain 0 is implemented in pc_loader.cc
 * Assuming Domain 2 is implemented in io_interf_v2_domain_2.cc
 *
 * */

#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;


const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;

OStream cout;

int main()
{
    cout << "I/O interference - Version 2" << endl;
    cout << "This is Domain 1" << endl;
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        for(int j = 0; j < 79; j++) {
            cout << "+";
        }
        cout << "" << endl;

        Periodic_Thread::wait_next();
    }

    cout << "Task on Guest OS 1 (Domain 1) finishing..." << endl;

    return 0;
}
