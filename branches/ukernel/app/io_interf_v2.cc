#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;

typedef _SYS::Domain_1 Domain_1;
typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;

static const unsigned int NUM_OF_PCPUS = _SYS::Traits<Application>::NUM_OF_PCPUS_ON_DOMAIN_1;
static const unsigned int FIRST_PCPU = 0;
static const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_1_TASK_ITERATIONS;
static const unsigned int WSS = (_SYS::Traits<Application>::WSS / 4); /* Work Set Size (in bytes). */
static const Microsecond PERIOD = _SYS::Traits<Application>::DOMAIN_PERIOD;
static const Microsecond DEADLINE = PERIOD;
static const Microsecond CAPACITY = PEDF::UNKNOWN;


static TSC::Time_Stamp guest_task_1_begin[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_task_1_end[NUM_OF_PCPUS];

static TSC::Time_Stamp guest_job_1_begin[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_job_1_end[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_job_1_total[NUM_OF_PCPUS];

static unsigned int * work_set[NUM_OF_PCPUS];

static Periodic_Thread * guest_tasks[NUM_OF_PCPUS];

int jobs(unsigned int pcpu)
{
    if (_SYS::Traits<Domain_1>::DBLV1) {
        db<Domain_1>(WRN) << "jobs pcpu " << pcpu << " starting" << endl;
    }

    unsigned int value;
    unsigned int i;

    work_set[pcpu] = new unsigned int[WSS];

    guest_task_1_begin[pcpu] = TSC::time_stamp();

    for (i = 0; i < ITERATIONS; i++) {
        guest_job_1_begin[pcpu] = TSC::time_stamp();
        ASM("job_begin:\n");

        /* Read and write the work_set here. */
        for (unsigned int tms = 0; tms < 1; tms++) {
            for (unsigned int wsi = 0; wsi < WSS; wsi++) {
                value = work_set[pcpu][wsi];
            }

            for (unsigned int wsi = 0; wsi < WSS; wsi++) {
                work_set[pcpu][wsi] = i;
            }
        }

        ASM("job_end:\n");
        /* ---- */

        guest_job_1_end[pcpu] = TSC::time_stamp();
        guest_job_1_total[pcpu] += (guest_job_1_end[pcpu] - guest_job_1_begin[pcpu]);

        Periodic_Thread::wait_next();
    }

    guest_task_1_end[pcpu] = TSC::time_stamp();

    delete work_set[pcpu];

    return 0;
}


int main()
{
    db<Domain_1>(WRN) << "# I/O interference - Version 2" << endl;
    db<Domain_1>(WRN) << "# This is Domain 1" << endl;
    db<Domain_1>(WRN) << "# Guest OS 1 (Domain 1) starting..." << endl;
    db<Domain_1>(WRN) << "WSS = " << sizeof(unsigned int) * WSS << endl;
    db<Domain_1>(WRN) << "NUM_OF_PCPUS = " << NUM_OF_PCPUS << endl;
    db<Domain_1>(WRN) << "ITERATIONS = " << ITERATIONS << endl;
    db<Domain_1>(WRN) << "Networking = " << ((_SYS::Traits<_SYS::Build>::NODES > 1) ? "enabled" : "disabled") << endl;

    Task * domain_u = Task::self();

    for (unsigned int pcpu = FIRST_PCPU; pcpu < NUM_OF_PCPUS; pcpu++) {
        PEDF criterion = PEDF(DEADLINE, PERIOD, CAPACITY, pcpu);
        Periodic_Thread::Configuration conf = Periodic_Thread::Configuration(PERIOD, ITERATIONS, Periodic_Thread::SUSPENDED, criterion, domain_u);
        guest_tasks[pcpu] = new Periodic_Thread(conf, &jobs, pcpu);
    }

    for (unsigned int pcpu = FIRST_PCPU; pcpu < NUM_OF_PCPUS; pcpu++) {
        guest_tasks[pcpu]->resume();
    }

    for (unsigned int pcpu = FIRST_PCPU; pcpu < NUM_OF_PCPUS; pcpu++) {
        guest_tasks[pcpu]->join();
    }

    for (unsigned int pcpu = FIRST_PCPU; pcpu < NUM_OF_PCPUS; pcpu++) {
        db<Domain_1>(WRN) << "total_latency_cycles_" << pcpu << " = " << guest_job_1_total[pcpu] << endl;
        db<Domain_1>(WRN) << "total_execution_cycles_" << pcpu << " = " << (guest_task_1_end[pcpu] - guest_task_1_begin[pcpu]) << endl;
        db<Domain_1>(WRN) << "total_latency_us_" << pcpu << " = " << Chronometer_Aux::micro(guest_job_1_total[pcpu]) << endl;
        db<Domain_1>(WRN) << "total_execution_us_" << pcpu << " = " << Chronometer_Aux::elapsed_micro(guest_task_1_begin[pcpu], guest_task_1_end[pcpu]) << endl;
        delete guest_tasks[pcpu];
    }

    db<Domain_1>(WRN) << "# Guest OS 1 (Domain 1) finishing..." << endl;

    return 0;
}
