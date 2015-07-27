// EPOS 1-step static hierarchy PTPv2 Protocol Declarations

#ifndef __ptp_h
#define __ptp_h

#include <nic.h>
#include <utility/random.h>
#include <condition.h>

__BEGIN_SYS

class PTP: private NIC::Observer
{
public:
    typedef NIC::Buffer Buffer;
    typedef NIC::Address Address;

    class Timestamp
    {
    public:
        Timestamp(unsigned long long seconds, unsigned long nanoseconds) {
            _seconds[0] = (short int) (seconds >> 4);
            _seconds[1] = (short int) (seconds >> 2);
            _seconds[2] = (short int) seconds;
            _nanoseconds = nanoseconds;
        }

        bool operator>(const Timestamp& t) {
            if (seconds() > t.seconds()) {
                return true;
            }

            if (seconds() == t.seconds() && nanoseconds() > t.nanoseconds()) {
                return true;
            }

            return false;
        }

        Timestamp * operator-(const Timestamp& t) {
            assert(*this > t);

            long long s = seconds();
            long long n = nanoseconds() - t.nanoseconds();

            if (n < 0) {
                s--;
                n += 1000000000;
            }

            return new Timestamp(seconds() - t.seconds(), n);
        }

        Timestamp * operator+(const Timestamp& t) {
            long long s = seconds();
            long long sum = nanoseconds() + t.nanoseconds();

            if (sum / 1000000000) {
                s += (sum / 1000000000);
            }

            return new Timestamp(s + t.seconds(), sum % 1000000000);
        }

        Timestamp * operator/(unsigned int d) {
            if ((seconds() % d) == 0) {
                return new Timestamp(seconds() / d, nanoseconds() / d);
            }

            long long n = (seconds() % d) + nanoseconds();

            return new Timestamp(seconds() / d, n / d);
        }

        unsigned long long seconds() const {
            long long s0 = _seconds[0],
                      s1 = _seconds[1],
                      s2 = _seconds[2];

            return ((s0 << 32) | (s1 << 16) | s2);
        }
        unsigned long nanoseconds() const {
            return _nanoseconds;
        }

        void seconds(unsigned long long s) {
            _seconds[0] = (short int) (s >> 4);
            _seconds[1] = (short int) (s >> 2);
            _seconds[2] = (short int) s;
        }
        void nanoseconds(unsigned long n) {
            _nanoseconds = n;
        }
    private:
        unsigned short int _seconds[3];
        unsigned long _nanoseconds;
    };

    typedef unsigned char Type;
    enum {
        SYNC                    = 0x0,
        DELAY_REQ               = 0x1,
        PDELAY_REQ              = 0x2,
        PDELAY_RESP             = 0x3,
        FOLLOW_UP               = 0x8,
        DELAY_RESP              = 0x9,
        PDELAY_RESP_FOLLOW_UP   = 0xA,
        ANNOUNCE                = 0xB,
        SIGNALING               = 0xC,
        MANAGEMENT              = 0xD
    };

    class Header
    {
    public:
        typedef short int Flag;
        enum {
            TWO_STEPS = 0x1000
        };

        static const unsigned int VER = 2;

        Header(): _transport_specific(0), _version(VER), _reserved1(0), _message_length(0),
            _reserved2(0), _flags(0 /*FIXME*/), _correction_field(0), _reserved3(0),
            _log_message_interval(0) {
        }
        Header(unsigned char domain_number, Type msg_type, unsigned int oui,
               unsigned long long uuid, unsigned short int node_port_number,
               unsigned int sequence_id):
          _message_type(msg_type), _transport_specific(0), _version(VER), _reserved1(0),
          _domain_number(domain_number), _reserved2(0), _flags(0 /*FIXME*/), _correction_field(0),
          _reserved3(0), _sequence_id(htons(sequence_id)), _log_message_interval(0) { // FIXME
            _source_port_id_lp = (oui < 8) | (uuid >> 32);
            _source_port_id_mp = (unsigned int) uuid;
            _source_port_id_rp = node_port_number;

            switch (msg_type) {
            case SYNC:
                _control_field = 0x0;
                _message_length = sizeof(Event_Message);
                break;
            case DELAY_REQ:
                _control_field = 0x1;
                _message_length = sizeof(Event_Message);
                _log_message_interval = 0x7F;
                break;
            case FOLLOW_UP:
                _control_field = 0x2;
                break;
            case DELAY_RESP:
                _control_field = 0x3;
                _message_length = sizeof(Delay_Resp_Message);
                break;
            case MANAGEMENT:
                _control_field = 0x4;
                break;
            default:
                _control_field = 0x5;
            }

            _message_length = htons(_message_length);
        }

        Type type() {
            return _message_type;
        }

        short int flags() {
            return ntohs(_flags);
        }

        unsigned char domain_number() {
            return _domain_number;
        }

        unsigned short int sequence_id() {
            return _sequence_id;
        }

    private:
        Type _message_type:4;
        const unsigned char _transport_specific:4;

        const unsigned char _version:4;
        const unsigned char _reserved1:4;

        unsigned short int _message_length;

        unsigned char _domain_number;

        const unsigned char _reserved2;

        unsigned short int _flags;

        unsigned long long _correction_field;

        const unsigned long _reserved3;

        unsigned long _source_port_id_lp; // this field originally has 10 bytes (!)
        unsigned long _source_port_id_mp; // they were divided in left, midst and right parts
        unsigned short int _source_port_id_rp; // they could be turned into an array, though

        unsigned short int _sequence_id;

        unsigned char _control_field;
        unsigned char _log_message_interval;
    } __attribute__((packed));

