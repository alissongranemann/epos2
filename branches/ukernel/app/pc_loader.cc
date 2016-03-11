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
#include <color.h>

using namespace EPOS;

typedef RTC::Microsecond Microsecond;
typedef Scheduling_Criteria::PEDF PEDF;


int dom0();
Task * create_domain(unsigned int domain_number, ELF * elf);
Thread * create_vcpu(Task * domain_u, int (* guest_os_task) (), int pcpu, bool hrt);
ELF * get_next_elf(char * elf_prt);


static const unsigned int NUM_OF_DOMAINS = _SYS::Traits<Application>::NUM_OF_DOMAINS;
static bool is_domain_hrt[NUM_OF_DOMAINS];
Task * domains[NUM_OF_DOMAINS];

OStream cout;


int main(int argc, char * argv[])
{
    // Network::init();
    is_domain_hrt[1] = _SYS::Traits<Application>::IS_DOM_1_HRT;
    is_domain_hrt[2] = _SYS::Traits<Application>::IS_DOM_2_HRT;

    cout << Color::BLUE() << "EPOS Application Loader" << Color::END_COLOR() << endl;

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

    for (unsigned int i = 1; i < NUM_OF_DOMAINS; i++)
    {
        cout << "Trying to load Domain " << i << endl;

        ELF * elf = get_next_elf(reinterpret_cast<char *>(argv));

        if(! elf) {
            cout << "Domain " << i << " could not be loaded (invalid ELF). Exiting!" << endl;
            return -1;
        }

        domains[i] = create_domain(i, elf);

        if(! domains[i]) {
            cout << "Domain " << i << " could not be loaded. Exiting!" << endl;
            return -1;
        }

        cout << "Domain " << i << " loaded!" << endl;
    }

    cout << "I will wait for all domains to finish!" << endl;
    for (unsigned int i = 1; i < NUM_OF_DOMAINS; i++)
    {
        cout << "Waiting for Domain " << i << " to finish... " << endl;
        domains[i]->main()->join();
    }

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

Task * create_domain(unsigned int domain_number, ELF * elf)
{
    inspect_elf(elf);

    if(!elf->valid()) {
        cout << "Application is corrupted. Exiting!" << endl;
        return 0;
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
        return 0;
    }
    inspect_elf(elf);
    cout << "Code segment loaded from ELF" << endl;

    cout << "Creating data segment: ";
    Segment * ds = new Segment(elf->segment_size(1));
    inspect_ds(reinterpret_cast<void *>(ds->__stub()->id().unit()));
    cout << "Created data segment (system object) = " << reinterpret_cast<void *>(ds->__stub()->id().unit()) << endl;
    cout << "Data segment size: " << ds->size() << endl;

    CPU::Log_Addr data = as->attach(ds);
    inspect_data(&data);
    cout << "data at: " << data << endl;

    if(elf->load_segment(1, data) < 0) {
        cout << "failed! Application data segment is corrupted! Exiting!" << endl;
        return 0;
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
    Thread * vcpu = create_vcpu(task, entry, domain_number, is_domain_hrt[domain_number]);
    // vcpu is inspected at Thread::epilogue
    cout << "done!" << endl;

    cout << "Setting domain's VCPU: ";
    task->main(vcpu);
    cout << "done!" << endl;

    // as->detach(cs);
    // as->detach(ds);

    return task;

}

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
        Thread * be_vcpu = new Thread(Thread::Configuration(Thread::READY, PEDF(PEDF::APERIODIC, pcpu), domain_u), guest_os_task);
        db<void>(WRN) << "done" << endl;
        vcpu = be_vcpu;
    }

    return vcpu;
}

ELF * get_next_elf(char * elf_ptr)
{
    static bool started;
    static char * next_elf;

    db<void>(WRN) << "next_elf at the beginning: " << reinterpret_cast<void *>(next_elf) << endl;

    if (! started) {
        db<void>(WRN) << "get_next_elf called for the first time" << endl;

        next_elf = elf_ptr;
        started = true;
    }

    unsigned int elf_size = *(reinterpret_cast<unsigned int *>(next_elf));
    db<void>(WRN) << "ELF size: " << elf_size << endl;

    ELF * elf = reinterpret_cast<ELF *>(reinterpret_cast<char *>(next_elf) + 4);
    db<void>(WRN) << "ELF address: " << reinterpret_cast<void *>(elf) << endl;

    next_elf = reinterpret_cast<char *>(elf) + elf_size;

    db<void>(WRN) << "next_elf at the end: " << reinterpret_cast<void *>(next_elf) << endl;

    return elf;
}



/* Domain 0 Bindings */

// Framework class attributes
__BEGIN_SYS
Framework::Cache Framework::_cache;
__END_SYS


// Global objects
__BEGIN_SYS
OStream kerr;
__END_SYS


// Bindings
extern "C" {
    void _panic() { _API::Thread::exit(-1); }
    void _exit(int s) { _API::Thread::exit(s); }


    void _cpu_int_enable()
    {
        _API::CPU::int_enable();
    }

    void _cpu_int_disable()
    {
        _API::CPU::int_disable();
    }

    unsigned int _cpu_id()
    {
        return _API::Machine::cpu_id();
    }

    void _smp_barrier()
    {
        _API::Machine::smp_barrier();
    }

    unsigned int _this_thread_id()
    {
        return _API::This_Thread::this_thread_id();
    }

}

__USING_SYS;
extern "C"
{
    void _syscall(void * m)
    {
        _SYS::CPU::syscall(m);
    }

    void _print(const char * s)
    {
        Message msg(Id(UTILITY_ID, 0), Message::PRINT, reinterpret_cast<unsigned int>(s));
        msg.act();
    }

    void _task_setup() {}

    void * _create_segment_in_place(void * place, unsigned int size, unsigned int mmu_flags)
    {
        Message msg(Id(SEGMENT_ID, 0), Message::CREATE_SEGMENT_IN_PLACE, place, size, mmu_flags);
        msg.act();
        return reinterpret_cast<void *>(msg.id().unit());
    }

    void * _create_heap_in_place(void * place, void * heap_segment)
    {
        Message msg(Id(SEGMENT_ID, 0), Message::CREATE_HEAP_IN_PLACE, place, heap_segment);
        msg.act();
        return reinterpret_cast<void *>(msg.id().unit());
    }

}

/* ---- */
