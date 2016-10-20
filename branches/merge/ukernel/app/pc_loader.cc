// EPOS ELF Loader Program

#include <utility/ostream.h>
#include <utility/elf.h>
#include <machine.h>
#include <system/info.h>
#include <segment.h>
#include <thread.h>
#include <task.h>
#include <communicator.h>

using namespace EPOS;

int dom0();

OStream cout;

int main(int argc, char * argv[])
{
    cout << "EPOS Application Loader" << endl;

    if(!argc) {
        cout << "No extra ELF programs found in boot image. Exiting!" << endl;
        return -1;
    }
    cout << "Found ELF image of " << argc << " bytes at " << reinterpret_cast<void *>(argv) << endl;

    cout << "Creating Dom0 services: ";
    Thread * d0 = new Thread(&dom0);
    d0->priority(_SYS::Thread::MAIN);
    cout << "done!" << endl;

    ELF * elf = reinterpret_cast<ELF *>(reinterpret_cast<char *>(argv) + 4);
    if(!elf->valid()) {
        cout << "Application is corrupted. Exiting!" << endl;
        return -1;
    }

    Address_Space * as = Task::self()->address_space();

    cout << "Creating code segment: ";
    Segment * cs = new Segment(elf->segment_size(0));
    CPU::Log_Addr code = as->attach(cs);
    if(elf->load_segment(0, code) < 0) {
        cout << "failed! Application code segment is corrupted! Exiting!" << endl;
        return -1;
    }
    unsigned long code_log_addr = code;
    Address_Space::set_as_read_only(code_log_addr, cs->size());
    as->detach(cs);
    cout << "done!" << endl;

    cout << "Creating data segment: ";
    Segment * ds = new Segment(elf->segment_size(1) + S::Traits<Application>::HEAP_SIZE);
    CPU::Log_Addr data = as->attach(ds);
    if(elf->load_segment(1, data) < 0) {
        cout << "failed! Application data segment is corrupted! Exiting!" << endl;
        return -1;
    }
    as->detach(ds);
    cout << "done!" << endl;

    cout << "Creating the new task: ";
    int (* entry)() = CPU::Log_Addr(elf->entry());
    Task * task = new Task(cs, ds, entry);
    cout << "done!" << endl;

    cout << "Processes loaded! Waiting for them to finish ... " << endl;
    task->main()->join();
    cout << "All spawned processes have finished!" << endl;

    cout << "Shutting down Dom0 services:";
    delete d0;
    cout << " done!" << endl;

    cout << "Shutting down the machined!" << endl;
    return 0;
}

int dom0()
{
    Port<IPC> comm(11);

    while(1) {
        S::Message message;

        unsigned int size = comm.receive(&message);
        char parms[S::Message::MAX_PARAMETERS_SIZE];
        message.in(parms);
        cout << "Dom0::received:msg=" << parms << endl;
        comm.send(&message);
    }
}
