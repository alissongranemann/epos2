// EPOS Inter-Process Communication Declarations

#ifndef __ipc_h
#define __ipc_h

#include <utility/observer.h>
#include "message.h"

__BEGIN_SYS

class IPC
{
private:
    typedef Id::Type_Id Type;

public:
    static const bool connectionless = true;
    static const unsigned int HEADERS_SIZE = 0;

    // IPC addresses are ordinary Object Ids, but Type_Id will be used by Communicator as local address
    class Address: public Id
    {
    public:
        typedef Type Local;

        enum Null { NULL = Id::NULL };

    public:
        Address() {}
        Address(const Null &): Id(Id::NULL) {}
        Address(const Id & id): Id(id) {}

        Local local() const { return type(); }
    };

    typedef Message Buffer;

    typedef Data_Observer<Message, Type> Observer;
    typedef Data_Observed<Message, Type> Observed;

public:
    IPC() {}

    static int send(const Type & from, Message * m) {
        db<Framework>(TRC) << "IPC::send(from=" << from << ",msg=" << m << ")" << endl;

        Type to = m->id().type();
        Message * msg = new (SYSTEM) Message(*m); // copy from user to kernel space
        msg->id(Id(from, m->id().unit()));

        db<Framework>(INF) << "IPC::send:msg=" << &msg << " => " << msg << endl;

        unsigned int size;
        if(notify(to, msg))
            size = sizeof(Message);
        else
            size = 0;

        return size;
    }

    static int receive(Buffer * buf, Message * msg) {
        db<Framework>(TRC) << "IPC::receive(buf=" << buf << ",msg=" << msg << ")" << endl;

        new (msg) Message(*buf); // copy from kernel to user
        delete buf;

        db<Framework>(INF) << "IPC::receive:buf=" << buf << " => " << *buf << endl;
        db<Framework>(INF) << "IPC::receive:msg=" << msg << " => " << *msg << endl;

        return sizeof(Message);
    }

    static void attach(Observer * obs, const Type & type) { _observed.attach(obs, type); }
    static void detach(Observer * obs, const Type & type) { _observed.detach(obs, type); }
    static bool notify(const Type & type, Message * msg) { return _observed.notify(type, msg); }

    static Observer * observer(const Type & type, unsigned int index = 0) { return _observed.observer(type, index); }

private:
    static Observed _observed; // Channel protocols are singletons
};

template<> struct Type<Port<IPC> > { static const Type_Id ID = IPC_COMMUNICATOR_ID; };

__END_SYS

#endif
