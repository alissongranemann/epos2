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
    friend class One_Hop_MAC;
    friend class TSTP_MAC;
    friend class PTS;

public:
    class Interest;
    class Sensor;

private:
    class Subscribed_Sensor;

    typedef List<Interest> Interests;
    typedef List<Sensor> Sensors;
    typedef List<Subscribed_Sensor> Subscribed_Sensors;

public:
    typedef TSTP_API API;
    typedef API::MAC::Buffer Buffer;

    template<typename U>
    class Datum {
    public:
        static const typename U::UNIT_CODE_T UNIT = U::UNIT_CODE;
        static const unsigned int DATUM_SIZE = U::DATUM_SIZE;
        template<typename T = char>
        volatile T * data() { return reinterpret_cast<volatile T*>(&_data); }
    private:
        char _data[DATUM_SIZE];
    };

    template<unsigned long long UNIQUE_CODE, unsigned int SIZE_OF_DATUM = sizeof(unsigned int)>
    struct User_Unit {
    public:
        typedef unsigned long long UNIT_CODE_T;
        static const UNIT_CODE_T UNIT = (UNIQUE_CODE | (1ull << 39ull));
        static const unsigned int DATUM_SIZE = SIZE_OF_DATUM;

        template<typename T = char>
        volatile T * data() { return reinterpret_cast<volatile T*>(&_data); }
    private:
        char _data[DATUM_SIZE];
    };

    typedef Datum<IEEE1451_0::UNITS::METER> Meter;

    class Interest : public Interest_Message {
        friend class TSTP;
    public:
        typedef void (Handler)(Interest *);

        template<typename DATUM>
        Interest(TSTP * tstp, DATUM * data, const Remote_Address & destination, const Time & t0, const Time & t_end,
                const Time & period, const Error & max_error, const RESPONSE_MODE & response_mode, const Handler * update_handler = 0, const Handler * period_handler = 0) :
            Interest_Message(tstp->_router->my_address(), tstp->time(), t0, destination, t0, t_end, period, DATUM::UNIT, response_mode, max_error),
            _tstp(tstp), _data(const_cast<char*>(data->data())), _last_update(0), _update_handler(update_handler), _period_handler(period_handler), _period_functor(period_handler, this), _link(this), _event()
        {
            db<TSTP>(TRC) << "Interest() => " << reinterpret_cast<void*>(this) << endl;
            _tstp->add_interest(this);
        }

        ~Interest() {
            db<TSTP>(TRC) << "~Interest() => " << reinterpret_cast<void*>(this) << endl;
            _tstp->remove_interest(this);
        }

        TSTP * tstp() { return _tstp; }

        template<typename T> 
        T* data() { return reinterpret_cast<T*>(_data); }

        template<typename T> 
        T* data(Time & last_update_time) { last_update_time = _last_update; return reinterpret_cast<T*>(_data); }

        Time last_update() const { return _last_update; }
        Local_Address last_update_address() const { return _last_update_address; }

        /*
        friend Debug & operator<<(Debug & db, const Interest & i) {
            db << *reinterpret_cast<const Interest_Message*>(&i) << ", _dt=" << reinterpret_cast<void*>(i._data) << ", _lup=" << i._last_update << ", _uph=" << reinterpret_cast<void*>(i._update_handler) << ", _peh=" << reinterpret_cast<void*>(i._period_handler) << ", _tstp=" << reinterpret_cast<void*>(i._tstp) << ", _link=" << reinterpret_cast<const void*>(&(i._link)) << ", _evt = " << reinterpret_cast<const void*>(&(i._event));
            return db;
        }
        friend OStream & operator<<(OStream & os, const Interest & i) {
            os << *reinterpret_cast<const Interest_Message*>(&i) << ", _dt=" << reinterpret_cast<void*>(i._data) << ", _lup=" << i._last_update << ", _uph=" << reinterpret_cast<void*>(i._update_handler) << ", _peh=" << reinterpret_cast<void*>(i._period_handler) << ", _tstp=" << reinterpret_cast<void*>(i._tstp) << ", _link=" << reinterpret_cast<const void*>(&(i._link)) << ", _evt = " << reinterpret_cast<const void*>(&(i._event));
            return os;
        }
        */

    private:
        template<typename T>
        void update(T * data, unsigned int size, const Time & time, const Local_Address & origin_address) {
            memcpy(_data, data, size);
            _last_update = time;
            _last_update_address = origin_address;
            if(_update_handler)
                (*_update_handler)(this);
        }

        char * _data;
        Time _last_update;
        Local_Address _last_update_address;
        Handler * _update_handler;
        const bool _period_handler;
        Functor_Handler<Interest> _period_functor;
        TSTP * _tstp;
        Interests::Element _link;
        Event _event;
    };

    class Sensor : public Data_Message {
        friend class TSTP;
    public:

        typedef void (Handler)(Sensor *);

        template<typename DATUM>
        Sensor(TSTP * tstp, DATUM * data, const Error & error, const Time & time_to_measure, const Time & time_between_measurements, Handler * measure = 0) :
            Data_Message(DATUM::UNIT), _data(const_cast<char*>(data->data())), _size_of_data(DATUM::DATUM_SIZE), _handler(measure), _error(error), _time_to_measure(time_to_measure), 
            _cooldown(time_between_measurements), _tstp(tstp), _link(this) {
                db<TSTP>(TRC) << "Sensor() => " << this << endl;
                _tstp->add_sensor(this);
        }

        ~Sensor() {
            db<TSTP>(TRC) << "~Sensor() => " << this << endl;
            _tstp->remove_sensor(this);
        }

        TSTP * tstp() { return _tstp; }

        template<typename T = char> 
        volatile T* data() { return reinterpret_cast<volatile T*>(_data); }

        unsigned int size_of_data() const { return _size_of_data; }
        Time time_to_measure() const { return _time_to_measure; }
        Time cooldown() const { return _cooldown; }
        Time last_measurement() const { return origin_time(); }
        Time last_update() const { return last_measurement(); }
        Time period() const { return _time_to_measure + _cooldown; }
        const Error & error() const { return _error; }

    private:
        unsigned int periodic_update() {
            db<TSTP>(TRC) << "Sensor::periodic_update() => " << reinterpret_cast<void*>(this) << endl;
            auto now = _tstp->time();
            if(_handler) {
                if(now - origin_time() < _cooldown) {
                    return 0;
                }
                (*_handler)(this);
            }
            origin_time(now);
            auto ret = size_of_data();
            memcpy(Data_Message::_data, _data, ret);
            return ret;
        }

        unsigned int event(const Time & when) {
            db<TSTP>(TRC) << "Sensor::event(t = "<< when << ") => " << reinterpret_cast<void*>(this) << endl;
            origin_time(when);
            auto ret = size_of_data();
            memcpy(Data_Message::_data, _data, ret);
            return ret;
        }

        char * _data;
        unsigned int _size_of_data;
        Handler * _handler;
        Error _error;
        Time _time_to_measure;
        Time _cooldown;
        TSTP * _tstp;
        Sensors::Element _link;
    };

