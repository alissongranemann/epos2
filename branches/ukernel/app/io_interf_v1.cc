/* Description at
 * Experiments_IO_Interf, Experiment 15 (wiki-experiments.os.git)
 *
 * Specification:
 * There are three domains
 * Domain 0.
 * The VCPU of Domain 0 runs on PCPU (core) 0.
 * It receives interrupts from NIC (because of TCP/IP packets that come from the network).
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
 * Additional notes:
 * The interrupt is received by the hardware, by the I/O APIC and routed to:
 * CPU 0? all CPUs? (test that).
 *
 * If Domain 0 is at CPU 0 then we could have a separation from Domain 0 and Domain 2.
 * However, Domain 2 should still need to use a kind of IPC to get data from Domain 0, right?
 *
 * */

#include <utility/ostream.h>
#include <utility/random.h>
#include <communicator.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;

typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;

const int ITERATIONS = 3;
const int PDU = 500;
const char * REMOTE_HOST = "194.167.1.1";

OStream cout;


int guest_os_1_task()
{
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        // Maybe add something here.
        Periodic_Thread::wait_next();
    }
}

int guest_os_2_task()
{
    cout << "Task on Guest OS 2 (Domain 2) starting..." << endl;

    cout << "TCP Test" << endl;

    char data[PDU];
    Link<TCP> * com;

    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "  MAC: " << ip->nic()->address() << endl;

    cout << "Receiver:" << endl;

    IP::Address peer_ip = ip->address();
    peer_ip[3]++;

    db<void>(WRN) << "Will listen at: " << ip->address() << ":8000" << endl;
    com = new Link<TCP>(TCP::Port(8000)); // listen
    db<void>(WRN) << "Some client has connected" << endl;

    for(int i = 0; i < ITERATIONS; i++) {
        int received = com->read(&data, sizeof(data));
        if(received == sizeof(data))
            cout << "  Data: " << data << endl;
        else
            cout << "  Data was not correctly received. It was " << sizeof(data) << " bytes long, but " << received << " bytes were received!"<< endl;
    }

    delete com;

    NIC::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

    return stat.tx_bytes + stat.rx_bytes;
}


/* The code and data segments of Domain 1 and 2 could be either loaded from an
 * ELF or copied from another domain (e.g. Domain 0).
 * For now, choosing the latter option.
 * */
