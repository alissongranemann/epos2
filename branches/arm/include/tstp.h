// EPOS TSTP Common Package

#ifndef __tstp_h
#define __tstp_h

#include <utility/hash.h>
#include <utility/handler.h>
#include <alarm.h>

#include <ieee1451_0.h>
#include <tstp_api.h>
#include <tstp_mac.h>

__BEGIN_SYS

class TSTP : public TSTP_API
{
    friend class TSTP_MAC;

public:
    typedef TSTP_API API;
    typedef IEEE1451_0::UNIT_CODE UNIT_CODE;

    /*
    template<typename Data_Type>
    class Timestamped_Data {
    public:
        template<typename T>
        virtual void set(const T & rhs, const Time & t) { _data = rhs; _last_update = t; }
        Data_Type get(const Time & now) { now = _last_update; return _data; }
        Data_Type get() { return _data; }
    protected:
        Data_Type _data;
        Time _last_update;
    };

    template<typename Data_Type>
    class Synchronous_Data : public Timestamped_Data<Data_Type> {
    public:
        Synchronous_Data() : _semaphore(0) { }

        template<typename T>
        virtual void set(const T & rhs, const Time & t) { 
            _data = rhs; 
            _last_update = t;
            _semaphore.v(); 
        }

        Data_Type get() { 
            _semaphore.p(); 
            return _data; 
        }
    private:
        Semaphore _semaphore;
    };
        
    template<typename Data_Type>
    class Asynchronous_Data : public Timestamped_Data<Data_Type> {
    public:
        typedef Function_Handler<Asynchronous_Data<Data_Type>> Handler;

        Asynchronous_Data() : _semaphore(0) { }

        template<typename T>
        void set(const T & rhs, const Time & t) {
            _data = rhs; 
            _last_update = t;
            if(_handler)
                _handler(*this);
        }

        Data_Type get() { return _data; }
    };
    
    template<UNIT_CODE C, bool Synchronous>
    class TSTP_Data : public IF<Synchronous, Synchronous_Data<Data_Type::C>, Asynchronous_Data<Data_Type::C>> {
    public:
        static const UNIT_CODE UNIT = C;
    };

    template<bool Synchronous = false>
    class Watt : public TSTP_Data<CODE::WATT, Synchronous> { };
    */

    template<UNIT_CODE::UNIT_CODE_T C = UNIT_CODE::METER>
    struct Data_Type_By_Code { typedef unsigned int Type; }; // TODO

    template<UNIT_CODE::UNIT_CODE_T C>
    class TSTP_Data {
    public:
        static const UNIT_CODE::UNIT_CODE_T UNIT = C;
        typedef typename Data_Type_By_Code<C>::Type Data_Type;
        template<typename T = Data_Type>
        T * data() { return reinterpret_cast<T*>(_data); }
    private:
        Data_Type _data;
    };

    typedef TSTP_Data<UNIT_CODE::METER> Meter;
    typedef TSTP_Data<UNIT_CODE::WATT> Watt;

    class Interest : public Interest_Message {
    public:
        typedef void (Handler)(Interest *);

        template<typename DATA>
        Interest(TSTP * tstp, DATA * data, const Remote_Address & destination, const Time & t0, const Time & t_end,
                const Time & period, const Error & max_error, const RESPONSE_MODE & response_mode, const Handler * update_handler = 0, const Handler * period_handler = 0) :
            Interest_Message(_router->my_address(), t0, destination, t0, t_end, period, DATA::UNIT, response_mode, max_error),
            _data(data->data()), _last_update(0), _update_functor(0), _period_functor(0), _period_handler(period_handler), _alarm(0)
        {
            if(update_handler)
                _update_functor = new Functor_Handler<Interest>(update_handler, this);
            if(period_handler) {
                _period_functor = new Functor_Handler<Interest>(set_alarm, this);
                _alarm = new Alarm(t0 + period - _tstp->time(), _period_functor, 1);                
            }
            _tstp->add_interest(this);
        }

