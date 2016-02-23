// EPOS TSTP Common Package

#ifndef __tstp_h
#define __tstp_h

#include <alarm.h>
#include <ieee1451_0.h>
#include <tstp_mac.h>
#include <nic.h>
#include <rtc.h>
#include <utility/hash.h>
#include <utility/ostream.h>

__BEGIN_SYS

class TSTP : public TSTP_Common
{
    friend class TSTP_MAC;
    friend class Interest;
    friend class Sensor;

public:
    typedef TSTP_MAC MAC;

    static Time time_now() { return TSTP_MAC::time_now(); }

    typedef Data (* Sensor_Handler)();
    typedef void (* Interest_Handler)(const Data &);

    class Interest : public TSTP_Common::Interest
    {
        friend class TSTP;

    public:
        Interest(Interest_Handler handler, const Region & region, const Time & t0, const Time & dt, const Time & period, const Unit & unit, const Data & precision, const RESPONSE_MODE & response_mode) 
            : TSTP_Common::Interest(region, t0, dt, period, unit, precision, response_mode), _handler(handler), _last_reading(0)
        {
            TSTP::instance->add(*this);
            MAC::send(this);
        }
        ~Interest() {
            TSTP::instance->remove(*this);
        }

        void handle(const Data & d) const { _handler(d); }
        Time last_reading() const { return _last_reading; }
        void last_reading(const Time & time) { _last_reading = time; }

        friend Debug & operator<<(Debug & db, const Interest & i) {
            db << "{" << static_cast<const TSTP_Common::Interest &>(i) << ",_handler=" << hex << reinterpret_cast<unsigned int *>(i._handler) << "}";
            return db;
        }
        friend OStream & operator<<(OStream & s, const Interest & i) {
            s << "{" << static_cast<const TSTP_Common::Interest &>(i) << ",_handler=" << hex << reinterpret_cast<unsigned int *>(i._handler) << "}";
            return s;
        }

    private:
        Interest_Handler _handler;
        Time _last_reading;
    }__attribute__((packed));

    class Sensor
    {
    public:
        Sensor(Unit unit, Sensor_Handler sense, Data precision, Microsecond period) 
        : _period(period), _unit(unit), _precision(precision),  _sense(sense)
        {
            TSTP::instance->add(*this);
            //TSTP::instance->publish(*this);
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
        friend OStream & operator<<(OStream & os, const Sensor & s) {
            os << "{_period=" << s._period << ",_unit=" << s._unit << ",_precision=" << s._precision << ",_sense=" << &s._sense << "}";
            return os;
        }

    private:
        Microsecond _period;
        Unit _unit;
        Data _precision;
        Sensor_Handler _sense;
    };

    static void init();

private:
    struct Scheduled_Message {
        Scheduled_Message(const Sensor * s, const Time & t, const Time & p, const Time & d, const Time & u) : first_time(true), sensor(s), t0(t), period(p), deadline(d), until(u), handler(&TSTP::send_data, this) {
            alarm = new Alarm(t0 - time_now(), &handler, 1); 
        }

        ~Scheduled_Message() { delete alarm; }

        bool first_time;
        const Sensor * sensor;
        Time t0;
        Time period;
        Time deadline;
        Time until;
        Functor_Handler<Scheduled_Message> handler;
        Alarm * alarm;
    };

    void process(const Time & when, const RSSI & rssi, Header * h) {
        db<TSTP>(TRC) << "TSTP: Interest Received : t=" << when << ",rssi=" << rssi << ",h=" << *h << endl;
        //PTP::process(when, h);
        //HECOPS::process(rssi, h);
    }

    template<typename T>
    void process(const Time & when, const RSSI & rssi, Header * h, T * payload) {
        db<TSTP>(TRC) << "TSTP::process(t=" << when << ",rssi=" << rssi << ",h=" << *h << ",i=" << *payload << ")" << endl;
        process(when, rssi, h);
        process(payload, h);
    }

    void process(TSTP_Common::Interest * i, Header * h);
    void process(TSTP_Common::Labeled_Data * d, Header * h);

    TSTP();

    static TSTP * instance;

    typedef Hash<Interest, Traits<TSTP>::MAX_INTERESTS> Interests;
    typedef Hash<Sensor, Traits<TSTP>::MAX_SENSORS> Sensors;

    Interests interests;
    Sensors sensors;

    //void publish(const Sensor & s);
    //void publish(const Interest & i);
    void subscribe(Sensor * s, TSTP_Common::Interest * i);
    static void send_data(Scheduled_Message * s);

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
};

__END_SYS

#endif
