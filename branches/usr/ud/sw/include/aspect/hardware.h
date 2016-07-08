// EPOS Component Hardware Aspect Program

#ifndef __hardware_h
#define __hardware_h

#include <system/config.h>
#include <framework/id.h>
#include <framework/serializer.h>
#include <component_manager.h>

__BEGIN_SYS

class Message_Hardware: public Message_Common
{
private:
    typedef Serializer::Buffer Buffer;

public:
    Message_Hardware(const Id & id): Message_Common(id) {}

    template<typename ... Tn>
    void in(Tn && ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        Serializer::deserialize(_parms, index, an ...);
    }
    template<typename ... Tn>
    void out(const Tn & ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        Serializer::serialize(_parms, index, an ...);
    }

    template<typename ... Tn>
    int act(const Method & m, const Tn & ... an) {
        int ret;

        _method = m;
        out(an ...);
        // TODO: Find a way to set the instance ID
        Component_Manager::call(_id.type(), _id.unit(), _method,
                Serializer::NPKT<Tn ...>::Result, Serializer::NPKT<int>::Result,
                _parms);
        in(ret);

        return ret;
    }

    friend Debug & operator << (Debug & db, const Message_Hardware & m) {
        db << "{id=" << m._id << ",m=" << hex << m._method
            << ",p={" << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[0]))) << ","
            << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[4]))) << ","
            << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[8]))) << "}}";
        return db;
    }

protected:
    Buffer _parms[MAX_PARAMETERS_SIZE];
};

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
