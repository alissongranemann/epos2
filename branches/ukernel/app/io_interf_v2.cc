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

static TSC::Time_Stamp guest_task_1_begin = 0;
static TSC::Time_Stamp guest_task_1_end = 0;

static TSC::Time_Stamp guest_job_1_begin = 0;
static TSC::Time_Stamp guest_job_1_end = 0;
static TSC::Time_Stamp guest_job_1_total = 0;

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;

const unsigned int WSS = 12 * 1024 * 1024; /* Work Set Size (in bytes).
    * On Intel Core 2 Quad Q9550
    * L2 is the last level cache (LLC) and it is shared among 2 cores.
    * There are 4 cores, 2 processors of 2 core each and, 2 x L2
    * (one for each processor).
    * L2 size is 6 MB.
    * Using an array that is twice than that to force cache misses.
    * */
unsigned int * work_set;
unsigned int value;

OStream cout;

int main()
{
    cout << "I/O interference - Version 2" << endl;
    cout << "This is Domain 1" << endl;
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;

    work_set = new unsigned int[WSS];

    guest_task_1_begin = TSC::time_stamp();

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        guest_job_1_begin = TSC::time_stamp();

        /* Read and write the work_set here. */
        // Sequential reads. TODO: try other access patterns
        for (unsigned int wsi = 0; wsi < WSS; wsi++) {
            value = work_set[wsi];
        }

        /* ---- */

        guest_job_1_end = TSC::time_stamp();
        guest_job_1_total += guest_job_1_end - guest_job_1_begin;

        Periodic_Thread::wait_next();
    }

    guest_task_1_end = TSC::time_stamp();

    cout << "Guest Task 1 total latency (cycles): " << guest_job_1_total << endl;
    cout << "Guest Task 1 total execution (cycles): " << (guest_task_1_end - guest_task_1_begin) << endl;
    cout << "Task on Guest OS 1 (Domain 1) finishing..." << endl;

    delete work_set;

    return 0;
}
