#include <color.h>
#include <utility/ostream.h>
#include <periodic_thread.h>
#include <communicator.h>
#include <thread.h>
#include <periodic_thread.h>

using namespace EPOS;

typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;
typedef int (* Guest_OS_Task )();

Thread * create_vcpu(Guest_OS_Task, int pcpu, bool hrt);

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;
const int NUM_OF_VCPUS = 3;
const int PDU = 500;

static bool is_domain_hrt[NUM_OF_VCPUS];
static Thread * vcpus[NUM_OF_VCPUS];
static Guest_OS_Task guest_os_tasks[NUM_OF_VCPUS];

OStream cout;

int guest_os_1_task()
{
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

int guest_os_2_task()
{
    cout << "Task on Guest OS 2 (Domain 2) starting..." << endl;
    cout << "TCP Test" << endl;

    char data[PDU];
    TCP_Link * com;

    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "  MAC: " << ip->nic()->address() << endl;

    cout << "Receiver:" << endl;

    IP::Address peer_ip = ip->address();
    peer_ip[3]++;

    db<void>(WRN) << "Will listen at: " << ip->address() << ":8000" << endl;
    com = new TCP_Link(TCP::Port(8000)); // listen
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
         << "Tx Packets: " << stat.get_tx_packets() << "\n"
         << "Tx Bytes:   " << stat.get_tx_bytes() << "\n"
         << "Rx Packets: " << stat.get_rx_packets() << "\n"
         << "Rx Bytes:   " << stat.get_rx_bytes() << endl;

    cout << "Task on Guest OS 2 (Domain 2) finishing..." << endl;

    return stat.get_tx_bytes() + stat.get_rx_bytes();
}

int main()
{
    is_domain_hrt[1] = _SYS::Traits<Application>::IS_DOM_1_HRT;
    is_domain_hrt[2] = _SYS::Traits<Application>::IS_DOM_2_HRT;
    guest_os_tasks[1] = guest_os_1_task;
    guest_os_tasks[2] = guest_os_2_task;

    cout << Color::BLUE() << "I/O Interf version Library Mode" << Color::END_COLOR() << endl;

    for (unsigned int i = 1; i < NUM_OF_VCPUS; i++)
    {
        cout << "Trying to load Domain " << i << endl;

        vcpus[i] = create_vcpu(guest_os_tasks[i], i, is_domain_hrt[i]);

        cout << "Domain " << i << " loaded!" << endl;
    }

    cout << "I will wait for all domains to finish!" << endl;
    for (unsigned int i = 1; i < NUM_OF_VCPUS; i++)
    {
        cout << "Waiting for Domain " << i << " to finish... " << endl;
        vcpus[i]->join();
    }

    cout << "All spawned processes have finished!" << endl;

    cout << "Shutting down the machine!" << endl;

    cout << "This is the end, my only friend, the end!" << endl;
    cout << "Bye!" << endl;
    return 0;
}

Thread * create_vcpu(Guest_OS_Task guest_os_task, int pcpu, bool hrt)
{
    Thread * vcpu = 0;

    if (hrt) {
        Microsecond period = _SYS::Traits<Application>::DOMAIN_PERIOD;
        Microsecond deadline = period;
        Microsecond capacity = PEDF::UNKNOWN;

        db<void>(WRN) << "creating RT vcpu...";
        ASM("rtvcpu:");

        PEDF rt_vcpu_scheduling_criterion = PEDF(deadline, period, capacity, pcpu);
        db<void>(TRC) << "rt_vcpu_scheduling_criterion: " << rt_vcpu_scheduling_criterion << endl;

        Periodic_Thread::Configuration rt_vcpu_conf = Periodic_Thread::Configuration(period, ITERATIONS, Periodic_Thread::READY, rt_vcpu_scheduling_criterion, 0);
        db<void>(TRC) << "rt_vcpu_conf: " << rt_vcpu_conf << endl;

        Periodic_Thread * rt_vcpu = new Periodic_Thread(rt_vcpu_conf, guest_os_task);

        db<void>(WRN) << " done" << endl;

        vcpu = reinterpret_cast<Thread *>(rt_vcpu);

    } else {
        db<void>(WRN) << "creating BE vcpu...";
        ASM("bevcpu:");
        Thread * be_vcpu = new Thread(Thread::Configuration(Thread::READY, PEDF(PEDF::APERIODIC, pcpu), 0), guest_os_task);
        db<void>(WRN) << "done" << endl;
        vcpu = be_vcpu;
    }

    return vcpu;
}