        ~Interest() {
            if(_alarm) delete _alarm; 
            if(_update_functor) delete _update_functor;
            if(_period_functor) delete _period_functor;
            _tstp->remove_interest(this);
        }

        const TSTP & tstp() { return *_tstp; }

        template<typename T> 
        const T& data() { return reinterpret_cast<T>(_data); }

        template<typename T> 
        const T& data(Time * last_update_time) { *last_update_time = _last_update; return reinterpret_cast<T>(_data); }

        Time last_update() { return _last_update; }

    private:
        template<typename T>
        void update(T * data, const Time & time) {
            *_data = *data;
            _last_update = time;
        }

        static void set_alarm(Interest * i) {
            if(i->_alarm) delete i->_alarm;
            if(i->_period_functor) delete i->_period_functor;
            i->_period_functor = new Functor_Handler<Interest>(i->_period_handler, i);
            i->_alarm = new Alarm(i->period(), i->_period_functor, Alarm::INFINITE); // TODO: Alarm times
        }

        Data_Type_By_Code<> * _data; // TODO
        Time _last_update;
        Functor_Handler<Interest> * _update_functor;
        Functor_Handler<Interest> * _period_functor;
        Handler * _period_handler;
        Alarm * _alarm;
        TSTP * _tstp;
    };

    class Sensor : public Data_Message {
        friend class TSTP;
    public:

        typedef void (Handler)(Sensor *);

        template<typename DATA>
        Sensor(const TSTP * tstp, const DATA * data, const Error & error, const Time & time_to_measure, const Time & time_between_measurements, const Handler * measure = 0) : 
            Data_Message(DATA::UNIT), _data(data->data()), _call_handler(measure), _handler(measure, this), _error(error), _time_to_measure(time_to_measure), _cooldown(time_between_measurements), _last_measurement(0), _tstp(tstp)
        { 
            _tstp->add_sensor(this);
        }

        ~Sensor() { _tstp->remove_sensor(this); }

        const TSTP & tstp() { return *_tstp; }

        template<typename T> 
        const T* data() { return reinterpret_cast<T*>(_data); }

        const Time & time_to_measure() const { return _time_to_measure; }
        const Time & cooldown() const { return _cooldown; }
        const Time & last_measurement() const { return _last_measurement; }
        const Error & error() const { return _error; }

    private:
        unsigned int update() {
            if(_call_handler) {
                auto now = _tstp->time();
                if(now - _last_measurement < _cooldown) {
                    return 0;
                }
                _handler();
                _last_measurement = now;
            }
            auto ret = sizeof(Data_Type_By_Code<>); // TODO
            memcpy(Data_Message::_data, _data, ret); 
            return ret;
        }

        Data_Type_By_Code<> * _data; // TODO
        bool _call_handler;
        Functor_Handler<Sensor> _handler;
        Error _error;
        Time _time_to_measure;
        Time _cooldown;
        Time _last_measurement;
        TSTP * _tstp;
    };

private:
    TSTP(MAC * mac, Time_Manager * time, Router * router, Security * security, unsigned int unit) : _mac(mac), _time(time), _router(router), _security(security) { 
        _mac->_tstp = this;
        _time->_tstp = this;
        _router->_tstp = this;
        _security->_tstp = this;
    }

public:
    Time time() { return _time->time_now(); }

    static void init(unsigned int unit);

private:
    void add_interest(Interest * i) { } // TODO
    void remove_interest(Interest * i) { } // TODO    
    void add_sensor(Sensor * s) { } // TODO
    void remove_sensor(Sensor * s) { } // TODO    

    TSTP();

    MAC * _mac;
    Time_Manager * _time;
    Router * _router;
    Security * _security;

    static TSTP * _network[Traits<NIC>::UNITS];

    // == TSTP_MAC -> TSTP interface ==
    void failed(Scheduled_Message * message);
};

__END_SYS

#endif