private:
    TSTP(MAC * mac, Time_Manager * time, Router * router, Security * security, unsigned int unit) : _current_event(0), _current_event_unscheduled(false), _mac(mac), _time(time), _router(router), _security(security) { 
        _mac->_tstp = this;
        _time->_tstp = this;
        _router->_tstp = this;
        _security->_tstp = this;
    }

public:
    Time time() { return _time->time_now(); }
    const Local_Address & address() { return _router->my_address(); }

    template<typename T>
    void event(T& data) {
        db<TSTP>(TRC) << "TSTP::event(data = " << reinterpret_cast<const void*>(&data) << ") => " << reinterpret_cast<void*>(this) << endl;
        db<TSTP>(TRC) << "key = " << reinterpret_cast<int>(data.data()) << endl;

        auto t = time();
        auto k = data.data();
        for(auto el = _subscribed_sensors.begin(); el != _subscribed_sensors.end(); el++) {
            auto ss = el->object();
            if(ss->_sensor->data<>() == k) {
                db<TSTP>(TRC) << "found subscribed sensor = " << el << endl;
                ss->event(t);
            }
        }
    }

    static TSTP * get_by_nic(unsigned int unit) {
        if(unit >= Traits<NIC>::UNITS) {
            db<TSTP>(WRN) << "TSTP::get_by_nic: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _network[unit];
    }

    template<unsigned int UNIT = 0>
    static void init(unsigned int unit = UNIT);

    void bootstrap() { // TODO
        _mac->bootstrap();        
    }

private:
    Event_Schedule _event_schedule;

private:
    class Subscribed_Sensor : public Handler {
    public:
        Subscribed_Sensor(Sensor * s, Interest_Message * i) : _sensor(s), _period(i->period()), _t_end(i->t_end()), _last_send(0), _event_driven(i->event_driven()), _link(this) {
            db<TSTP>(TRC) << "Subscribed_Sensor() => " << reinterpret_cast<void*>(this) << endl;
            db<TSTP>(TRC) << "key = " << reinterpret_cast<int>(s->data<char>()) << endl;
        }

        ~Subscribed_Sensor() {
            db<TSTP>(TRC) << "~Subscribed_Sensor() => " << reinterpret_cast<void*>(this) << endl;
        }

        Time t_end() const { return _t_end; }
        Time period() const { return _period; }
        TSTP * tstp() const { return _sensor->tstp(); }

        void operator()() {
            db<TSTP>(TRC) << "Subscribed_Sensor::handler(s=" << reinterpret_cast<void*>(this) <<")"<< endl;
            const auto t = tstp()->time();
            db<TSTP>(TRC) << "t=" << t << endl;
            db<TSTP>(TRC) << "_last_send=" << _last_send << endl;
            db<TSTP>(TRC) << "period()=" << period() << endl;
            if((t - _last_send >= period()) and (t - _sensor->last_measurement() >= _sensor->cooldown())) {
                auto sz = _sensor->periodic_update();
                db<TSTP>(TRC) << "sz=" << sz << endl;
                if(sz > 0) {
                    _sensor->deadline(t + _period);
                    tstp()->send(reinterpret_cast<Data_Message*>(_sensor), sizeof(Data_Message) - MAX_DATA_SIZE + sz);
                    _last_send = t;
                }
            }
            if(t + period() >= t_end()) {
                tstp()->unsubscribe(this);
                delete this;
            }
        }

        void event(const Time & when) {
            db<TSTP>(TRC) << "Subscribed_Sensor::event(when = " << when <<")"<< endl;
            db<TSTP>(TRC) << "_event_driven = " << _event_driven << endl;
            if(_event_driven) {
                if(when >= _t_end) {
                    tstp()->unsubscribe(this);
                    delete this;
                } else {
                    auto sz = _sensor->event(when);
                    if(sz > 0) {
                        _sensor->deadline(when + _period);
                        tstp()->send(reinterpret_cast<Data_Message*>(_sensor), sizeof(Data_Message) - MAX_DATA_SIZE + sz);
                        _last_send = when;
                    }
                    tstp()->reschedule(&_event, tstp()->time() + _period);
                }
            }
        }

        Sensor * _sensor;
        const Time _period;
        const Time _t_end;
        Time _last_send;
        Event _event;
        const bool _event_driven;
        Subscribed_Sensors::Element _link;
    };

    Interests _interests;
    Sensors _sensors;
    Subscribed_Sensors _subscribed_sensors;

    template<typename T>
    void send(T * t, unsigned int sz = sizeof(T)) {
        db<TSTP>(TRC) << "TSTP::send(sz = " << sz << ")" << endl;
        t->origin_address(address());
        auto buf = _mac->alloc(sz, reinterpret_cast<Frame *>(t));
        if(buf) {
            _mac->send(buf);
        }
    }

    void add_interest(Interest * in) {
        _interests.insert(&(in->_link));
        if(in->_period_handler) {
            new (&(in->_event)) Event(in->t0() + in->period(), &(in->_period_functor), in->period(), in->t_end());
            schedule(&(in->_event));
        }
        send(reinterpret_cast<Interest_Message*>(in)); // TODO: should this be done here/now?
    }
    void remove_interest(Interest * in) {
        _interests.remove(&(in->_link));
        unschedule(&(in->_event));
    }
    void add_sensor(Sensor * s) {
        _sensors.insert(&(s->_link));
    }
    void remove_sensor(Sensor * s) {
        _sensors.remove(&(s->_link));
        auto el = _subscribed_sensors.head();
        Subscribed_Sensors::Element * next;
        for(; el; el = next) {
            next = el->next();
            if(el->object()->_sensor == s) {
                _subscribed_sensors.remove(el);
                unschedule(&(el->object()->_event));
                delete el->object();
            }
        }
    }

public:
    void unschedule(Event * e) {
        _time->cancel_interrupt();
        _event_schedule.remove(e);
        db<TSTP>(TRC) << "TSTP::unschedule(e=" << e << ") : " << _event_schedule.size() << endl;
        if(e == _current_event) {
            db<TSTP>(TRC) << "_current_event_unscheduled" << endl;
            _current_event_unscheduled = true;
        }
        if(!_current_event) {
            auto new_head = _event_schedule.head();
            if(new_head) {
                db<TSTP>(TRC) << "new_head" << endl;
                db<TSTP>(TRC) << new_head->rank() << endl;
                db<TSTP>(TRC) << time() << endl;
                _time->interrupt(new_head->rank());
            }
        }
    }
    void reschedule(Event * e, const Time & when) {
        _time->cancel_interrupt();
        db<TSTP>(TRC) << "TSTP::reschedule(event = " << e << ",when = " << when << ")" << endl;
        auto removed =_event_schedule.remove(e);
        if(removed) {
            removed->rank(when);
            _event_schedule.insert(removed);
            if(e == _current_event) {
                _current_event_unscheduled = true;
            }
        }
        if(!_current_event) {
            auto new_head = _event_schedule.head();
            _time->interrupt(new_head->rank());
        }
    }
    void schedule(Event * event) {
        _time->cancel_interrupt();
        db<TSTP>(TRC) << "TSTP::schedule_event(event = " << event << ")" << endl;
        _event_schedule.insert(&(event->_link));
        if(!_current_event) {
            auto new_head = _event_schedule.head();
            _time->interrupt(new_head->rank());
        }
    }

private:
    Event * _current_event;
    bool _current_event_unscheduled;

    // Called by time manager interrupt
    void process_event() {
        //db<TSTP>(TRC) << "TSTP::process_event() : " << _event_schedule.size() << " event(s)" << endl;
        auto t = time();
        auto el = _event_schedule.remove_head();
        if(not el) {
            return;
        }
        _current_event = el->object();
        //db<TSTP>(TRC) << "TSTP::process_event : t=" << t << ", event=" << el->object() << endl;
        (*_current_event)();
        if(not _current_event_unscheduled) {
            auto next_t = t + _current_event->period;
            auto end = _current_event->end;
            if(next_t <= end) {
                el->rank(next_t);
                _event_schedule.insert(el);
                //db<TSTP>(TRC) << "TSTP::process_event : reinserted event" << endl;
            }
        }
        _current_event = 0;
        _current_event_unscheduled = false;
        el = _event_schedule.head();
        if(el) {
            _time->interrupt(el->rank());
        }
    }

    MAC * _mac;
    Time_Manager * _time;
    Router * _router;
    Security * _security;

    static TSTP * _network[Traits<NIC>::UNITS];

    // == TSTP_MAC -> TSTP interface ==
    void failed(Buffer * b);
    void update(Interest_Message * interest, bool is_for_me = true) {
        _time->update_interest(reinterpret_cast<char *>(interest));
        if(is_for_me) {
            //db<TSTP>(TRC) << "TSTP::update: Interest " << (*interest) << endl;
            auto k = interest->unit();
            for(auto el = _sensors.begin(); el != _sensors.end(); ++el) {
                auto sensor = el->object();
                //db<TSTP>(TRC) << *sensor << endl;
                db<TSTP>(TRC) << sensor->unit() << endl;
                if(sensor->unit() == k) {
                    if((sensor->period() < interest->period()) and (sensor->error() <= interest->error())) {
                        db<TSTP>(TRC) << "Found sensor " << reinterpret_cast<void *>(sensor) << endl;
                        for(auto el = _subscribed_sensors.head(); el; el = el->next()) {
                            if((el->object()->_sensor == sensor) and (el->object()->period() == interest->period()) and (el->object()->t_end() == interest->t_end())) {
                                db<TSTP>(TRC) << "This sensor is already subscribed to this interest!" << endl;
                                return;
                            }
                        }
                        subscribe(sensor, interest);
                    }
                }
            }
        }
    }
    void update(Data_Message * data, unsigned int size, bool is_for_me = true) {
        _time->update_data(reinterpret_cast<char *>(data));
        if(is_for_me) {
            db<TSTP>(TRC) << "TSTP::update: Data " << data << ", sz= " << size << endl;
            auto k = data->unit();
            db<TSTP>(TRC) << "TSTP::update: unit = " << k << endl;
            for(auto el = _interests.begin(); el != _interests.end(); ++el) {
                auto interest = el->object();
                db<TSTP>(TRC) << "key = " << interest->unit() << endl;
                if(interest->unit() == k) {
                    db<TSTP>(TRC) << "TSTP::update: key is equal" << endl;
                    db<TSTP>(TRC) << "data->origin_time() = " << data->origin_time() << endl;
                    db<TSTP>(TRC) << "interest->last_update() = " << interest->last_update() << endl;
                    if((interest->last_update() < data->origin_time())) {
                        db<TSTP>(TRC) << "last_update < origin_time " << endl;
                        if (_router->accept(interest->destination(), data->origin_address())) {
                            db<TSTP>(TRC) << "Found valid interest " << interest << endl;
                            interest->update(data->data(), size - (sizeof(Data_Message) - MAX_DATA_SIZE), data->origin_time(), data->origin_address());
                        }
                    }
                }
            }
        }
    }
    void subscribe(Sensor * s, Interest_Message * i) {
        db<TSTP>(TRC) << "TSTP::subscribe(" << reinterpret_cast<void *>(s) << ", " << reinterpret_cast<void *>(i) << ")" << endl;
        db<TSTP>(TRC) << "t0 = " << i->t0() << endl;
        auto ss = new Subscribed_Sensor(s, i);
        _subscribed_sensors.insert(&(ss->_link));
        new (&(ss->_event)) Event(i->t0(), ss, i->period(), i->t_end());
        schedule(&(ss->_event));
    }
    void unsubscribe(Subscribed_Sensor * s) {
        _subscribed_sensors.remove(&(s->_link));
        unschedule(&(s->_event));
    }
};

__END_SYS

#endif
