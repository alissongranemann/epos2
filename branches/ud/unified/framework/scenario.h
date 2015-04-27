// EPOS Component Framework - Execution Scenario

// Scenario is simply an ordered collection of Aspect programs that are to be
// applied to a given component through its scenario adapter.

#ifndef __scenario_h
#define __scenario_h

// TODO: The aspects bellow must be migrated to EPOS 2.0
//#include <aspect/static_alloc.h>
//#include <aspect/dynamic_alloc.h>

__BEGIN_SYS

// TODO: Catapult doesn't support variadic templates thus we can't use the
// current TLIST implementation. Create a new implementation to allow aspects in
// hardware components.
template<typename Component>
//class Scenario: public Traits<Component>::ASPECTS::template Recur<Component>
class Scenario
{
protected:
    Scenario() {}

public:
    ~Scenario() {}

    // TODO: Not sure if those operators are supported in Catapult
    //void * operator new(size_t bytes) { return ::operator new(bytes, SYSTEM); }
    //void operator delete(void * ptr) { ::operator delete(ptr); }
};

__END_SYS

#endif
