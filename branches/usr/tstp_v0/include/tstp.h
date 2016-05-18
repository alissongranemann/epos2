// EPOS Trustful SpaceTime Protocol Declarations

#ifndef __tstp_h
#define __tstp_h

#include <tstp_common.h>
#include <utility/observer.h>
#include <utility/buffer.h>
#include <utility/hash.h>
#include <network.h>
#include <tstp_net.h>
#include <timer.h>

__BEGIN_SYS

typedef Traits<TSTP>::MAC TSTPNIC;

class TSTP_Packet: public TSTP_Common::Header
{
public:
    static const unsigned int MTU = NIC::MTU - sizeof(TSTP_Common::Header);
private:
    typedef unsigned char Data[MTU];
public:
    TSTP_Packet() {}

    TSTP_Common::Header * header() { return this; }

    template<typename T>
    T * data() { return reinterpret_cast<T *>(&_data); }

    friend Debug & operator<<(Debug & db, const TSTP_Packet & p) {
        db << "{h=" << reinterpret_cast<const TSTP_Common::Header &>(p) << ",d=" << p._data << "}";
        return db;
    }

private:
    Data _data;
} __attribute__((packed));

// Time Managers
class RTC_Time_Manager : public TSTP_Common {
public:
    static Time now() { return RTC::seconds_since_epoch(); }
};

// Passive Time Synchronization
class PTS : public TSTP_Common, private TSTPNIC::Observer
{
    typedef TSTP_Timer Timer;
    static const unsigned int TX_DELAY = TSTPNIC::TX_DELAY; 

    typedef TSTPNIC::Buffer Buffer;
    typedef TSTP_Packet Packet;

    typedef TSTP_Timer::Time_Stamp Time_Stamp;
    static const IF<(static_cast<Time_Stamp>(-1) < static_cast<Time_Stamp>(0)), bool, void>::Result _time_stamp_sign_check; // TSTP_Timer::Time_Stamp must be signed!
    typedef TSTP_Common::Time_Offset Short_Time_Stamp;
    typedef TSTP_Common::Microsecond Time;

public:
    PTS() {
        _timer.start();
        TSTPNIC::attach(this);
    }
    ~PTS() {
        _timer.stop();
        TSTPNIC::detach(this);
    }

    static Time now() { return ts_to_us(_timer.now()); }

    static bool bootstrap() {
        while(!_synced);
        return true; 
    }

    // Called by TSTPNIC
    typedef IC::Interrupt_Handler Interrupt_Handler;
    static void cancel_interrupt() { _timer.cancel_interrupt(); }

    static void interrupt(const Time & when, const Interrupt_Handler & h) {
        auto tnow = _timer.now();
        Time_Stamp w = us_to_ts(when);
        if(when <= tnow) {
            _timer.interrupt(tnow + 1000, h); //TODO
        } else {
            _timer.interrupt(w, h);
        }
    }

    // Called by Observer
    void update(TSTPNIC::Observed * obs, Buffer * buf) {
        //Time_Stamp adjust = 0;
        auto packet = buf->frame()->data<Packet>();
        switch(packet->type()) {
        case INTEREST: {
            //Short_Time_Stamp sfd = buf->sfd_time();
            //Short_Time_Stamp to_set = packet->last_hop_time() + TX_DELAY;
            //Time_Stamp diff1 = sfd - to_set;
            //Time_Stamp diff2 = to_set - sfd;
            //adjust = _UTIL::abs(diff1) < _UTIL::abs(diff2) ? diff1 : diff2;
            //_timer.adjust(adjust);
            //_synced = true;
        } break;
        default: break;
        }
    }

private:
    static Time ts_to_us(const Time_Stamp & ts) { return _timer.ts_to_us(ts); }
    static Time_Stamp us_to_ts(const Time & us) { return _timer.us_to_ts(us); }

    static Timer _timer;
    static volatile bool _synced;
};

//Locators
class NIC_Locator : public TSTP_Common {
public:
    static Coordinates here() { return (TSTPNIC::nic()->address())[5] % 2 ? Coordinates(0, 0, 0) : Coordinates(10, 10, 10); }
};

class TSTP: public TSTP_Common, private TSTPNIC::Observer
{
    template<typename> friend class Smart_Data;

private:
    typedef TSTPNIC NIC;
    typedef Traits<TSTP>::Time_Manager Time_Manager;
    typedef Traits<TSTP>::Security Security;
    typedef Traits<TSTP>::Locator Locator;
    typedef Traits<TSTP>::Router Router;

public:
    // Buffers received from the NIC
    typedef TSTPNIC::Buffer Buffer;

    // Packet
    typedef TSTP_Packet Packet;
    static const unsigned int MTU = Packet::MTU;

    // TSTP observer/d conditioned to a message's address (ID)
    typedef Data_Observer<Packet, int> Observer;
    typedef Data_Observed<Packet, int> Observed;

