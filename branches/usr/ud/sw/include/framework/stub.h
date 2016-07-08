// EPOS Component Framework - Component Stub

// Stub selectively binds the Handle either to the component's scenario Adapter or to its Proxy.
// Proxies are used in the kernel mode or when a component is subject to the Remote Aspect program.

#ifndef __stub_h
#define __stub_h

#include "adapter.h"
#include "proxy.h"

__BEGIN_SYS

template<typename Component, bool remote>
class Stub: public Adapter<Component>
{
public:
    template<typename ... Tn>
    Stub(const Tn & ... an): Adapter<Component>(an ...) {}
    ~Stub() {}
};

template<typename Component>
class Stub<Component, true>: public Proxy<Component>
{
public:
    // Dereferencing handles for Task(cs, ds)
    Stub(const Stub<Segment, true> & cs, const Stub<Segment, true> & ds): Proxy<Component>(cs.id().unit(), ds.id().unit()) {}

    // Dereferencing proxy for Thread(task, an ..., usp) and allocating a user-level stack for the thread
    template<typename ... Tn>
    Stub(const Stub<Task, true> & t, const Tn & ... an)
    : Proxy<Component>(t.id().unit(), an ..., new char[Traits<Application>::STACK_SIZE] + Traits<Application>::STACK_SIZE) {}

    template<typename ... Tn>
    Stub(const Tn & ... an): Proxy<Component>(an ...) {}

    ~Stub() {}
};

// Dereferencing proxy for Thread(an ..., usp) and allocating a user-level stack for the thread
template<>
template<typename ... Tn>
Stub<Thread, true>::Stub(const Tn & ... an)
: Proxy<Thread>(an ..., new char[Traits<Application>::STACK_SIZE] + Traits<Application>::STACK_SIZE) {}

__END_SYS

#endif
