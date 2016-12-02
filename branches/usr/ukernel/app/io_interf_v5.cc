#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;

typedef _SYS::Domain_1 Domain_1;
typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;

static const unsigned int NUM_OF_PCPUS = _SYS::Traits<Application>::NUM_OF_PCPUS_ON_DOMAIN_1;
static const unsigned int FIRST_PCPU = 0;
static const unsigned int WORKER_PCPU = 3; /* For now, there is a single Worker PCPU. This specifies which one is it. */

static const bool IO_INTERF_INTERRUPTS = _SYS::Traits<Application>::IO_INTERF_INTERRUPTS;

/// Factors
/* DMA - related */
static const bool FPGA_ENABLED = _SYS::Traits<_SYS::FPGA>::enabled;
static const bool FPGA_MONITOR_ENABLED = _SYS::Traits<_SYS::FPGA>::monitor_enabled;
static const unsigned int DMA_OPERATION = _SYS::Traits<_SYS::FPGA>::DMA_OPERATION;
static const unsigned int DMA_BUFFER_SIZE = _SYS::Traits<_SYS::FPGA>::DMA_BUFFER_SIZE;
static const unsigned int WRITE_DMA_TLP_SIZE = _SYS::Traits<_SYS::FPGA>::WRITE_DMA_TLP_SIZE;
static const unsigned int WRITE_DMA_TLP_COUNT = _SYS::Traits<_SYS::FPGA>::WRITE_DMA_TLP_COUNT;
static const unsigned long long DELAY_BETWEEN_DMA_TRANSACTIONS = _SYS::Traits<_SYS::FPGA>::DELAY_BETWEEN_DMA_TRANSACTIONS;
static const unsigned int DMA_RUNS = _SYS::Traits<Application>::DMA_RUNS;
static const bool AUTO_RESTART_DMA_TRANSACTIONS = _SYS::Traits<_SYS::FPGA>::AUTO_RESTART_DMA_TRANSACTIONS;

/* Worker - related */
static const unsigned int WORKER_ACCESS_PATTERN = _SYS::Traits<Application>::worker_access_pattern;
static const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_1_TASK_ITERATIONS;

/// ----

static const unsigned int WSS = (_SYS::Traits<Application>::WSS / 4); /* Work Set Size (in bytes). */

