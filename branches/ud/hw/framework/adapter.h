// EPOS Component Framework - Scenario Adapter

// Scenario adapters are the heart of EPOS component framework.  They collect
// component-specific Aspect programs to build a scenario for it to run.
// Scenario features are enforced by wrapping all and any method invocation
// (event creation and destruction) within the enter() and leave() methods.

#ifndef __adapter_h
#define __adapter_h

#include "scenario.h"

__BEGIN_SYS

template<typename Component>
class Adapter: public Component, public Scenario<Component>
{
    //using Scenario<Component>::enter;
    //using Scenario<Component>::leave;
    //using Scenario<Component>::static_enter;
    //using Scenario<Component>::static_leave;

public:
    //template<typename ... Tn>
    //Adapter(const Tn & ... an): Component(an ...) { static_leave(); }
    //~Adapter() { static_enter(); }

    Adapter(): Component() {}

    //void * operator new(size_t bytes) {
        //static_enter();
        //return Scenario<Component>::operator new(bytes);
    //}
    //void operator delete(void * adapter) {
        //Scenario<Component>::operator delete(adapter);
        //static_leave();
    //}
};

__END_SYS

#endif
