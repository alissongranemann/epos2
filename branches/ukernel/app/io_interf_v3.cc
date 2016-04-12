#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;

typedef _SYS::Domain_1 Domain_1;

static TSC::Time_Stamp guest_task_1_begin = 0;
static TSC::Time_Stamp guest_task_1_end = 0;

static TSC::Time_Stamp guest_job_1_begin = 0;
static TSC::Time_Stamp guest_job_1_end = 0;
static TSC::Time_Stamp guest_job_1_total = 0;

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_1_TASK_ITERATIONS;

const unsigned int WSS = 3 * 1024 * 1024; /* Work Set Size (in bytes).
    * On Intel Core 2 Quad Q9550
    * L2 is the last level cache (LLC) and it is shared among 2 cores.
    * There are 4 cores, 2 processors of 2 core each and, 2 x L2
    * (one for each processor).
    * L2 size is 6 MB.
    * Using an array that is twice than that (3MB * sizeof(unsigned int) ==
    * 3MB * 4 = 12 MB) to force cache misses.
    * */
unsigned int * work_set;
unsigned int value;

int main()
{
    db<Domain_1>(WRN) << "# I/O interference - Version 3" << endl;
    db<Domain_1>(WRN) << "# This is Domain 1" << endl;
    db<Domain_1>(WRN) << "# Task on Guest OS 1 (Domain 1) starting..." << endl;
    db<Domain_1>(WRN) << "WSS = " << sizeof(unsigned int) * WSS << endl;

    unsigned int i;

    work_set = new unsigned int[WSS];

    guest_task_1_begin = TSC::time_stamp();

    for (i = 0; i < ITERATIONS; i++) {
        guest_job_1_begin = TSC::time_stamp();

        /* Read and write the work_set here. */
        for (unsigned int tms = 0; tms < 1; tms++) {
            for (unsigned int wsi = 0; wsi < WSS; wsi++) {
                value = work_set[wsi];
            }

            for (unsigned int wsi = 0; wsi < WSS; wsi++) {
                work_set[wsi] = i;
            }
        }

        /* ---- */

        guest_job_1_end = TSC::time_stamp();
        guest_job_1_total += (guest_job_1_end - guest_job_1_begin);

        Periodic_Thread::wait_next();
    }

    guest_task_1_end = TSC::time_stamp();

    db<Domain_1>(WRN) << "gt1_total_latency_cycles = " << guest_job_1_total << endl;
    db<Domain_1>(WRN) << "gt1_total_execution_cycles = " << (guest_task_1_end - guest_task_1_begin) << endl;
    db<Domain_1>(WRN) << "gt1_total_latency_us = " << Chronometer_Aux::micro(guest_job_1_total) << endl;
    db<Domain_1>(WRN) << "gt1_total_execution_us = " << Chronometer_Aux::elapsed_micro(guest_task_1_begin, guest_task_1_end) << endl;
    db<Domain_1>(WRN) << "gt1_iterations = " << i << endl;
    db<Domain_1>(WRN) << "# Task on Guest OS 1 (Domain 1) finishing..." << endl;

    delete work_set;

    return 0;
}
