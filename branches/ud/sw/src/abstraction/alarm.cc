// EPOS Alarm Abstraction Implementation

#include <semaphore.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
Alarm_Timer * Alarm::_timer;
volatile Alarm::Tick Alarm::_elapsed;
Alarm::Queue Alarm::_request;


// Methods
Alarm::Alarm(const Microsecond & time, Handler * handler, int times)
: _ticks(ticks(time)), _handler(handler), _times(times), _link(this, _ticks)
{
    lock();

    db<Alarm>(TRC) << "Alarm(t=" << time
                   << ",tk=" << _ticks
                   << ",h=" << reinterpret_cast<void *>(handler)
                   << ",x=" << times << ") => " << this << endl;

    if(_ticks) {
        _request.insert(&_link);
        unlock();
		kout << "Alarm inserting new request: " << this << endl;
    } else {
        unlock();
        (*handler)();
    }
}


Alarm::~Alarm()
{
    lock();

    db<Alarm>(TRC) << "~Alarm(this=" << this << ")" << endl;

    _request.remove(this);

    unlock();
}


// Class methods
void Alarm::delay(const Microsecond & time)
{
    db<Alarm>(TRC) << "Alarm::delay(time=" << time << ")" << endl;

    Semaphore semaphore(0);
    Semaphore_Handler handler(&semaphore);
    Alarm alarm(time, &handler, 1); // if time < tick trigger v()
    semaphore.p();
}

void Alarm::handler()
{
    lock();

    _elapsed++;
	if(!(_elapsed%1000))	kout<<"_e: " << _elapsed<<endl;

    if(Traits<Alarm>::visible) {
		kout << "Alarm Visible:\n";
        Display display;
        int lin, col;
        display.position(&lin, &col);
        display.position(0, 79);
        display.putc(_elapsed);
        display.position(lin, col);
    }

    Alarm * alarm = 0;

	if(!_request.empty())
	{
		if(_request.head()->promote() <= 0)// rank can be negative whenever
		{                                  // multiple handlers get created
										   // for the same time tick
			Queue::Element * e = _request.remove();
			alarm = e->object();
			if((alarm->_times == 0) || (alarm->_handler==0) )
				kout << "Alarm PROBLEM: _times: " << alarm->_times << " _handler: " << alarm->_handler << endl;

			if(alarm->_times != INFINITE)
				alarm->_times--;

			if(alarm->_times) {
				e->rank(alarm->_ticks);
				_request.insert(e);
			}
			unlock();

			if(alarm) {
				/*db<Alarm>(TRC) << "Alarm::handler(this=" << alarm << ",e=" << _elapsed << ",h="
				  << reinterpret_cast<void*>(alarm->handler)
				  << ")" << endl;*/
				(*alarm->_handler)();
			}

			lock();
		}
	}
	unlock();
}

__END_SYS
