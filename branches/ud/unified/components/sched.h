// EPOS Scheduler Abstraction Declarations

#ifndef __sched_unified_h
#define __sched_unified_h

#include "component.h"

#include "../utility/maybe.h"
#include "../utility/list.h"
#include "../framework/meta.h"

using Implementation::List_Elements::Doubly_Linked_Scheduling_Value;

namespace Implementation {

namespace Scheduling_Criteria
{
    // Priority (static and dynamic)
    class Priority
    {
    public:
    enum {
        MAIN   = 0,
        HIGH   = 1,
        NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
        LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
        IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
    };

    static const bool timed = false;
    static const bool preemptive = true;
    static const bool energy_aware = false;

    static const unsigned int QUEUES = 1;

    public:
    Priority(int p = NORMAL): _priority(p) {}

    operator const volatile int() const volatile { return _priority; }

    protected:
    volatile int _priority;
    };

    // Round-Robin
    class Round_Robin: public Priority
    {
    public:
    enum {
        MAIN   = 0,
        NORMAL = 1,
        IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
    };

    static const bool timed = true;
    static const bool preemptive = true;
    static const bool energy_aware = false;

    public:
    Round_Robin(int p = NORMAL): Priority(p) {}
    };

};

// Scheduling_Queue
template <typename T, typename Criterion, unsigned int Q>
class Sched_Queue
{
private:
    typedef Doubly_Linked_Scheduling_Value<T, Criterion> Element_Type;
    typedef Scheduling_List<T, Criterion, Element_Type> Queue;

public:
    typedef typename Queue::Element Element;

public:
    Sched_Queue() {}

    unsigned int size() { return _ready[Criterion::current()].size(); }

    Maybe<Element*> volatile chosen() {
        return _ready[Criterion::current()].chosen();
    }

    void insert(Element * e) {
        _ready[e->rank().queue()].insert(e);
    }

    Maybe<Element*> remove(Element * e) {
        // removing object instead of element forces a search and renders
        // removing inexistent objects harmless
        return _ready[e->rank().queue()].remove(e->object());
    }

    Maybe<Element*> choose() {
        return _ready[Criterion::current()].choose();
    }

    Maybe<Element*> choose_another() {
        return _ready[Criterion::current()].choose_another();
    }

    Maybe<Element*> choose(Element * e) {
        return _ready[e->rank().queue()].choose(e->object());
    }

private:
    Queue _ready[Q];
};

// Specialization for single-queue
template <typename T, typename Criterion>
class Sched_Queue<T, Criterion, 1>: public Scheduling_List<T, Criterion, Doubly_Linked_Scheduling_Value<T, Criterion> >
{
private:
    typedef Scheduling_List<T, Criterion, Doubly_Linked_Scheduling_Value<T, Criterion> > Base;

public:
    typedef typename Base::Element Element;

public:
    Maybe<Element*> remove(Element * e) {
        if (e) {
            Element* tmp = Base::remove(e);
            return Maybe<Element*>(tmp, tmp != 0);
        }
        else {
            return Maybe<Element*>();
        }
    }

    Maybe<Element*> choose() {
        return Base::choose();
    }

    Maybe<Element*> choose(Element * e) {
        return Base::choose(e->object());
    }
};


// Scheduler
// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling queue sorting criterion (viz, through 
// operators <, >, and ==) and must also define a method "link" to export the
// list element pointing to the object being handled.
//
// The second parameter is the allocator. The allocator is used to allocate the
// links used in the Scheduling_Queue.
template <class T, class Criterion = typename T::Criterion>
class Sched: public Component, public Sched_Queue<T, Criterion, Criterion::QUEUES>
{
private:
    typedef Sched_Queue<T, Criterion, Criterion::QUEUES> Base;

public:
    typedef typename Base::Element Element;

public:
    enum{
        OP_UPDATE_RANK,
        OP_GET_RANK,
        OP_SCHEDULABLES,
        OP_CHOSEN,
        OP_INSERT,
        OP_REMOVE,
        OP_SUSPEND,
        OP_RESUME,
        OP_CHOOSE,
        OP_CHOOSE_ANOTHER,
        OP_CHOOSE_link,
    };


public:
    Sched(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<Sched<void,void> >::n_ids])
        :Component(rx_ch, tx_ch, iid[0]),
         Base() {}


    void update_rank(Element* link, int rank) {
        link->rank(rank);
    }

    int get_rank(Element* link) {
        return link->rank();
    }

    unsigned int schedulables() { return Base::size(); }

    T chosen() {
        return Base::chosen().get()->object();
    }

    void insert(Element* link) {
        Base::insert(link);
    }

    Maybe<Element*> remove(Element* link) {
        return Base::remove(link);
    }

    void suspend(Element* link) {
        Base::remove(link);
    }

    void resume(Element* link) {
        Base::insert(link);
    }

    T choose() {
        T obj = Base::choose().get()->object();
        return obj;
    }

    T choose_another() {
        T obj = Base::choose_another().get()->object();
        return obj;
    }

    Maybe<Element*> choose(Element* link) {
        return Base::choose(link);
    }
};

};

#endif
