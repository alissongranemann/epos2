// EPOS Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <utility/handler.h>
#include <utility/observer.h>
#include <synchronizer.h>

__BEGIN_SYS

class Semaphore: protected Synchronizer_Common
{
public:
    Semaphore(int v = 1);
    ~Semaphore();

    void p();
    void v();

private:
    volatile int _value;
};


// An event handler that triggers a semaphore (see handler.h)
class Semaphore_Handler: public Handler
{
public:
    Semaphore_Handler(Semaphore * h) : _handler(h) {}
    ~Semaphore_Handler() {}

    void operator()() { _handler->v(); }

private:
    Semaphore * _handler;
};


// An asynchronous observer/observed based on semaphore
typedef Concurrent_Observed Semaphore_Observed;

class Semaphore_Observer : public Concurrent_Observer
{
protected:
    Semaphore_Observer() : Concurrent_Observer(), _notify(0)
    {
    }


    void signal_notify()             
    {
       _notify.v();
    }


   void wait_notify()
   {
       _notify.p();
   }
                
private:
    Semaphore _notify;
};


__END_SYS

#endif
