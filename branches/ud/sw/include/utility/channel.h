// Semaphore implementation with busy waiting

#ifndef __cm_semaphore_h
#define __cm_semaphore_h

#include <cpu.h>

__BEGIN_SYS

template<int MAX_THREADS>
class Busy_Semaphore
{
public:
    Busy_Semaphore(int v = 1) : _value(v) {
        for (int i = 0; i < MAX_THREADS; ++i) _queue[i] = false;
    }

    ~Busy_Semaphore() {

    }

    void p() {
        //kout << "CM_p_in\n";
        bool was = lock();

        int aux = 0;
        if((aux = fdec(_value)) < 1){
            unsigned int idx = abs(aux);
            _queue[idx] = true; while(_queue[idx]);
        }

        unlock(was);
        //kout << "CM_p_out\n";
    }

    void v() {
        //kout << "CM_v_in\n";
        bool was = lock();

        if(finc(_value) < 0){
            unsigned int idx = abs(_value);
            _queue[idx] = false;
        }

        unlock(was);
        //kout << "CM_v_out\n";
    }

private:
    int finc(volatile int & number) { return CPU_Common::finc(number); }
    int fdec(volatile int & number) { return CPU_Common::fdec(number); }
    unsigned int abs(int val){
        return (unsigned int)((val < 0) ? (val*(-1)) : val);
    }

    bool lock(){
        bool was = CPU::int_enabled();
        CPU::int_disable();
        return was;
    }

    void unlock(bool was){
        if (was) CPU::int_enable();
    }

private:
    volatile int _value;
    volatile bool _queue[MAX_THREADS];
};

template<typename T, int BUFFER_SIZE>
class Blocking_Channel: private Busy_Semaphore<BUFFER_SIZE>
{
public:
    Blocking_Channel() : Busy_Semaphore<BUFFER_SIZE>(0), _begin(0), _end(0) {

    }

    ~Blocking_Channel() {

    }

    T& read() {
        //kout << "CMC_p_in\n";
        Busy_Semaphore<BUFFER_SIZE>::p();
        //kout << "CMC_p_out\n";
        return _buffer[turn_around(_begin)];
    }

    void write(const T &value) {
        //kout << "CMC_v_in\n";
        _buffer[turn_around(_end)] = value;
        if(_end == _begin) db<Synchronizer>(WRN) << "CM_Semaphore_Channel<"<< BUFFER_SIZE <<">::v(this=" << this << ", (_end==_begin)=" << _begin << ") Buffer overflow\n";
        Busy_Semaphore<BUFFER_SIZE>::v();
        //kout << "CMC_v_out\n";
    }

private:
    int _begin;
    int _end;

    int turn_around(int &val){
        int old = val;
        val = (val + 1) % BUFFER_SIZE;
        return old;
    }

    T  _buffer[BUFFER_SIZE];
};

__END_SYS

#endif
