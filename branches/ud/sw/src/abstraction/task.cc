// EPOS Task Abstraction Implementation

#include <task.h>

__BEGIN_SYS

// Class attributes
Task * Task::_master;


// Methods
Task::Task(const Segment & code, const Segment & data)
: _as (new (SYSTEM) Address_Space), _cs(&code), _ds(&data),
  _code(_as->attach(*_cs)), _data(_as->attach(*_ds))
{
    db<Task>(TRC) << "Task(cs=" << _cs << ",ds=" << _ds << ") => " << this << endl;
}


Task::~Task()
{
    db<Task>(TRC) << "~Task(this=" << this << ")" << endl;

    while(!_threads.empty())
        delete _threads.remove()->object();
}


const Task * Task::self()
{
    return Thread::self()->_task;
}

__END_SYS
