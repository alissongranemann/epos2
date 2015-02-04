// EPOS Scheduler Abstraction Declarations

#ifndef __sched_h
#define __sched_h

/*
#include <aspects/dispatcher.h>
#include <aspects/stub.h>
#include <aspects/adapter.h>
#include "thread.h"
#include "../../components/implementation/sched.h"


namespace Implementation {
//TODO these definitions are workarounds. Get rid of them

typedef Sched<System::Thread*,System::Thread::Criterion> Sched_Thread;

template<> class Resource_Table<Sched_Thread,0> : public Resource_Table<Sched<void,void>,0> {};

template<> class Resource_Table_Array<Sched_Thread> : public Resource_Table_Array<Sched<void,void> > {};

};

__BEGIN_SYS

template<>
class Scenario_Adapter<Implementation::Sched<Thread*,Thread::Criterion> > :
                        public Implementation::Sched<Thread*,Thread::Criterion>,
                        public SW_Scenario<
                                   Implementation::Sched<Thread*,Thread::Criterion>,
                                   Static_Allocation<Implementation::Sched<Thread*,Thread::Criterion>::Element, unsigned int, Traits<Implementation::Sched<Thread*,Thread::Criterion> >::queue_size> > {
public:
    typedef Thread* T;
    typedef Thread::Criterion Criterion;
    typedef Implementation::Sched<T,Thread::Criterion> Component;
    typedef Component::Element Element;
    typedef SW_Scenario<
                Component,
                Static_Allocation<Element, unsigned int, Traits<Component>::queue_size> >
            Scenario;

    typedef Scenario::Idx_Type Link;

public:
    Scenario_Adapter(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char _iid[Traits<Component>::n_ids])
        :Component(rx_ch,tx_ch,_iid),
         Scenario(rx_ch,tx_ch,_iid)
    {

    }

public:
    void update_rank(Link link, int rank) {
        Component::update_rank(Scenario::get(link), rank);
    }

    int get_rank(Link link) {
        return Component::get_rank(Scenario::get(link));
    }


    unsigned int schedulables() { return Component::schedulables(); }

    T chosen() {
        return Component::chosen();
    }

    Link insert(T obj, Criterion rank) {
        Link link;
        Scenario::allocate(link, obj, rank);
        Component::insert(Scenario::get(link));
        return link;
    }

    T remove(Link link) {
        Maybe<Element*> elem = Component::remove(Scenario::get(link));
        T tmp = elem.exists() ? elem.get(Scenario::null())->object() : (T)0;
        Scenario::free(link);
        return tmp;
    }

    void suspend(Link link) {
        Component::suspend(Scenario::get(link));
    }

    void resume(Link link) {
        Component::resume(Scenario::get(link));
    }

    T choose() {
        return Component::choose();
    }

    T choose_another() {
        return Component::choose_another();
    }

    T choose(Link link) {
        Maybe<Element*> elem = Component::choose(Scenario::get(link));
        return elem.exists() ? elem.get(Scenario::null())->object() : (T)0;
    }

};

DISPATCHER_BEGIN(Sched_Thread,2)
    DISPATCH_BEGIN
        CALL_2(update_rank, Implementation::Sched_Thread::OP_UPDATE_RANK, Component::Link, int)
        CALL_R_1(get_rank, Implementation::Sched_Thread::OP_GET_RANK, int, Component::Link)
        CALL_R_0(schedulables, Implementation::Sched_Thread::OP_SCHEDULABLES,unsigned int)
        CALL_R_0(chosen, Implementation::Sched_Thread::OP_CHOSEN,Component::T)
        CALL_R_2(insert, Implementation::Sched_Thread::OP_INSERT, Component::Link, Component::T, Component::Criterion)
        CALL_R_1(remove, Implementation::Sched_Thread::OP_REMOVE, Component::T, Component::Link)
        CALL_1(suspend, Implementation::Sched_Thread::OP_SUSPEND, Component::Link)
        CALL_1(resume, Implementation::Sched_Thread::OP_RESUME, Component::Link)
        CALL_R_0(choose, Implementation::Sched_Thread::OP_CHOOSE,Component::T)
        CALL_R_0(choose_another, Implementation::Sched_Thread::OP_CHOOSE_ANOTHER,Component::T)
        CALL_R_1(choose, Implementation::Sched_Thread::OP_CHOOSE_link,Component::T, Component::Link)
    DISPATCH_END
DISPATCHER_END

STUB_BEGIN(Sched_Thread)
    typedef Component::Link Link;
    typedef Component::T T;
    typedef Component::Criterion Criterion;

    void update_rank(Link link, int rank) {
        call<Implementation::Sched_Thread::OP_UPDATE_RANK>(link, rank);
    }
    int get_rank(Link link) {
        call_r<Implementation::Sched_Thread::OP_GET_RANK,int>(link);
    }
    unsigned int schedulables() {
        call_r<Implementation::Sched_Thread::OP_SCHEDULABLES,unsigned int>();
    }
    T chosen() {
        call_r<Implementation::Sched_Thread::OP_CHOSEN,T>();
    }
    Link insert(T obj, Criterion rank) {
        call_r<Implementation::Sched_Thread::OP_INSERT,Link>(obj, rank);
    }
    T remove(Link link) {
        call_r<Implementation::Sched_Thread::OP_REMOVE,T>(link);
    }
    void suspend(Link link) {
        call<Implementation::Sched_Thread::OP_SUSPEND>(link);
    }
    void resume(Link link) {
        call<Implementation::Sched_Thread::OP_RESUME>(link);
    }
    T choose() {
        call_r<Implementation::Sched_Thread::OP_CHOOSE,T>();
    }
    T choose_another() {
        call_r<Implementation::Sched_Thread::OP_CHOOSE_ANOTHER,T>();
    }
    T choose(Link link) {
        call_r<Implementation::Sched_Thread::OP_CHOOSE_link,T>(link);
    }
STUB_END

DEFINE_COMPONENT(Sched_Thread);


__END_SYS
*/

#endif
