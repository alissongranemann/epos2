// EPOS TSTP Common Package

#ifndef __tstp_h
#define __tstp_h

#include <ieee1451_0.h>
#include <rtc.h>
#include <utility/hash.h>

#include <tstp_time.h>
#include <tstp_router.h>
#include <tstp_mac.h>
#include <tstp_security.h>

__BEGIN_SYS

class TSTP
{
    friend class Interest;
    friend class Sensor;

    typedef Traits<TSTP>::MAC MAC;
    typedef Traits<TSTP>::Time_Manager Time_Manager;
    typedef Traits<TSTP>::Router Router;
    typedef Traits<TSTP>::Security Security;

    typedef Router::Address Address;

    TSTP(MAC * mac, Time_Manager * time, Router * router, Security * security, unsigned int unit) : _mac(mac), _time(time), _router(router), _security(security) { 
        _mac->_tstp = this;
        _time->_tstp = this;
        _router->_tstp = this;
        _security->_tstp = this;
    }

public:
    typedef IEEE1451_0::Unit Unit;
    typedef RTC::Microsecond Microsecond;
    typedef unsigned int Data;

    typedef Data (* Sensor_Handler)();    

    class Interest
    {
        /*
        friend class TSTP;

    public:
        Interest(Unit unit, Data * data, Data precision, Microsecond period) 
            : _period(period), _unit(unit), _precision(precision),  _data(data)
        {
            TSTP::instance->add(*this);
            TSTP::instance->publish(*this);
        }
        ~Interest()
        {
            TSTP::instance->remove(*this);
        }

        Unit unit() const { return _unit; }
        Microsecond period() const { return _period; }
        Data precision() const { return _precision; }

        friend Debug & operator<<(Debug & db, const Interest & i) {
            db << "{_period=" << i._period << ",_unit=" << i._unit << ",_precision=" << i._precision << ",_data=" << i._data << ",*_data=" << *(i._data) << "}";
            return db;
        }

    private:
        Microsecond _period;
        Unit _unit;
        Data _precision;
        Data * _data;
        */
    };

    class Sensor
    {
        /*
    public:
        Sensor(Unit unit, Sensor_Handler sense, Data precision, Microsecond period) 
        : _period(period), _unit(unit), _precision(precision),  _sense(sense)
        {
            TSTP::instance->add(*this);
            TSTP::instance->publish(*this);
        }
        ~Sensor()
        {
            TSTP::instance->remove(*this);
        }

        Unit unit() const { return _unit; }
        Microsecond period() const { return _period; }
        Data precision() const { return _precision; }
        Data data() const { return _sense(); }

        friend Debug & operator<<(Debug & db, const Sensor & s) {
            db << "{_period=" << s._period << ",_unit=" << s._unit << ",_precision=" << s._precision << ",_sense=" << &s._sense << "}";
            return db;
        }

    private:
        Microsecond _period;
        Unit _unit;
        Data _precision;
        Sensor_Handler _sense;
        */
    };

    struct Header
    {
        /*
        enum TYPE 
        {
            INTEREST = 0,
            DATA = 1,
            REPORT = 2,
            // 3 is unused
            BOOTSTRAP_0 = 4,
            BOOTSTRAP_1 = 5,
            BOOTSTRAP_2 = 6,
            BOOTSTRAP_3 = 7,
        };

        Header(TYPE t) : message_type(t) {};
        
        template<typename T>
        T* morph() { if(message_type == T::TYPE) return reinterpret_cast<T*>(this); return 0; }

        unsigned message_type : 3;
        unsigned time_request : 1;
        unsigned spatial_scale : 2;
        unsigned temporal_scale : 2;
        unsigned location_confidence : 8;
        */
    } __attribute__((packed));

    struct Interest_Msg : public Header
    {
        /*
        static const Header::TYPE TYPE = Header::TYPE::INTEREST;

        Interest_Msg(const Interest & i) : Header(TYPE), period(i.period()), unit(i.unit()), precision(i.precision()) 
        { }

        Microsecond period;
        Unit unit;
        Data precision;
        */
    } __attribute__((packed));

    struct Data_Msg : public Header
    {
        /*
        static const Header::TYPE TYPE = Header::TYPE::DATA;

        Data_Msg(const Sensor & s) : Header(TYPE), unit(s.unit()), data(s.data()) 
        { }

        Unit unit;
        Data data;
        */
    } __attribute__((packed));

    struct Report_Msg : public Header
    {
        /*
        static const Header::TYPE TYPE = Header::TYPE::REPORT;

        Report_Msg(const Sensor & s) : Header(TYPE), period(s.period()), unit(s.unit()), precision(s.precision()) 
        { }

        Microsecond period;
        Unit unit;
        Data precision;
        */
    } __attribute__((packed));

	//void update(NIC::Observed * o, NIC::Protocol p, NIC::Buffer *b);

    static void init(unsigned int unit);

private:
    TSTP();

    MAC * _mac;
    Time_Manager * _time;
    Router * _router;
    Security * _security;

    static TSTP * _network[Traits<NIC>::UNITS];

    /*
    static TSTP * instance;

    typedef Hash<Interest, Traits<TSTP>::MAX_INTERESTS> Interests;
    typedef Hash<Sensor, Traits<TSTP>::MAX_SENSORS> Sensors;

    Interests interests;
    Sensors sensors;

    void publish(const Sensor & s);
    void publish(const Interest & i);
    void subscribe(Sensor * s, Microsecond period);
    static void send_data(Sensor * s);

    void add(const Sensor & s) { sensors.insert(new Sensors::Element(&s, s.unit())); }
    void add(const Interest & in) { interests.insert(new Interests::Element(&in, in.unit())); }
    void remove(const Sensor & s) 
    { 
        auto el = sensors.remove(&s); 
        if(el)
            delete el;
    }
    void remove(const Interest & in)
    { 
        auto el = interests.remove(&in); 
        if(el)
            delete el;
    }

    NIC _nic;
    */
};

__END_SYS

#endif
