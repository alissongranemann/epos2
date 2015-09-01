// EPOS Component Software Aspect Program

#ifndef __software_h
#define __software_h

#include <system/config.h>
#include <framework/message.h>

extern "C" { int _syscall(void *); }

__BEGIN_SYS

class Message_Software: public Message_Common
{
public:
    Message_Software(const Id & id): Message_Common(id) {}

    template<typename ... Tn>
    void in(Tn && ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        DESERIALIZE(_parms, index, an ...);
    }
    template<typename ... Tn>
    void out(const Tn & ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        SERIALIZE(_parms, index, an ...);
    }

    template<typename ... Tn>
    int act(const Method & m, const Tn & ... an) {
        _method = m;
        out(an ...);
        _syscall(this);
        return _method;
    }

    friend Debug & operator << (Debug & db, const Message_Software & m) {
        db << "{id=" << m._id << ",m=" << hex << m._method
            << ",p={" << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[0]))) << ","
            << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[4]))) << ","
            << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[8]))) << "}}";
        return db;
    }

protected:
    char _parms[MAX_PARAMETERS_SIZE];
};

template<typename Component>
class Software
{
protected:
    Software() {}

public:
    void enter() { db<Aspect>(TRC) << "Software::enter()" << endl; }
    void leave() { db<Aspect>(TRC) << "Software::leave()" << endl; }

    static void static_enter() { db<Aspect>(TRC) << "Software::static_enter()" << endl; }
    static void static_leave() { db<Aspect>(TRC) << "Software::static_leave()" << endl; }
};

__END_SYS

#endif
