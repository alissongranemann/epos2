// EPOS Component Hardware Aspect Program

#ifndef __hardware_h
#define __hardware_h

#include <system/config.h>

__BEGIN_SYS

template<typename Component>
class Hardware
{
protected:
    Hardware() {}

public:
    void enter() { db<Aspect>(TRC) << "Hardware::enter()" << endl; }
    void leave() { db<Aspect>(TRC) << "Hardware::leave()" << endl; }

    static void static_enter() { db<Aspect>(TRC) << "Hardware::static_enter()" << endl; }
    static void static_leave() { db<Aspect>(TRC) << "Hardware::static_leave()" << endl; }
};

__END_SYS

#endif