Task * create_domain_u(int (* guest_os_task) (), int pcpu, bool hrt)
{
    db<void>(WRN) << "create_domain_u, guest_os_task = " << reinterpret_cast<void*>(guest_os_task) << ", pcpu = " << pcpu << ", hrt = " << hrt << endl;

    Task * domain_0 = Task::self();
    db<void>(WRN) << "domain_0" << endl;

    Address_Space * as_0 = domain_0->address_space();
    db<void>(WRN) << "as_0" << endl;

    Segment * cs_0 = domain_0->code_segment();
    db<void>(WRN) << "cs_0" << endl;
    CPU::Log_Addr code_0 = domain_0->code();
    db<void>(WRN) << "code_0" << endl;
    Segment * ds_0 = domain_0->data_segment();
    db<void>(WRN) << "ds_0" << endl;
    CPU::Log_Addr data_0 = domain_0->data();
    db<void>(WRN) << "data_0" << endl;

    db<void>(WRN) << "creating segment objects...";
    Segment cs_u = Segment(cs_0->size());
    Segment ds_u = Segment(ds_0->size());
    db<void>(WRN) << " done" << endl;

    db<void>(WRN) << "attaching segments...";
    CPU::Log_Addr code_u = as_0->attach(&cs_u);
    CPU::Log_Addr data_u = as_0->attach(&ds_u);
    db<void>(WRN) << " done" << endl;

    db<void>(WRN) << "copying segments...";
    memcpy(code_u, code_0, cs_u.size());
    memcpy(data_u, data_0, ds_u.size());
    db<void>(WRN) << " done" << endl;

    db<void>(WRN) << "detaching segments...";
    as_0->detach(&cs_u);
    as_0->detach(&ds_u);
    db<void>(WRN) << " done" << endl;

    Task * domain_u = 0;
    if (hrt) {
        // PEDF(const Microsecond & d, const Microsecond & p = SAME, const Microsecond & c = UNKNOWN, int cpu = ANY)
        Microsecond period = _SYS::Traits<Application>::DOMAIN_PERIOD;
        Microsecond deadline = period;
        Microsecond capacity = PEDF::UNKNOWN;

        db<void>(WRN) << "creating domain...";
        domain_u = new Task(0, &cs_u, &ds_u, guest_os_task);
        db<void>(WRN) << "domain = " << reinterpret_cast<void *>(domain_u) << endl;
        db<void>(WRN) << "domain (sys obj) = " << reinterpret_cast<void *>(domain_u->__stub()->id().unit()) << endl;
        /* I wanted to change the Task constructor that contais the Thread
         * configuration as parameter to create also RT_Threads
         * (or Periodic_Threads).
         * However, including periodic_thread.h in task.h caused circular
         * references.
         * Decided to create the periodic thread outside.
         * So, created a new contructor of Task that has a pointer to Thread as
         * parameter.
         * However, a task (domain_u in this case) must be created before the
         * Thread since Thread constructor will use it.
         * So, passing zero and initiating the main thread of the domain_u
         * after creating the thread (see bellow).
        */
        db<void>(WRN) << "done" << endl;

        db<void>(WRN) << "creating RT vcpu...";
        ASM("rtvcpu:");

        PEDF rt_vcpu_scheduling_criterion = PEDF(deadline, period, capacity, pcpu);
        db<void>(TRC) << "rt_vcpu_scheduling_criterion: " << rt_vcpu_scheduling_criterion << endl;
        db<void>(TRC) << "PEDF system object: " << reinterpret_cast<void *>(rt_vcpu_scheduling_criterion.__stub()->id().unit()) << endl;

        Periodic_Thread::Configuration rt_vcpu_conf = Periodic_Thread::Configuration(period, ITERATIONS, Periodic_Thread::READY, rt_vcpu_scheduling_criterion, domain_u);
        db<void>(TRC) << "rt_vcpu_conf: " << rt_vcpu_conf << endl;
        db<void>(TRC) << "rt_vcpu_conf system object: " << reinterpret_cast<void *>(rt_vcpu_conf.__stub()->id().unit()) << endl;

        Periodic_Thread * rt_vcpu = new Periodic_Thread(rt_vcpu_conf, guest_os_task); /* domain_u cannot be null here. */
        db<void>(WRN) << "rt_vcpu system object: " << reinterpret_cast<void *>(rt_vcpu->__stub()->id().unit()) << endl;

        db<void>(WRN) << " done" << endl;

        domain_u->main(reinterpret_cast<Thread *>(rt_vcpu));

    } else {
        // domain_u = new Task(Thread::Configuration(Thread::READY, CPU_Affinity(Priority::NORMAL, pcpu), domain_u), &cs_u, &ds_u, guest_os_task);
        db<void>(WRN) << "creating domain...";
        domain_u = new Task(0, &cs_u, &ds_u, guest_os_task);
        db<void>(WRN) << "done" << endl;

        db<void>(WRN) << "creating BE vcpu...";
        ASM("bevcpu:");
        Thread * be_vcpu = new Thread(Thread::Configuration(Thread::READY, PEDF(PEDF::APERIODIC), domain_u), guest_os_task);
        db<void>(WRN) << "done" << endl;

        domain_u->main(be_vcpu);
    }

    db<void>(WRN) << "will return created domain" << endl;

    ASM("end_of_create_domain_u:");

    return domain_u;
}

#include <network.h>

int main()
{
    Network::init();

    cout << "I/O interference - Version 1" << endl;

    cout << "Creating Domain 0" << endl;
    cout << "done!" << endl;

    cout << "Creating Domain 1" << endl;
    Task * domain_1 = create_domain_u(&guest_os_1_task, 1, true);
    cout << "done!" << endl;

    cout << "Creating Domain 2" << endl;
    Task * domain_2 = create_domain_u(&guest_os_2_task, 2, false);
    // domain_2->priority(_SYS::Thread::MAIN);
    cout << "done!" << endl;

    cout << "Domains loaded! Waiting for their VCPUs to finish ... " << endl;
    domain_1->main()->join();
    cout << "Domain 1 VCPU finished" << endl;
    domain_2->main()->join();
    cout << "Domain 2 VCPU finished" << endl;
    cout << "All VCPUs have finished!" << endl;

    cout << "Shutting down the machine!" << endl;
    cout << "Bye!" << endl;

    return 0;
}
