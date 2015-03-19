// EPOS PC_Timer Test Program

#include <utility/queue.h>
#include <utility/buffer.h>
#include <utility/ostream.h>
#include <utility/observer.h>
#include <semaphore.h>
#include <active.h>
#include <machine.h>
#include <display.h>
#include <timer.h>
#include <chronometer.h>


#define CONCURRENT_OBSERVER 1 /* Should have the same value as in
                                 _SYS::Traits<Application>::concurrent_observer */

using namespace EPOS;

OStream cout;

static volatile unsigned int generated_interrupts = 0;
static unsigned int dropped_interrupts = 0;


class Timer_HM : public _SYS::IF<_SYS::Traits<Application>::concurrent_observer,
                           Concurrent_Observed,
                           Observed>::Result
{
public:
    typedef _SYS::IF<_SYS::Traits<Application>::concurrent_observer,
                     Concurrent_Observed,
                     Observed>::Result Base;
public:
    static Timer_HM* instance()
    {
        if (! _instance)
            _instance = new Timer_HM();

        return _instance;
    }

    ~Timer_HM()
    {
        delete _buf;
    }

protected:
    Timer_HM() : Base()
    {
        for(unsigned int i = 0; i < _SYS::Traits<Application>::num_buffers; i++)
            _buf[i] = new Buffer<char>();

        _buffer_index = 0;
        _chrono.reset();
    }

public:
    static void handler()
    {
        if (_instance && (generated_interrupts == 0))
            instance()->_chrono.start(); /* Start counting ISR time here */

        if (generated_interrupts == _SYS::Traits<Application>::iterations) {
            _SYS::IC::disable(_SYS::IC::INT_TIMER);
            /* Stop counting ISR time here
               If a interrupt is not served (dropped) is natural to
               not count such time.
            */
            instance()->_chrono.stop();

            return;
        }

        if (_instance) {
            // cout << "+";
            generated_interrupts++;
            _SYS::IC::disable(_SYS::IC::INT_TIMER);
            // cout << "1>: " << instance()->_buffer_index << endl;
            Buffer<char>* buf = instance()->_buf[(instance()->_buffer_index)++];
            instance()->_buffer_index = instance()->_buffer_index % _SYS::Traits<Application>::num_buffers;
            _SYS::IC::enable(_SYS::IC::INT_TIMER);
            // cout << "2>: " << instance()->_buffer_index << endl;
            if (buf->lock()) {
                buf->data('@');
                instance()->_produced.insert(buf->link());
                instance()->notify();
            } else {
                // "Interrupt dropped"
                // cout << "P";
                dropped_interrupts++;
            }
        }
    }

    // Remove and return head of the queue
    Buffer<char>* dequeue()
    {
        return _produced.remove()->object();
    }

    bool empty()
    {
        return _produced.empty();
    }

    _SYS::Chronometer::Microsecond total_isr_time()
    {
        return _chrono.read();
    }

private:
    static Timer_HM* _instance;

private:    
    unsigned int _buffer_index;
    Buffer<char>* _buf[_SYS::Traits<Application>::num_buffers];
    Atomic_Queue<Buffer<char> > _produced;
    _SYS::Chronometer _chrono;

};

Timer_HM* Timer_HM::_instance = 0;


class Abstract_Timer_Manager
{
protected:
    Abstract_Timer_Manager()
    {
    }

protected:
    void _update(Timer_HM* observed)
    {
        if (! observed->empty()) {
            // cout << "-";
            Buffer<char>* buf = observed->dequeue();
            // cout << buf->data();
            char data = buf->data();
            if (data != '@')
                cout << "error on receiving data" << endl;
            buf->unlock();
        }
    }
};


#if CONCURRENT_OBSERVER
class Concurrent_Timer_Manager : public Abstract_Timer_Manager, public Active, public Concurrent_Observer
{
public:
    Concurrent_Timer_Manager(Dual_Handler * handler, Timer_HM * hm) : Abstract_Timer_Manager(), Active(), Concurrent_Observer(handler, hm)
    {
        hm->attach(_handler);
    }


    ~Concurrent_Timer_Manager()
    {
        Timer_HM::instance()->detach(_handler);
    }

protected:
    void do_update()
    {
        // cout << "D";
        _update((Timer_HM*) _observed);
    }


    int run()
    {
        // for(unsigned int i = 0; i< _SYS::Traits<Application>::iterations; i++)
        while(generated_interrupts < _SYS::Traits<Application>::iterations) {
            // cout << "U";
            update();
        }

        return 0;
    }
};
#else
typedef void Concurrent_Timer_Manager;
#endif


#if ! CONCURRENT_OBSERVER
class Sequential_Timer_Manager : public Abstract_Timer_Manager, public Observer
{
public:
    Sequential_Timer_Manager(Dual_Handler * handler, Timer_HM * hm) : Abstract_Timer_Manager(), Observer()
    {
        hm->attach(this);
    }


    ~Sequential_Timer_Manager()
    {
        Timer_HM::instance()->detach(this);
    }


    void start()
    {
    }

    void join()
    {
        while(generated_interrupts < _SYS::Traits<Application>::iterations) {
            // cout << "J";
        }
        // while(true);
    }

protected:
    void update(Observed* observed)
    {
        _update((Timer_HM*) observed);
    }
};
#else
typedef void Sequential_Timer_Manager;
#endif


class Timer_Manager : public _SYS::IF<_SYS::Traits<Application>::concurrent_observer,
                           Concurrent_Timer_Manager,
                           Sequential_Timer_Manager>::Result
{
public:
    typedef _SYS::IF<_SYS::Traits<Application>::concurrent_observer,
                     Concurrent_Timer_Manager,
                     Sequential_Timer_Manager>::Result Base;
public:
    Timer_Manager(Dual_Handler * handler, Timer_HM * hm) : Base(handler, hm)
    {
    }

    ~Timer_Manager()
    {
    }
};


int main()
{
    if (_SYS::Traits<Application>::concurrent_observer)
        cout << "PC_Timer test concurrent" << endl;
    else
        cout << "PC_Timer test sequential" << endl;

    Semaphore * sem = new Semaphore(0);
    Dual_Handler * sem_handler = new Semaphore_Handler(sem);
    User_Timer timer(10000, Timer_HM::handler);

    Timer_Manager* tm = new Timer_Manager(sem_handler, Timer_HM::instance());
    tm->start();
    tm->join();

    delete tm;
    delete sem_handler;
    delete sem;

    cout << endl << "Set to generate: " << _SYS::Traits<Application>::iterations << " interrupts" << endl;
    cout << generated_interrupts << " interrupts were generated" << endl;
    cout << dropped_interrupts << " interrupts were dropped" << endl;
    cout << "Total ISR time: " << Timer_HM::instance()->total_isr_time()  << " us" << endl;
    cout << "The End!" << endl;

    return 0;
}
