// EPOS Application Loader

// #include <system/boot_image.h>
// #include <thread.h>
// #include <task.h>

#include "application_loader.h"

using namespace EPOS;

static Thread * threads[2];

OStream cout;
OStream cerr;

// Class attributes
Application_Loader * Application_Loader::_instance = 0; /// <<singleton>>


// Methods
int Application_Loader::load_applications()
{
    db<Application_Loader>(ERR) << "Application_Loader::load_applications" << endl;

    unsigned int loaded_apps = 0;

    Boot_Image * bi = Boot_Image::self();
    db<Application_Loader>(ERR) << "Boot image retrieved..." << endl;

    User_Space_ELF * app_elf = bi->next_extra_elf();
    db<Application_Loader>(ERR) << "app_elf retrieved..." << endl;

    if (! app_elf) {
        db<Application_Loader>(ERR) << "There is no other application: bye" << endl;
        return -1;
    }

    /*
    if(! app_elf->valid()) {
        db<Application_Loader>(ERR) << "ELF image is corrupted!" << endl;
        return -1;
    }
    */

    const Task * task0 = Task::self();
    Address_Space * as0 = task0->address_space();


    do {
        // Load application (one code segment, one or more data segments)
        db<Application_Loader>(ERR) << "loading app..." << endl;
       
        db<Application_Loader>(ERR) << "Will create a code segment " << app_elf->segment_size(0) << " bytes long." << endl;  
        // Segment * code_segment = new Segment(CPU::Log_Addr(app_elf->segment_address(0)), app_elf->segment_size(0), (int) _SYS::Segment::Flags::APP); // NOTE: generating garbage here
        Segment * code_segment = new Segment(app_elf->segment_size(0)); // NOTE: generating garbage here
        db<Application_Loader>(ERR) << "code segment is located at (physical) " << code_segment->phy_address() << " and it is " << code_segment->size() << " bytes long" << endl;

        db<Application_Loader>(ERR) << "Will create a data segment " << app_elf->segment_size(1) << " bytes long." << endl;  
        // Segment * data_segment = new Segment(CPU::Log_Addr(app_elf->segment_address(1)), app_elf->segment_size(1), (int) _SYS::Segment::Flags::APP); /* NOTE: For now, ignoring all application data segments but the first.*/ // NOTE: generating garbage here
        Segment * data_segment = new Segment(app_elf->segment_size(1)); /* NOTE: For now, ignoring all application data segments but the first.*/ // NOTE: generating garbage here
        db<Application_Loader>(ERR) << "data segment is located at (physical) " << data_segment->phy_address() << " and it is " << data_segment->size() << " bytes long" << endl;

        db<Task>(ERR) << "Attaching segments!" << endl;
        CPU::Log_Addr code = as0->attach(*code_segment);
        cout << "  code => " << code << " done!" << endl;

        CPU::Log_Addr data = as0->attach(*data_segment);
        cout << "  data => " << data << " done!" << endl;
        
        
        db<Task>(ERR) << "will load code segment!" << endl;
        if(app_elf->load_segment(0, code) < 0) {                                          
            db<Task>(ERR) << "Task code segment was corrupted!" << endl;
            // return -1;
        }
        db<Task>(ERR) << "...code segment loaded" << endl;

        for(int i = 1; i < app_elf->segments(); i++) {
            if(app_elf->load_segment(i, data) < 0) {
                db<Task>(ERR) << "Task data segment was corrupted!" << endl;
                // return -1;
            }
            db<Task>(ERR) << "...data segment loaded" << endl;
        }

        db<Application_Loader>(ERR) << "Detaching segments:";
        as0->detach(*code_segment);
        as0->detach(*data_segment);
        db<Application_Loader>(ERR) << " done!" << endl;

        Task * task = new Task(*code_segment, *data_segment); // NOTE: generating garbage here
        db<Application_Loader>(ERR) << "task created" << endl;

        int (* entry)();
        // entry = reinterpret_cast<int (*)()>(app_elf->entry());
        entry = reinterpret_cast<int (*)()>(0x000001d0); // ISSUE #2: Application_Loader::load_applications:> Thead entrypoint is hardcoded.
        threads[loaded_apps] = new Thread(*task, entry);
        db<Application_Loader>(ERR) << "thread created" << endl;
            
        loaded_apps++;
        
        if (loaded_apps < 2) // ISSUE #3: Application_Loader::load_applications:> Number of loaded appplications is fixed (and hardcoded)
            app_elf = bi->next_extra_elf();
    } while (app_elf  && loaded_apps < 2); // ISSUE #3.
    
    return loaded_apps;
}


int main()
{
    cout << "This is Application Loader" << endl;

    Application_Loader * app_loader = Application_Loader::instance();
    int loaded_apps = app_loader->load_applications();

    if (loaded_apps <= 0) {
        cerr << "No application loaded: bye" << endl;
        return -1;
    }

    cout << "Loaded: " << loaded_apps << " applications" << endl;
    int status_a = threads[0]->join();
    int status_b = threads[1]->join();
    // Thread::yield();
    cout << "Thread A exited with status " << status_a << " and thread B exited with status " << status_b << endl;
    // cout << "Now I will enter in an infinite loop!" << endl;
    // while(true); /* NOTE: might choose to join all threads instead. */

    // cerr << "UNREACHABLE!" << endl;
    cerr << "Bye!" << endl;

    return 0;
}