    class Event_Message: private Header
    {
    public:
//        Event_Message(): Header(), _t(0,0) {}
        Event_Message(unsigned long long seconds, unsigned int nanoseconds):
            Header() {
            _origin_timestamp_seconds[0] = (short int) (seconds >> 4);
            _origin_timestamp_seconds[1] = (short int) (seconds >> 2);
            _origin_timestamp_seconds[2] = (short int) seconds;
            _origin_timestamp_nanoseconds = nanoseconds;
        }

        Header * header() {
            return this;
        }

        void origin_seconds(unsigned long long s) {
            _origin_timestamp_seconds[0] = (short int) (s >> 4);
            _origin_timestamp_seconds[1] = (short int) (s >> 2);
            _origin_timestamp_seconds[2] = (short int) s;
        }
        void origin_nanoseconds(unsigned long n) {
            _origin_timestamp_nanoseconds = n;
        }

        unsigned long long origin_seconds() {
            long long s0 = _origin_timestamp_seconds[0],
                      s1 = _origin_timestamp_seconds[1],
                      s2 = _origin_timestamp_seconds[2];

            return ((s0 << 32) | (s1 << 16) | s2);
        }
        unsigned long origin_nanoseconds() {
            return _origin_timestamp_nanoseconds;
        }

        void origin_timestamp(Timestamp * t) {
            origin_seconds(t->seconds());
            origin_nanoseconds(t->nanoseconds());
        }

        Timestamp * receive_timestamp() {
            return new Timestamp(origin_seconds(), origin_nanoseconds());
        }

    private:
        unsigned short int _origin_timestamp_seconds[3];
        unsigned long _origin_timestamp_nanoseconds;
    }__attribute__((packed));

    class Delay_Resp_Message: private Header
    {
    public:
        Delay_Resp_Message(unsigned long long seconds, unsigned int nanoseconds,
                unsigned int oui, unsigned long long uuid, unsigned short int node_port_number = 1):
            Header() {
            _receive_timestamp_seconds[0] = (short int) (seconds >> 4);
            _receive_timestamp_seconds[1] = (short int) (seconds >> 2);
            _receive_timestamp_seconds[2] = (short int) seconds;
            _receive_timestamp_nanoseconds = nanoseconds;

            _request_port_id_lp = (oui < 8) | (uuid >> 32);
            _request_port_id_mp = (unsigned int) uuid;
            _request_port_id_rp = node_port_number;
        }

        Header * header() {
            return this;
        }

        void receive_seconds(unsigned long long s) {
            _receive_timestamp_seconds[0] = (short int) (s >> 4);
            _receive_timestamp_seconds[1] = (short int) (s >> 2);
            _receive_timestamp_seconds[2] = (short int) s;
        }
        void receive_nanoseconds(unsigned long n) {
            _receive_timestamp_nanoseconds = n;
        }

        unsigned long long receive_seconds() {
            long long s0 = _receive_timestamp_seconds[0],
                      s1 = _receive_timestamp_seconds[1],
                      s2 = _receive_timestamp_seconds[2];

            return ((s0 << 32) | (s1 << 16) | s2);
        }
        unsigned long receive_nanoseconds() {
            return _receive_timestamp_nanoseconds;
        }

        void receive_timestamp(Timestamp * t) {
            receive_seconds(t->seconds());
            receive_nanoseconds(t->nanoseconds());
        }

        Timestamp * receive_timestamp() {
            return new Timestamp(receive_seconds(), receive_nanoseconds());
        }

    private:
        unsigned short int _receive_timestamp_seconds[3];
        unsigned long _receive_timestamp_nanoseconds;

        unsigned int _request_port_id_lp; // this field originally has 10 bytes (!)
        unsigned int _request_port_id_mp; // they were divided in left, midst and right parts
        unsigned short int _request_port_id_rp; // they could be turned into an array, though
    } __attribute__((packed));

public:
    PTP(bool master/* = Traits<PTP>::MASTER TODO*/) : _master(master), t1(0), t2(0), t3(0), t4(0),
    _current_sync_id(Random::random()), _current_delay_req_id(Random::random()), _c() {
        db<PTP>(TRC) << "PTP::PTP() => " << this << endl;

        _nic.attach(this, NIC::PTP);
    }
    ~PTP() {
        _nic.detach(this, NIC::PTP);
    }

    bool synchronize_slaves(unsigned char domain_number = 0);

    Address address() {
        return _nic.address();
    }

    void update(NIC::Observed *, NIC::Protocol, Buffer *);

private:
    Timestamp * now(unsigned int id = 0);

private:
    NIC _nic;

    const bool _master;

    Timestamp * t1;
    Timestamp * t2;
    Timestamp * t3;
    Timestamp * t4;

    unsigned short int _current_sync_id; // Each new Sync shall increment this number. Follow Ups should have the same as their Syncs.
    unsigned short int _current_delay_req_id; // Each new Delay Requisition shall increment this number. Delay Responses should have the same as their syncs.
    // IDs are chosen and incremented per type of message per destination address, the destination is always "broadcast", so it's ignored.

    Condition _c;
};

__END_SYS

#endif

// Sources are scarce and contradictory, but here they are:
// http://www.ieee802.org/1/files/public/docs2007/as-garner-protocol-state-machines-frame-formats-0307.pdf
// http://wiki.hevs.ch/uit/index.php5/Standards/Ethernet_PTP/frames
// http://www.chronos.co.uk/files/pdfs/cal/TechnicalBrief-IEEE1588v2PTP.pdf
// http://www.ieee1588.com/IEEE1588_Functional_Principle_of_PTP.html
// http://ptfinc.com/dsheets/App_23_part%202_PTP.pdf
// http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=view&target=ptpv2.pcap
// http://wiki.wireshark.org/Protocols/ptp
// Wireshark (the program) used to analyze .pcap files generated
