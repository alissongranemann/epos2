// EPOS Application Loader Abstraction Declarations

#ifndef __application_loader_h
#define __application_loader_h

// #include <system/config.h>

// __BEGIN_SYS

class Application_Loader // <<singleton>>
{
public:
    static Application_Loader * instance()
    {
        if (! _instance)
            _instance = new Application_Loader();

        return _instance;
    }

public:
    /*! Loads applications that belong to ELF images starting from the application extra segment.
     * For each ELF file loaded (i.e. each application) is created a task and a thread for it.
     * Returns the number of loaded applications or -1 in case there is a corrupted ELF file.
     */
    int load_applications();

private:
    Application_Loader()
    {
    }

private:
    static Application_Loader * _instance;
};

// __END_SYS

#endif