    // Hash to store TSTP Observers by type
    class Interested;
    typedef Hash<Interested, 10, Unit> Interests;
    class Responsive;
    typedef Hash<Responsive, 10, Unit> Responsives;


    // TSTP Messages
    // Each TSTP message is encapsulated in a single package. TSTP does not need nor supports fragmentation.

    // Interest/Response Modes
    enum Mode {
        // Response
        SINGLE = 0, // Only one response is desired for each interest job (desired, but multiple responses are still possible)
        ALL    = 1, // All possible responses (e.g. from different sensors) are desired
        // Interest
        DELETE = 2  // Revoke an interest
    };

    // Interest Message
    class Interest: public Header
    {
    public:
        Interest(const Region & region, const Unit & unit, const Mode & mode, const Error & precision, const Microsecond & expiry, const Microsecond & period = 0)
        : Header(INTEREST, 0, 0, here(), here(), 0), _region(region), _unit(unit), _mode(mode), _precision(0), _expiry(expiry), _period(period) {}

        const Unit & unit() const { return _unit; }
        const Region & region() const { return _region; }
        Microsecond period() const { return _period; }
        Time expiry() const { return _expiry; } // TODO: must return absolute time
        Mode mode() const { return static_cast<Mode>(_mode); }
        Error precision() const { return static_cast<Error>(_precision); }

        bool time_triggered() { return _period; }
        bool event_driven() { return !time_triggered(); }

        friend Debug & operator<<(Debug & db, const Interest & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",m=" << ((m._mode == ALL) ? 'A' : 'S') << ",e=" << int(m._precision) << ",x=" << m._expiry << ",re=" << m._region << ",p=" << m._period;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        unsigned char _mode : 2;
        unsigned char _precision : 6;
        Time_Offset _expiry;
        Microsecond _period;
    } __attribute__((packed));

    // Response (Data) Message
    class Response: public Header
    {
    private:
        typedef unsigned char Data[MTU - sizeof(Unit) - sizeof(Error) - sizeof(Time)];

    public:
        Response(const Unit & unit, const Error & error = 0, const Time & expiry = 0)
        : Header(RESPONSE, 0, 0, here(), here(), 0), _unit(unit), _error(error), _expiry(expiry) {}

        const Unit & unit() const { return _unit; }
        Time expiry() const { return _expiry; }
        Error error() const { return _error; }

        template<typename T>
        void value(const T & v) { *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data) = v; }

        template<typename T>
        T value() { return *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Response & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",e=" << int(m._error) << ",x=" << m._expiry << ",d=" << hex << *const_cast<Response &>(m).data<unsigned>() << dec;
            return db;
        }

    protected:
        Unit _unit;
        Error _error;
        Time_Offset _expiry;
        Data _data;
    } __attribute__((packed));

    // Command Message
    class Command: public Header
    {
    private:
        typedef unsigned char Data[MTU - sizeof(Region) - sizeof(Unit)];

    public:
        Command(const Unit & unit, const Region & region)
        : Header(COMMAND, 0, 0, here(), here(), 0), _region(region), _unit(unit) {}

        const Region & region() const { return _region; }
        const Unit & unit() const { return _unit; }

        template<typename T>
        T * command() { return reinterpret_cast<T *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Command & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",reg=" << m._region;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        Data _data;
    } __attribute__((packed));

    // Control Message
    class Control: public Header
    {
    private:
        typedef unsigned char Data[MTU - sizeof(Region) - sizeof(Unit)];

    public:
        Control(const Unit & unit, const Region & region)
        : Header(CONTROL, 0, 0, here(), here(), 0), _region(region), _unit(unit) {}

        const Region & region() const { return _region; }
        const Unit & unit() const { return _unit; }

        template<typename T>
        T * command() { return reinterpret_cast<T *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Control & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",reg=" << m._region;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        Data _data;
    } __attribute__((packed));

    // Interested (binder between Interest messages and Smart Data)
    class Interested: public Interest
    {
    public:
        template<typename T>
        Interested(T * data, const Region & region, const Unit & unit, const Mode & mode, const Precision & precision, const Microsecond & expiry, const Microsecond & period = 0)
        : Interest(region, unit, mode, precision, expiry, period), _link(this, T::UNIT) {
            db<TSTP>(TRC) << "TSTP::Interested(d=" << data << ",r=" << region << ",p=" << period << ") => " << reinterpret_cast<const Interest &>(*this) << endl;
            _interested.insert(&_link);
            advertise();
        }
        ~Interested() {
            db<TSTP>(TRC) << "TSTP::~Interested(this=" << this << ")" << endl;
            _interested.remove(&_link);
            revoke();
        }

        void advertise() { send(); }
        void revoke() { _mode = DELETE; send(); }

