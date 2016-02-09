// EPOS ELF Loader Program

#include <utility/ostream.h>
#include <utility/elf.h>
#include <machine.h>
#include <system/info.h>
#include <segment.h>
#include <thread.h>
#include <task.h>
#include <communicator.h>
#include <network.h>
#include <aux_debug.h>

using namespace EPOS;

typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;

int dom0();

OStream cout;

Thread * create_vcpu(Task * domain_u, int (* guest_os_task) (), int pcpu, bool hrt)
{
    if (! domain_u) {
        db<void>(ERR) << "Error: domain_u cannot be null!" << endl;
        return 0;
    }

    Thread * vcpu = 0;

    if (hrt) {
        Microsecond period = _SYS::Traits<Application>::DOMAIN_PERIOD;
        Microsecond deadline = period;
        Microsecond capacity = PEDF::UNKNOWN;
        unsigned int iterations = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;

        db<void>(WRN) << "creating RT vcpu...";
        ASM("rtvcpu:");

        PEDF rt_vcpu_scheduling_criterion = PEDF(deadline, period, capacity, pcpu);
        db<void>(TRC) << "rt_vcpu_scheduling_criterion: " << rt_vcpu_scheduling_criterion << endl;
        db<void>(TRC) << "PEDF system object: " << reinterpret_cast<void *>(rt_vcpu_scheduling_criterion.__stub()->id().unit()) << endl;

        Periodic_Thread::Configuration rt_vcpu_conf = Periodic_Thread::Configuration(period, iterations, Periodic_Thread::READY, rt_vcpu_scheduling_criterion, domain_u);
        db<void>(TRC) << "rt_vcpu_conf: " << rt_vcpu_conf << endl;
        db<void>(TRC) << "rt_vcpu_conf system object: " << reinterpret_cast<void *>(rt_vcpu_conf.__stub()->id().unit()) << endl;

        Periodic_Thread * rt_vcpu = new Periodic_Thread(rt_vcpu_conf, guest_os_task);
        db<void>(WRN) << "rt_vcpu system object: " << reinterpret_cast<void *>(rt_vcpu->__stub()->id().unit()) << endl;

        db<void>(WRN) << " done" << endl;

        vcpu = reinterpret_cast<Thread *>(rt_vcpu);

    } else {
        db<void>(WRN) << "creating BE vcpu...";
        ASM("bevcpu:");
        Thread * be_vcpu = new Thread(Thread::Configuration(Thread::READY, PEDF(PEDF::APERIODIC), domain_u), guest_os_task);
        db<void>(WRN) << "done" << endl;
        vcpu = be_vcpu;
    }

    return vcpu;
}


int main(int argc, char * argv[])
{
    Network::init();

    cout << "EPOS Application Loader" << endl;

    if(!argc) {
        cout << "No extra ELF programs found in boot image. Exiting!" << endl;
        return -1;
    }
    cout << "Found ELF image of " << argc << " bytes at " << reinterpret_cast<void *>(argv) << endl;
    // db<void>(WRN) << "argv" << (reinterpret_cast<char *>(argv)) << endl;

    cout << "Creating Dom0 services: ";
    Thread * d0 = new Thread(&dom0);
    // d0 inspected at Thread::epilogue
    d0->priority(_SYS::Thread::MAIN);
    cout << "done!" << endl;

    ELF * elf = reinterpret_cast<ELF *>(reinterpret_cast<char *>(argv) + 4);
    inspect_elf(elf);

    if(!elf->valid()) {
        cout << "Application is corrupted. Exiting!" << endl;
        return -1;
    }

    Address_Space * as = Task::self()->address_space();
    inspect_as(reinterpret_cast<void *>(as->__stub()->id().unit()));
    cout << "Got Task Master address space. System object is = " << reinterpret_cast<void *>(as->__stub()->id().unit()) << endl;

    cout << "Creating code segment: ";
    Segment * cs = new Segment(elf->segment_size(0));
    inspect_cs(reinterpret_cast<void *>(cs->__stub()->id().unit()));
    cout << "Created code segment (system object) = " << reinterpret_cast<void *>(cs->__stub()->id().unit()) << endl;
    cout << "Code segment size: " << cs->size() << endl;

    CPU::Log_Addr code = as->attach(cs);
    inspect_code(&code);
    cout << "code at: " << code << endl;

    if(elf->load_segment(0, code) < 0) {
        cout << "failed! Application code segment is corrupted! Exiting!" << endl;
        return -1;
    }
    inspect_elf(elf);
    cout << "Code segment loaded from ELF" << endl;

    cout << "Creating data segment: ";
    Segment * ds = new Segment(elf->segment_size(1) + S::Traits<Application>::HEAP_SIZE);
    inspect_ds(reinterpret_cast<void *>(ds->__stub()->id().unit()));
    cout << "Created data segment (system object) = " << reinterpret_cast<void *>(ds->__stub()->id().unit()) << endl;
    cout << "Data segment size: " << ds->size() << endl;

    CPU::Log_Addr data = as->attach(ds);
    inspect_data(&data);
    cout << "data at: " << data << endl;

    if(elf->load_segment(1, data) < 0) {
        cout << "failed! Application data segment is corrupted! Exiting!" << endl;
        return -1;
    }
    inspect_elf(elf);
    cout << "Data segment loaded from ELF" << endl;

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
    cout << "Creating the new domain: ";
    int (* entry)() = CPU::Log_Addr(elf->entry());
    inspect_entry(entry);

    Task * task = new Task(0, cs, ds, entry);
    inspect_task(reinterpret_cast<void *>(task->__stub()->id().unit()));
    cout << "done!" << endl;
    cout << "Domain system object = " << reinterpret_cast<void *>(task->__stub()->id().unit()) << endl;

    cout << "Creating the new VCPU: ";
    Thread * vcpu = create_vcpu(task, entry, 1, true);
    // vcpu is inspected at Thread::epilogue
    cout << "done!" << endl;

    cout << "Setting domain's VCPU: ";
    task->main(vcpu);
    cout << "done!" << endl;

    as->detach(cs);
    as->detach(ds);

    cout << "Domains loaded! Waiting for them to finish ... " << endl;
    task->main()->join();
    cout << "All spawned processes have finished!" << endl;

    cout << "Shutting down Dom0 services:";
    delete d0;
    cout << " done!" << endl;

    cout << "Shutting down the machine!" << endl;

    cout << "This is the end, my only friend, the end!" << endl;
    cout << "Bye!" << endl;
    return 0;
}

int dom0()
{
    Port<IPC> comm(11);
    inspect_comm(reinterpret_cast<void *>(comm.__stub()->id().unit()));
    cout << "comm system object = " << reinterpret_cast<void *>(comm.__stub()->id().unit()) << endl;

    cout << "starting domain 0 task..." << endl;

    while (true) {
        S::Message message;

        unsigned int size = comm.receive(&message);
        char parms[S::Message::MAX_PARAMETERS_SIZE];
        message.in(parms);
        cout << "Dom0::received:msg=" << parms << endl;
        comm.send(&message);
    }
}
