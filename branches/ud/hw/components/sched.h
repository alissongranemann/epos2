// EPOS Scheduler Abstraction Declarations

#ifndef __sched_h
#define __sched_h

/*#include "aspects/stub.h"
#include "aspects/adapter.h"
#include "traits.h"
#include "system/types.h"
#include "../../../components/implementation/sched.h"


namespace Implementation {
//TODO these definitions are workarounds. Get rid of them

typedef Sched<System::Thread, System::Traits<System::Thread>::Criterion> Sched_Thread;

};

namespace System {

SCENARIO_ADAPTER_BEGIN(Sched_Thread,2,Implementation::Sched_Thread::Element, unsigned char, Traits<Implementation::Sched_Thread>::queue_size)
public:
    typedef Thread T;
    typedef Traits<T>::Criterion Criterion;
    typedef Component::Element Element;
    typedef Implementation::Maybe<Element*> Maybe;

    typedef Scenario::Idx_Type Link;

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
        Maybe elem = Component::remove(Scenario::get(link));
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
        Maybe elem = Component::choose(Scenario::get(link));
        return elem.exists() ? elem.get(Scenario::null())->object() : (T)0;
    }

    DISPATCH_BEGIN
        CALL_2(update_rank, Implementation::Sched_Thread::OP_UPDATE_RANK, Link, int)
        CALL_R_1(get_rank, Implementation::Sched_Thread::OP_GET_RANK, Link)
        CALL_R_0(schedulables, Implementation::Sched_Thread::OP_SCHEDULABLES)
        CALL_R_0(chosen, Implementation::Sched_Thread::OP_CHOSEN)
        CALL_R_2(insert, Implementation::Sched_Thread::OP_INSERT, T, Criterion)
        CALL_R_1(remove, Implementation::Sched_Thread::OP_REMOVE, Link)
        CALL_1(suspend, Implementation::Sched_Thread::OP_SUSPEND, Link)
        CALL_1(resume, Implementation::Sched_Thread::OP_RESUME, Link)
        CALL_R_0(choose, Implementation::Sched_Thread::OP_CHOOSE)
        CALL_R_0(choose_another, Implementation::Sched_Thread::OP_CHOOSE_ANOTHER)
        CALL_R_1(choose, Implementation::Sched_Thread::OP_CHOOSE_link, Link)
    DISPATCH_END
SCENARIO_ADAPTER_END


STUB_BEGIN(Sched_Thread,2)
    typedef Thread T;
    typedef Scenario_Adapter<Implementation::Sched_Thread>::Link Link;
    typedef Scenario_Adapter<Implementation::Sched_Thread>::Criterion Criterion;

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
        call_r<Implementation::Sched_Thread::OP_CHOSEN,Thread>();
    }
    T insert(T obj, Criterion rank) {
        call_r<Implementation::Sched_Thread::OP_INSERT,Link>(obj, rank);
    }
    T remove(Link link) {
        call_r<Implementation::Sched_Thread::OP_REMOVE,Thread>(link);
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


};*/

#endif