    private:
        void send() {
            db<TSTP>(TRC) << "TSTP::Interested::send() => " << reinterpret_cast<const Interest &>(*this) << endl;
            Buffer * buf = NIC::alloc(sizeof(Interest));
            memcpy(buf->frame()->data<Interest>(), this, sizeof(Interest));
            NIC::send(buf);
        }

    private:
        Interests::Element _link;
    };

    // Responsive (binder between Smart Data (Sensors) and Response messages)
    class Responsive: public Response
    {
    public:
        template<typename T>
        Responsive(T * data, const Unit & unit, const Error & error, const Time & expiry)
        : Response(unit, error, expiry), _size(sizeof(Response) + sizeof(typename T::Value)), _link(this, T::UNIT) {
            db<TSTP>(TRC) << "TSTP::Responsive(d=" << data << ",s=" << _size << ") => " << this << endl;
            db<TSTP>(INF) << "TSTP::Responsive() => " << reinterpret_cast<const Response &>(*this) << endl;
            _responsives.insert(&_link);
        }
        ~Responsive() {
            db<TSTP>(TRC) << "TSTP::~Responsive(this=" << this << ")" << endl;
            _responsives.remove(&_link);
        }


        using Header::time;
        using Header::origin;

        void respond(const Time & expiry) { send(expiry); }

    private:
        void send(const Time & expiry) {
            db<TSTP>(TRC) << "TSTP::Responsive::send(x=" << expiry << ")" << endl;
            Buffer * buf = NIC::alloc(_size);
            memcpy(buf->frame()->data<Response>(), this, _size);
            db<TSTP>(INF) << "TSTP::Responsive::send:response=" << this << " => " << reinterpret_cast<const Response &>(*this) << endl;
            NIC::send(buf);
        }

    private:
        unsigned int _size;
        Responsives::Element _link;
    };

 public:
    TSTP() {
        db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
        NIC::attach(this);
    }
    ~TSTP() {
        db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
        NIC::detach(this);
    }

    static Time now() { return Time_Manager::now(); }
    static Coordinates here() { return Locator::here(); }

    static void attach(Observer * obs, void * subject) { _observed.attach(obs, int(subject)); }
    static void detach(Observer * obs, void * subject) { _observed.detach(obs, int(subject)); }
    static bool notify(void * subject, Packet * packet) { return _observed.notify(int(subject), packet); }

    static void init(unsigned int unit) {
        db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    }

private:
    static Coordinates absolute(const Coordinates & coordinates) { return coordinates; }

    void update(NIC::Observed * obs, Buffer * buf) {
        db<TSTP>(TRC) << "TSTP::update(obs=" << obs << ",buf=" << buf << ")" << endl;

        if(buf->destined_to_me()) {
            Packet * packet = buf->frame()->data<Packet>();
            switch(packet->type()) {
            case INTEREST: {
                Interest * interest = reinterpret_cast<Interest *>(packet);
                db<TSTP>(INF) << "TSTP::update:msg=" << interest << " => " << *interest << endl;
                // Check for local capability to respond and notify interested observers
                Responsives::List * list = _responsives[interest->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
                if(list)
                    for(Responsives::Element * el = list->head(); el; el = el->next()) {
                        Responsive * responsive = el->object();
                        if(interest->region().contains(responsive->origin(), now())) {
                            notify(responsive, packet);
                        }
                    }
            } break;
            case RESPONSE: {
                Response * response = reinterpret_cast<Response *>(packet);
                db<TSTP>(INF) << "TSTP::update:msg=" << response << " => " << *response << endl;
                // Check region inclusion and notify interested observers
                Interests::List * list = _interested[response->unit()];
                if(list)
                    for(Interests::Element * el = list->head(); el; el = el->next()) {
                        Interested * interested = el->object();
                        if(interested->region().contains(response->origin(), response->time()))
                            notify(interested, packet);
                    }
            } break;
            case COMMAND: {
                Command * command = reinterpret_cast<Command *>(packet);
                db<TSTP>(INF) << "TSTP::update:msg=" << command << " => " << *command << endl;
                // Check for local capability to respond and notify interested observers
                Responsives::List * list = _responsives[command->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
                if(list)
                    for(Responsives::Element * el = list->head(); el; el = el->next()) {
                        Responsive * responsive = el->object();
                        if(command->region().contains(responsive->origin(), now()))
                            notify(responsive, packet);
                    }
            } break;
            case CONTROL: break;
            }
        }

        buf->nic()->free(buf);
    }

private:
    static Interests _interested;
    static Responsives _responsives;

    static Observed _observed; // Channel protocols are singletons
 };


template<TSTP_Common::Scale S>
inline TSTP_Common::Time TSTP_Common::_Header<S>::time() const {
    return TSTP::now() + _elapsed;
}

template<TSTP_Common::Scale S>
inline void TSTP_Common::_Header<S>::time(const TSTP_Common::Time & t) {
    _elapsed = t - TSTP::now();
}

__END_SYS

#endif
