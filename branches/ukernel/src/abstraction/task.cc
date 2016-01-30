// EPOS Task Abstraction Implementation

#include <task.h>

__BEGIN_SYS

// Class attributes
Task * volatile Task::_current;

// Methods
Task::~Task()
{
    db<Task>(TRC) << "~Task(this=" << this << ")" << endl;

    while(!_threads.empty())
        delete _threads.remove()->object();

    delete _as;
}


void Task::constructor_1_prologue(Address_Space * as, Segment * cs, Segment * ds)
{
}

void Task::constructor_2_prologue(Segment * cs, Segment * ds)
{
}

void Task::constructor_3_prologue(Segment * cs, Segment * ds)
{
}

void Task::constructor_4_prologue(Segment * cs, Segment * ds)
{
}

__END_SYS
