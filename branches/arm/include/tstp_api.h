// EPOS TSTP Definitions Common Package

#ifndef __tstp_api_h
#define __tstp_api_h

#include <cpu.h>
#include <ieee1451_0.h>
#include <tstp_time.h>
#include <tstp_router.h>
#include <tstp_security.h>

__BEGIN_SYS

class TSTP_API
{
public:
    typedef Traits<TSTP>::MAC MAC;
    typedef Traits<TSTP>::Time_Manager Time_Manager;
    typedef Traits<TSTP>::Router Router;
    typedef Traits<TSTP>::Security Security;

    typedef Router::Local_Address Local_Address;
    typedef Router::Remote_Address Remote_Address;
    typedef Router::Hint Address_Hint;
    typedef Time_Manager::Time Time;
    typedef Security::MAC Sec_MAC;

    typedef IEEE1451_0::UNIT_CODE::UNIT_CODE_T Unit;
    typedef unsigned char Error;
    //typedef RTC::Microsecond Microsecond;
    //typedef unsigned int Data;
    //typedef Data (* Sensor_Handler)();
    typedef CPU::Reg16 Message_ID;

    // Must be <= PHY::MTU
    static const unsigned int MTU = 120;

    enum MESSAGE_TYPE {
        INTEREST = 0,
        DATA = 1,
        REPORT = 2,
        // 3 is unused
        BOOTSTRAP_0 = 4,
        BOOTSTRAP_1 = 5,
        BOOTSTRAP_2 = 6,
        BOOTSTRAP_3 = 7,
    };

    enum RESPONSE_MODE {
        SINGLE = 0,
        ALL = 1,
    };

    class Header
    {
    public:
        Header() { }
        Header(const MESSAGE_TYPE & t) : _message_type(t) {}
        Header(const MESSAGE_TYPE & t, const Local_Address & origin, const Time & deadline) : _message_type(t), _time_request(0), _spatial_scale(0), _temporal_scale(0), _last_hop_address(origin), _last_hop_time(0), _origin_address(origin), _origin_time(0), _deadline(deadline) { }

        MESSAGE_TYPE message_type() { return static_cast<MESSAGE_TYPE>(_message_type); }
        void message_type(const MESSAGE_TYPE & t) { _message_type = t; }

        const Local_Address & last_hop_address() { return _last_hop_address; }
        void last_hop_address(const Local_Address & _addr) { _last_hop_address = _addr; }

        const Local_Address & origin_address() { return _origin_address; }
        void origin_address(const Local_Address & _addr) { _origin_address = _addr; }

        Time last_hop_time() { return _last_hop_time; }
        void last_hop_time(const Time & t) { _last_hop_time = t; }

        Time origin_time() { return _origin_time; }
        void origin_time(const Time & t) { _origin_time = t; }

        bool time_request() { return _time_request; }
        void time_request(bool t) { _time_request = t; }

        Time deadline() { return _deadline; }
        void deadline(const Time & t) { _deadline = t; }

        //friend Debug & operator<<(Debug & db, const Header & h) {
        //    db << "{type=" << h._message_type << ",tr=" << h._time_request << ",sscale=" << h._spatial_scale << ",tscale=" << h._temporal_scale << ",lconf=" << h._location_confidence << ",lhaddr=" << h._last_hop_address << ",lht=" << h._last_hop_time << ",oaddr=" << h._origin_address << ",ot=" << h._origin_time << "}" << endl;
        //    return db;
        //}
        //friend OStream & operator<<(OStream & os, const Header & h) {
        //    os << "{type=" << h._message_type << ",tr=" << h._time_request << ",sscale=" << h._spatial_scale << ",tscale=" << h._temporal_scale << ",lconf=" << h._location_confidence << ",lhaddr=" << h._last_hop_address << ",lht=" << h._last_hop_time << ",oaddr=" << h._origin_address << ",ot=" << h._origin_time << "}" << endl;
        //    return os;
        //}

    private:
        unsigned _message_type : 3;
        unsigned _time_request : 1;
        unsigned _spatial_scale : 2;
        unsigned _temporal_scale : 2;
        //unsigned _location_confidence : 8;
        Local_Address _last_hop_address;
        Time _last_hop_time;
        Local_Address _origin_address;
        Time _origin_time;
        Time _deadline;
    } __attribute__((packed));

    // Generic Frame. Should be parsed and cast to a Message
    class Frame : public Header {
    public:
        template<typename T>
        T* data() { return 0; }
    private:
        unsigned char _data[MTU - sizeof(Header)];
    }__attribute__((packed));

    class Interest_Message : public Header {
    public:
        static const MESSAGE_TYPE TYPE = MESSAGE_TYPE::REPORT;

        Interest_Message() : Header(TYPE) { }
        Interest_Message(const Local_Address & origin, const Time & deadline, const Remote_Address & destination, 
                const Time & t0, const Time & t_end, const Time & period, const Unit & unit, 
                const RESPONSE_MODE & response_mode, const Error & max_error) : 
            Header(TYPE, origin, deadline), _destination(destination), _t0(t0), _t_end(t_end), 
            _period(period), _unit(unit),  _response_mode(response_mode), _error(max_error) { }

        Time t0() { return _t0; }
        Time t_end() { return _t_end; }
        Time period() { return _period; }

    private:
        Remote_Address _destination;
        Time _t0;
        Time _t_end;
        Time _period;
        Unit _unit;
        unsigned _response_mode : 1;
        unsigned _error : 7;
    }__attribute__((packed));

    class Buffer;
    class Scheduled_Message {
        Message_ID _id;
        Frame * _frame;
        Buffer * _buffer;
        Alarm * _alarm;
    };

    typedef _UTIL::Buffer<TSTP, Frame, Scheduled_Message> Buffer;

    static const unsigned int MAX_DATA_SIZE = MTU - sizeof(Header) - sizeof(Sec_MAC);

    class Data_Message : public Header {
        friend class TSTP_Security;
    public:
        static const MESSAGE_TYPE TYPE = MESSAGE_TYPE::DATA;
        Data_Message() : Header(TYPE) { }
        Data_Message(Unit unit) : Header(TYPE), _unit(unit) { }
    protected:
        Sec_MAC _mac;
        Unit _unit;
        unsigned char _data[MAX_DATA_SIZE];
    }__attribute__((packed));

    class Report_Message : public Header {
    public:
        static const MESSAGE_TYPE TYPE = MESSAGE_TYPE::REPORT;
        Report_Message() : Header(TYPE) { }
    private:
        Sec_MAC _mac;
        Unit _unit;
        unsigned char _data[MAX_DATA_SIZE];
    }__attribute__((packed));
};

__END_SYS

#endif
