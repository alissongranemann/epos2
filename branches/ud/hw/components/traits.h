// EPOS Configuration

//FIXME should this file location mirror the software tree ?

#ifndef __traits_h
#define __traits_h

#include "system/types.h"
#include "../../unified/traits.h"

namespace System {

template <class Imp>
struct Traits : public Implementation::Traits<Imp> {};


// Utilities
//...


// System parts
//...


// Common Mediators 
//...


// Services
//...


// Abstractions
template <> struct Traits<Thread>: public Traits<void>
{
    typedef Implementation::Scheduling_Criteria::Priority Criterion;
    static const bool smp = false;
};

template <> struct Traits<Implementation::Sched<Thread, Traits<Thread>::Criterion> > :  public Implementation::Traits<Implementation::Sched<void,void> >
{
};


}

#endif
