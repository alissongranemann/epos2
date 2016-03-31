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
    friend class Interest;
    friend class TSTP_MAC;
    friend class PTP;

    template<typename T> class Rescheduler;

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
        friend class TSTP;
        friend class TSTP::Rescheduler<Interest>;
    public:
        typedef void (Handler)(Interest *);

        template<typename DATA>
        Interest(TSTP * tstp, DATA * data, const Remote_Address & destination, const Time & t0, const Time & t_end,
                const Time & period, const Error & max_error, const RESPONSE_MODE & response_mode, const Handler * update_handler = 0, const Handler * period_handler = 0) :
            Interest_Message(tstp->_router->my_address(), t0, destination, t0, t_end, period, 
                    DATA::UNIT, response_mode, max_error),
            _tstp(tstp), _data(data->data()), 
            _last_update(0), _update_functor(0), _period_functor(0), _period_handler(period_handler), _rescheduler(0)
        {
            if(update_handler)
                _update_functor = new Functor_Handler<Interest>(update_handler, this);
            if(period_handler)
                _period_functor = new Functor_Handler<Interest>(period_handler, this);
            _tstp->add_interest(this);
        }

        ~Interest() {
            _tstp->remove_interest(this);
            if(_update_functor) delete _update_functor;
            if(_period_functor) delete _period_functor;
            if(_rescheduler) delete _rescheduler;
        }

        TSTP * tstp() { return _tstp; }

        template<typename T> 
        const T& data() { return reinterpret_cast<T>(_data); }

        template<typename T> 
        const T& data(Time * last_update_time) { *last_update_time = _last_update; return reinterpret_cast<T>(_data); }

        Time last_update() { return _last_update; }

        friend Debug & operator<<(Debug & db, const Interest & i) {
            db << *reinterpret_cast<const Interest_Message*>(&i) << ", _dt=" << reinterpret_cast<void*>(i._data) << ", _lup=" << i._last_update << ", _upf=" << reinterpret_cast<void*>(i._update_functor) << ", _pef=" << reinterpret_cast<void*>(i._period_functor) << ", _peh=" << reinterpret_cast<void*>(i._period_handler) << ", _tstp=" << reinterpret_cast<void*>(i._tstp) << ", _resc=" << reinterpret_cast<void*>(i._rescheduler);
            return db;
        }
        friend OStream & operator<<(OStream & os, const Interest & i) {
            os << *reinterpret_cast<const Interest_Message*>(&i) << ", _dt=" << reinterpret_cast<void*>(i._data) << ", _lup=" << i._last_update << ", _upf=" << reinterpret_cast<void*>(i._update_functor) << ", _pef=" << reinterpret_cast<void*>(i._period_functor) << ", _peh=" << reinterpret_cast<void*>(i._period_handler) << ", _tstp=" << reinterpret_cast<void*>(i._tstp) << ", _resc=" << reinterpret_cast<void*>(i._rescheduler);
            return os;
        }

    private:
        template<typename T>
        void update(T * data, const Time & time) {
            *_data = *data;
            _last_update = time;
            if(_update_functor)
                (*_update_functor)();
        }

        Functor_Handler<Interest> * period_functor() { return _period_functor; }

        Data_Type_By_Code<>::Type * _data; // TODO
        Time _last_update;
        Functor_Handler<Interest> * _update_functor;
        Functor_Handler<Interest> * _period_functor;
        Handler * _period_handler;
        TSTP * _tstp;
        Rescheduler<Interest> * _rescheduler;
    };

    class Sensor : public Data_Message {
        friend class TSTP;
    public:

        typedef void (Handler)(Sensor *);

        template<typename DATA>
        Sensor(TSTP * tstp, DATA * data, const Error & error, const Time & time_to_measure, const Time & time_between_measurements, Handler * measure = 0) : 
            Data_Message(DATA::UNIT), _data(data->data()), _handler(0), 
            _error(error), _time_to_measure(time_to_measure), 
            _cooldown(time_between_measurements), _last_measurement(0), _tstp(tstp) { 
            if(measure)
                _handler = new Functor_Handler<Sensor>(measure, this);
            _tstp->add_sensor(this);
        }

        ~Sensor() { 
            if(_handler) delete _handler;
            _tstp->remove_sensor(this); 
        }

        TSTP * tstp() { return _tstp; }

        template<typename T> 
        const T* data() { return reinterpret_cast<T*>(_data); }

        const Time & time_to_measure() const { return _time_to_measure; }
        const Time & cooldown() const { return _cooldown; }
        const Time & last_measurement() const { return _last_measurement; }
        Time period() const { return _time_to_measure + _cooldown; }
        const Error & error() const { return _error; }

    private:
        unsigned int update() {
            if(_handler) {
                auto now = _tstp->time();
                if(now - _last_measurement < _cooldown) {
                    return 0;
                }
                (*_handler)();
                _last_measurement = now;
            }
            auto ret = sizeof(Data_Type_By_Code<>::Type); // TODO
            memcpy(Data_Message::_data, _data, ret); 
            return ret;
        }

        Data_Type_By_Code<>::Type * _data; // TODO
        Functor_Handler<Sensor> * _handler;
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

    static TSTP * get_by_nic(unsigned int unit) {
        if(unit >= Traits<NIC>::UNITS) {
            db<TSTP>(WRN) << "TSTP::get_by_nic: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _network[unit];
    }

    template<unsigned int UNIT = 0>
    static void init(unsigned int unit = UNIT);

private:
    class Event;
    typedef Simple_Ordered_List<Event, Time> Event_Schedule;

    Event_Schedule _event_schedule;

    class Event {
    public:
        typedef Event_Schedule::Element Element;

        Event(TSTP * tstp, const Time & when, Handler * h) : link(this, when), handle(h) { 
            db<TSTP>(TRC) << "Event() => " << reinterpret_cast<void*>(this) << endl;
        }
        ~Event() {
            db<TSTP>(TRC) << "~Event() => " << reinterpret_cast<void*>(this) << endl;
        }

        void operator()() { (*handle)(); }

        Element link;
        Handler * handle;
    };

    template<typename T>
    class Rescheduler : public Handler {
    public:
        Rescheduler(T * o) : _object(o) { 
            _object->_rescheduler = this; 
            db<TSTP>(TRC) << "Rescheduler() => " << reinterpret_cast<void*>(this) << endl;
        }
        ~Rescheduler() {
            db<TSTP>(TRC) << "~Rescheduler() => " << reinterpret_cast<void*>(this) << endl;
        }

        void operator()() {
            auto t = _object->tstp()->time();
            auto next = t + _object->period();
            bool ok = (next <= _object->t_end());
            if(ok) {
                event = _object->tstp()->schedule(next, this); 
            }
            if(_object->period_functor())
                (*(_object->period_functor()))();
            if(not ok) {
                _object->_rescheduler = 0;
                delete this;
            }
        }
        Event * event;
    private:
        T * _object;
    };

    class Subscribed_Sensor {
        friend class TSTP::Rescheduler<Subscribed_Sensor>;
    public:
        Subscribed_Sensor(Sensor * s, Interest_Message * i) : _sensor(s), _tstp(s->tstp()), _period(i->period()), _t_end(i->t_end()), _period_functor(&handler, this) { 
            db<TSTP>(TRC) << "Subscribed_Sensor() => " << reinterpret_cast<void*>(this) << endl;
        }

        ~Subscribed_Sensor() {
            db<TSTP>(TRC) << "~Subscribed_Sensor() => " << reinterpret_cast<void*>(this) << endl;
        }

        Functor_Handler<Subscribed_Sensor> * period_functor() { return &_period_functor; }
        Time t_end() { return _t_end; }
        Time period() { return _period; }
        TSTP * tstp() { return _tstp; }

    private:
        static void handler(Subscribed_Sensor * s) {
            if(s->_sensor) {
                auto sz = s->_sensor->update();
                s->_tstp->send(reinterpret_cast<Data_Message*>(s), sizeof(Data_Message) - MAX_DATA_SIZE + sz);
            }
            if(s->_tstp->time() + s->period() >= s->_t_end)
                delete s;
        }

        Rescheduler<Subscribed_Sensor> * _rescheduler;
        Sensor * _sensor;
        TSTP * _tstp;
        Time _period;
        Time _t_end;
        Functor_Handler<Subscribed_Sensor> _period_functor;
    };

    typedef Hash<Interest, Traits<TSTP>::MAX_INTERESTS> Interests;
    typedef Hash<Sensor, Traits<TSTP>::MAX_SENSORS> Sensors;

    Interests _interests;
    Sensors _sensors;

    void add_interest(Interest * i) {
        if(i->_period_functor) {
            auto resc = new Rescheduler<Interest>(i);
            auto evt = schedule(i->t0() + i->period(), resc);
            resc->event = evt; 
        }
        _interests.insert(new Interests::Element(i, i->unit()));
        send(reinterpret_cast<Interest_Message*>(i));
    }

    template<typename T>
    void send(T * t, unsigned int sz = sizeof(T)) {
        auto buf = _mac->alloc(sz, reinterpret_cast<Frame *>(t));
        if(buf) {
            _mac->send(buf);
        }
    }

    // Should only be called via Interest's destructor
    void remove_interest(Interest * in) {
        auto el = _interests.remove(in);
        if(el) delete el;
        if(in->_rescheduler and in->_rescheduler->event)
            _event_schedule.remove(in->_rescheduler->event);
    }
    void add_sensor(Sensor * s) {
        kout << "Sensor unit = " << s->unit() << endl;
        _sensors.insert(new Sensors::Element(s, s->unit())); 
    }
    void remove_sensor(Sensor * s) {
        auto el = _sensors.remove(s);
        if(el) delete el;
    }

    Event * schedule(const Time & when, Handler * what) {
        db<TSTP>(TRC) << "TSTP::schedule(when=" << when << ", what= " << reinterpret_cast<void *>(what) << ")" << endl;
        auto event = new Event(this, when, what);
        db<TSTP>(TRC) << "event = " << event << endl;
        auto el = _event_schedule.head();
        _event_schedule.insert(&(event->link));
        auto el2 = _event_schedule.head();
        if(el != el2) {
            _next_event = el2->object();
            _time->interrupt(el2->rank());
        }
        return event;
    }

    Event * _next_event;

    void process_event() {
        db<TSTP>(TRC) << "TSTP::process_event() : " << reinterpret_cast<void *>(_next_event) << endl;
        auto el = _event_schedule.remove(&_next_event->link);
        (*_next_event)();
        db<TSTP>(TRC) << "TSTP::process_event : el=" << reinterpret_cast<void *>(el) << endl;
        if(el) { delete el->object(); }
        el = _event_schedule.head();
        if(el) {
            _next_event = el->object();
            _time->interrupt(el->rank());
        }
    }

    TSTP();

public://TODO: remove "public"
    MAC * _mac;
    Time_Manager * _time;
    Router * _router;
    Security * _security;

    static TSTP * _network[Traits<NIC>::UNITS];

    // == TSTP_MAC -> TSTP interface ==
    void failed(Scheduled_Message * message);
    void update(Interest_Message * interest) { // TODO
        db<TSTP>(TRC) << "TSTP::update: Interest " << (*interest) << endl;
        for(auto el = _sensors.search_key(interest->unit()); el; el = el->next()) {
            auto sensor = el->object();
            if((sensor->period() <= interest->period()) and (sensor->error() <= interest->error())) {
                db<TSTP>(TRC) << "Found sensor " << reinterpret_cast<void *>(sensor) << endl;
                subscribe(sensor, interest);
            }
        }
    }
    void subscribe(Sensor * s, Interest_Message * i) {
        db<TSTP>(TRC) << "TSTP::subscribe(" << reinterpret_cast<void *>(s) << ", " << reinterpret_cast<void *>(i) << ")" << endl;
        db<TSTP>(TRC) << "t0 = " << i->t0() << endl;
        schedule(i->t0(), new Rescheduler<Subscribed_Sensor>(new Subscribed_Sensor(s, i))); // TODO: grab event pointer
    }
    void update(Data_Message * data, unsigned int size) { // TODO
    }
};

__END_SYS

#endif