static TSC::Time_Stamp guest_task_1_begin[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_task_1_end[NUM_OF_PCPUS];

static TSC::Time_Stamp guest_job_1_begin[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_job_1_end[NUM_OF_PCPUS];
static TSC::Time_Stamp guest_job_1_total[NUM_OF_PCPUS];

static TSC::Time_Stamp worker_time_stamps[ITERATIONS]; /* Designed for a single CPU. */

static unsigned int * work_set[NUM_OF_PCPUS];

static Thread * guest_tasks[NUM_OF_PCPUS];

/* Memory access algorithms */
/*! Sequential access. 75% reads, 25% writes. */
void seq_r75_w25(unsigned int pcpu)
{
    unsigned int value;
    for (unsigned int wsi = 0; wsi < WSS; wsi++) {
        if (wsi % 5) {
            value = work_set[pcpu][wsi];
        } else {
            work_set[pcpu][wsi] = wsi;
        }
    }
}

void rand_r75_w25(unsigned int pcpu)
{
    unsigned int value;
    unsigned int pos;

    for (unsigned int wsi = 0; wsi < WSS; wsi++) {
        pos = Random::random() % WSS;

        // db<Domain_1>(WRN) << "pos " << pos << endl;

        if (wsi % 5) {
            value = work_set[pcpu][pos];
        } else {
            work_set[pcpu][pos] = wsi;
        }
    }
}
/* ---- */

int jobs()
{
    unsigned int pcpu = WORKER_PCPU;

    // db<void>(WRN) << "Worker begin" << endl;

    if (_SYS::Traits<Domain_1>::DBLV1) {
        db<Domain_1>(WRN) << "jobs pcpu " << pcpu << " starting" << endl;
    }

    unsigned int i;

    work_set[pcpu] = new unsigned int[WSS];

    guest_task_1_begin[pcpu] = TSC::time_stamp();

    for (i = 0; i < ITERATIONS; i++) {
        guest_job_1_begin[pcpu] = TSC::time_stamp();

        /* Read and write the work_set here. */
        ASM("job_begin:\n");
        switch (WORKER_ACCESS_PATTERN)
        {
        case _SYS::SEQ_R75_W25: {
            seq_r75_w25(pcpu);
        } break;
        case _SYS::RAND_R75_W25: {
            rand_r75_w25(pcpu);
        } break;
        default: {
        }
        }

        ASM("job_end:\n");
        /* ---- */

        guest_job_1_end[pcpu] = TSC::time_stamp();
        guest_job_1_total[pcpu] += (guest_job_1_end[pcpu] - guest_job_1_begin[pcpu]);
        worker_time_stamps[i] = guest_job_1_end[pcpu] - guest_job_1_begin[pcpu];

        // Periodic_Thread::wait_next();
    }

    guest_task_1_end[pcpu] = TSC::time_stamp();

    // delete work_set[pcpu];

    db<void>(WRN) << "Worker done" << endl;

    return 0;
}


int interf()
{
    if (FPGA_ENABLED) {
        if (IO_INTERF_INTERRUPTS || AUTO_RESTART_DMA_TRANSACTIONS) {
            FPGA::run(); /* Subsequent runs will be triggered by FPGA's
                          * interrupt handler. */
        } else {
            for (unsigned int i = 0; i < DMA_RUNS; i++) {
                FPGA::run();
            }
        }
    }

    db<void>(WRN) << "Interf done" << endl;
}


void print_factors()
{
    /* DMA - related */
    db<Domain_1>(WRN) << "Experiment Factors" << endl;
    db<Domain_1>(WRN) << "FPGA = " << (FPGA_ENABLED ? "enabled" : "disabled") << endl;
    db<Domain_1>(WRN) << "DMA Operation = " << ((DMA_OPERATION == _SYS::WRITE)? "WRITE" : "READ") << endl;
    db<Domain_1>(WRN) << "DMA buffer size (bytes) = " << DMA_BUFFER_SIZE << endl;
    db<Domain_1>(WRN) << "WRITE_DMA_TLP_SIZE = " << WRITE_DMA_TLP_SIZE << endl;
    db<Domain_1>(WRN) << "WRITE_DMA_TLP_COUNT = " << WRITE_DMA_TLP_COUNT << endl;
    db<Domain_1>(WRN) << "DELAY_BETWEEN_DMA_TRANSACTIONS = " << DELAY_BETWEEN_DMA_TRANSACTIONS << endl;
    db<Domain_1>(WRN) << "DMA_RUNS = " << DMA_RUNS << endl;
    db<Domain_1>(WRN) << "AUTO_RESTART_DMA_TRANSACTIONS = " << AUTO_RESTART_DMA_TRANSACTIONS << endl;

    /* Worker - related */
    db<Domain_1>(WRN) << "WORKER_ACCESS_PATTERN = " << ((WORKER_ACCESS_PATTERN == _SYS::SEQ_R75_W25)? "SEQ_R75_W25" : "RAND_R75_W25") << endl;
    db<Domain_1>(WRN) << "WORKER_SET_SIZE = " << sizeof(unsigned int) * WSS << endl;
    db<Domain_1>(WRN) << "WORKER_ITERATIONS = " << ITERATIONS << endl;

    db<Domain_1>(WRN) << "----" << endl;
}


void report()
{
    unsigned int pcpu = WORKER_PCPU;
    db<Domain_1>(WRN) << "total_latency_cycles_" << pcpu << " = " << guest_job_1_total[pcpu] << endl;
    db<Domain_1>(WRN) << "total_execution_cycles_" << pcpu << " = " << (guest_task_1_end[pcpu] - guest_task_1_begin[pcpu]) << endl;
    db<Domain_1>(WRN) << "total_latency_us_" << pcpu << " = " << Chronometer_Aux::micro(guest_job_1_total[pcpu]) << endl;
    db<Domain_1>(WRN) << "total_execution_us_" << pcpu << " = " << Chronometer_Aux::elapsed_micro(guest_task_1_begin[pcpu], guest_task_1_end[pcpu]) << endl;

    db<Domain_1>(WRN) << "latency_us per iteration" << endl;
    for (unsigned int i = 0; i < ITERATIONS; i++) {
        db<Domain_1>(WRN) << Chronometer_Aux::micro(worker_time_stamps[i]) << endl;
    }
}

int main()
{
    db<Domain_1>(WRN) << "# I/O interference - Version 5" << endl;
    db<Domain_1>(WRN) << "# This is Domain 1" << endl;
    db<Domain_1>(WRN) << "# Guest OS 1 (Domain 1) starting..." << endl;
    db<Domain_1>(WRN) << "NUM_OF_PCPUS = " << NUM_OF_PCPUS << endl;
    db<Domain_1>(WRN) << "Networking = " << ((_SYS::Traits<_SYS::Build>::NODES > 1) ? "enabled" : "disabled") << endl;

    print_factors();

    if (FPGA_ENABLED) FPGA::print_configuration();
    if (FPGA_MONITOR_ENABLED) FPGA::monitor_start();

    Random::seed(1);

    Task * domain_u = Task::self();

    guest_tasks[1] = new Thread(Thread::Configuration(Thread::SUSPENDED, PEDF(PEDF::APERIODIC, WORKER_PCPU), domain_u), &jobs);
    guest_tasks[0] = new Thread(Thread::Configuration(Thread::SUSPENDED, PEDF(PEDF::APERIODIC, 0), domain_u), &interf);

    guest_tasks[0]->resume();
    guest_tasks[1]->resume();

    guest_tasks[0]->join();
    guest_tasks[1]->join();

    delete guest_tasks[1];
    delete guest_tasks[0];

    if (FPGA_ENABLED) FPGA::report();

    report();

    if (FPGA_MONITOR_ENABLED) FPGA::monitor_stop();

    db<Domain_1>(WRN) << "# Guest OS 1 (Domain 1) finishing..." << endl;
    db<Domain_1>(WRN) << "Bye!" << endl;

    return 0;
}
