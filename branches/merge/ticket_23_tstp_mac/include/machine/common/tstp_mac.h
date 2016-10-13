// EPOS Trustful SpaceTime Protocol MAC Declarations

#ifndef __tstp_mac_h
#define __tstp_mac_h

// Include only TSTP_Common from tstp.h
#ifdef __tstp_h
#include <tstp.h>
#else
#define __tstp_h
#include <tstp.h>
#undef __tstp_h
#endif

#include <ic.h>
#include <utility/random.h>
#include <utility/math.h>

__BEGIN_SYS

template<typename Radio>
class TSTP_MAC: public TSTP_Common, public TSTP_Common::Observed, public Radio
{
public:
    using TSTP_Common::Address;
    using TSTP_Common::Header;
    using TSTP_Common::Frame;
    typedef typename Radio::Timer Timer;
    typedef typename Radio::Timer::Time_Stamp Time_Stamp;

    static const unsigned int MTU = Frame::MTU;

    typedef _UTIL::Buffer<NIC, Phy_Frame, void, TSTP_Metadata> Buffer;

//private:

    //TODO: Check. Maybe move something to Traits
    static const bool drop_expired = true;
    static const unsigned int PERIOD = 225000;
    static const unsigned int CI = PERIOD;
    static const unsigned int Tu = 192; // IEEE 802.15.4 TX Turnaround Time
    static const unsigned int G = Tu + 128; // Tu + 8 / symbol_rate
    static const unsigned int Ts = 480; // Time to send a single microframe (including PHY headers)
    //static const unsigned int Ts = 580 - Tu; // Time to send a single microframe (including PHY headers)
    static const unsigned int MICROFRAME_TIME = Ts;
    static const unsigned int MIN_Ti = G;//Tu;// Minimum time between consecutive microframes
    static const unsigned int TX_UNTIL_PROCESS_DATA_DELAY = 0;//5100; //TODO
    static const unsigned long long RADIO_RADIUS = 1700;

    //static const unsigned int NMF = (1 + ((CI - Ts) / (MIN_Ti + Ts))) > 0xfff ? 0xfff :
    //                                (1 + ((CI - Ts) / (MIN_Ti + Ts)));
    static const unsigned int NMF = 5;
    static const unsigned int N_MICROFRAMES = NMF;
    static const unsigned int Ti = (CI - Ts) / (NMF - 1) - Ts;
    static const unsigned int TIME_BETWEEN_MICROFRAMES = Ti;
    static const unsigned int DATA_LISTEN_MARGIN = TIME_BETWEEN_MICROFRAMES / 2; // Subtract this amount when calculating time until data transmission
    static const unsigned int DATA_SKIP_TIME = DATA_LISTEN_MARGIN + 4500;
    static const unsigned int Tr = 2*Ts + Ti;
    static const unsigned int RX_MF_TIMEOUT = Tr;
    static const unsigned int S = PERIOD - RX_MF_TIMEOUT;
    static const unsigned int SLEEP_PERIOD = S;
    static const unsigned int DUTY_CYCLE = (RX_MF_TIMEOUT * 1000000) / PERIOD; //ppm

    static const unsigned int RX_DATA_TIMEOUT = DATA_SKIP_TIME + DATA_LISTEN_MARGIN + 4 * (MICROFRAME_TIME + TIME_BETWEEN_MICROFRAMES);
    static const unsigned int CCA_TIME = (2 * MICROFRAME_TIME + TIME_BETWEEN_MICROFRAMES) > 256 ? (2 * MICROFRAME_TIME + TIME_BETWEEN_MICROFRAMES) : 256;

protected:
    TSTP_MAC() {}

    // Called after the Radio's constructor
    void constructor_epilogue() {
        update_tx_schedule(0);
    }

    // Filter and assemble RX Buffer Metainformation
    bool pre_notify(Buffer * buf) {
        db<TSTP_MAC<Radio>>(TRC) << "pre_notify(buf=" << buf << ")" << endl;
        assert(_in_rx_mf || _in_rx_data);

        if(_in_rx_mf) { // State: RX MF (part 2/3)
            if(buf->size() == sizeof(Microframe)) {
                Timer::int_disable();
                Radio::power(Power_Mode::SLEEP);

                Microframe * mf = buf->frame()->data<Microframe>();
                Frame_ID id = mf->id();

                // Initialize Buffer Metainformation
                buf->sfd_time_stamp = Timer::sfd();
                buf->id = id;
                buf->downlink = mf->all_listen();
                buf->is_new = true;
                buf->is_microframe = true;
                buf->relevant = mf->all_listen();
                buf->trusted = false;

                // Clear scheduled messages with same ID
                for(Buffer::Element * el = _tx_schedule.head(); el; ) {
                    Buffer::Element * next = el->next();
                    Buffer * b = el->object();
                    if(b->id == id) {
                        _tx_schedule.remove(el);
                        delete b;
                    }
                    el = next;
                }

                return true;
            }
            return false;
        } else { // State: RX Data (part 2/3)
            if(buf->size() == sizeof(Microframe))
                return false;
            // Initialize Buffer Metainformation
            buf->sfd_time_stamp = Timer::sfd();
            buf->id = _receiving_data_id;
            buf->sender_distance = _receiving_data_hint;
            buf->is_new = true;
            buf->is_microframe = false;
            buf->relevant = true;
            buf->trusted = false;
            buf->offset = 1000000;

            return true;
        }
    }

    bool post_notify(Buffer * buf) {
        assert(_in_rx_mf || _in_rx_data);

        if(_in_rx_mf) { // State: RX MF (part 3/3)
            Microframe * mf = buf->frame()->data<Microframe>();
            Time_Stamp data_time = buf->sfd_time_stamp + Timer::us2count(TIME_BETWEEN_MICROFRAMES) + static_cast<Time_Stamp>(mf->count()) * Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME) - Timer::us2count(DATA_LISTEN_MARGIN);

            if(buf->relevant) { // Transition: [Relevant MF]
                _receiving_data_id = buf->id;
                _receiving_data_hint = mf->hint();
                free(buf);
                // State: Sleep until Data
                Timer::interrupt(data_time, rx_data);
                return true;
            } else { // Transition: [Irrelevant MF]
                Timer::interrupt(data_time + Timer::us2count(DATA_SKIP_TIME), update_tx_schedule);
                return false;
            }
        } else { // State: RX Data (part 3/3)
            // Normalize offset calculated by the router
            long long dist = abs(buf->my_distance - (buf->sender_distance - RADIO_RADIUS));
            long long betha = (G * RADIO_RADIUS * 1000000) / (dist * G);
            buf->offset = buf->offset * betha / 1000000;

            // Introduce Euclidean Distance component to offset
            buf->offset = (((buf->offset * dist * 1000000) / (G * RADIO_RADIUS / SLEEP_PERIOD)) * G) / 1000000;

            update_tx_schedule(0);
            return false;
        }
    }

public:
    // Assemble TX Buffer Metainformation
    void marshal(Buffer * buf, const Address & src, const Address & dst, const Type & type) {
        buf->id = Random::random() & 0xfff;// TODO
        unsigned long long r = Random::random();
        unsigned long long mod = SLEEP_PERIOD / G + 1;
        buf->offset = (r % mod) * G * 1000000;
        buf->is_microframe = false;
        buf->trusted = false;
        buf->destined_to_me = false; // TODO
        buf->tx_time = Timer::count2us(Timer::read());// TODO
        buf->deadline = buf->tx_time + SLEEP_PERIOD * 10;// TODO
        buf->downlink = true;// TODO
    }

    unsigned int unmarshal(Buffer * buf, Address * src, Address * dst, Type * type, void * data, unsigned int size) { /*TODO*/ return 0; }

    int send(Buffer * buf) {
        _tx_schedule.insert(buf->link());
        return 0;
    }

private:

    // State Machine

    static void update_tx_schedule(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::update_tx_schedule(id=" << id << ")" << endl;
        // State: Update TX Schedule
        Radio::power(Power_Mode::SLEEP);
        _in_rx_data = false;
        _in_rx_mf = false;

        _tx_pending = 0;

        Time_Stamp now_ts = Timer::read();
        Time_Stamp now_us = Timer::count2us(now_ts);

        // Fetch next message and remove expired ones
        // TODO: Turn _tx_schedule into an ordered list
        for(Buffer::Element * el = _tx_schedule.head(); el; ) {
            Buffer::Element * next = el->next();
            Buffer * b = el->object();
            if(drop_expired && (b->deadline <= now_us)) {
                _tx_schedule.remove(el);
                delete b;
            } else if((b->tx_time <= now_us) && ((!_tx_pending) || (_tx_pending->deadline >= b->deadline)))
                _tx_pending = b;
            el = next;
        }

        if(_tx_pending) { // Transition: [TX pending]
            // State: Backoff CCA (Backoff part)
            new (&_mf) Microframe(_tx_pending->downlink, _tx_pending->id, N_MICROFRAMES - 1, _tx_pending->my_distance);
            Radio::power(Power_Mode::LIGHT);
            Radio::copy_to_nic(&_mf, sizeof(Microframe));
            Radio::listen();

            Timer::interrupt(Timer::read() + Timer::us2count(_tx_pending->offset) / 1000000, cca);
        } else { // Transition: [No TX pending]
            // State: Sleep S
            Timer::interrupt(now_ts + Timer::us2count(SLEEP_PERIOD), rx_mf);
        }
    }

    // State: Backoff CCA (CCA part)
    static void cca(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::cca(id=" << id << ")" << endl;
        assert(N_MICROFRAMES > 1);
        // Try to send the first Microframe
        if(Radio::cca(CCA_TIME)) {
            _mf_time = Timer::read();
            if(Radio::transmit()) { // Transition: [Channel free]
                _mf_time += Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME);
                _mf.dec_count();
                while(!Radio::tx_done());
                Radio::copy_to_nic(&_mf, sizeof(Microframe));
                Timer::interrupt(_mf_time, tx_mf);
            } else // Transition: [Channel busy]
                rx_mf(0);
        } else // Transition: [Channel busy]
            rx_mf(0);
    }

    // State: RX MF (part 1/3)
    static void rx_mf(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::rx_mf(id=" << id << ")" << endl;
        //FIXME: no need to set both flags
        _in_rx_mf = true;
        _in_rx_data = false;
        Radio::power(Power_Mode::FULL);
        Radio::listen();

        // If timeout is reached, Transition: [No MF]
        Timer::interrupt(Timer::read() + Timer::us2count(RX_MF_TIMEOUT), update_tx_schedule);
    }

    // State: RX Data (part 1/3)
    static void rx_data(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::rx_data(id=" << id << ")" << endl;
        //FIXME: no need to set both flags
        _in_rx_data = true;
        _in_rx_mf = false;
        Radio::power(Power_Mode::FULL);
        Radio::listen();

        // Set timeout
        Timer::interrupt(Timer::read() + Timer::us2count(RX_DATA_TIMEOUT), update_tx_schedule);
    }

    // State: TX MFs
    static void tx_mf(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::tx_mf(id=" << id << ")" << endl;
        // The first Microframe is sent at cca()
        Radio::transmit_no_cca();

        _mf_time += Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME);

        while(!Radio::tx_done());

        if(_mf.dec_count() > 0) {
            Radio::copy_to_nic(&_mf, sizeof(Microframe));
            Timer::interrupt(_mf_time, tx_mf);
        } else {
            Radio::copy_to_nic(_tx_pending->frame(), _tx_pending->size());
            Timer::interrupt(_mf_time, tx_data);
        }
    }

    static void tx_data(const IC::Interrupt_Id & id) {
        db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::tx_data(id=" << id << ")" << endl;
        if(!_tx_pending->destined_to_me) { // Transition: [Is not dest.]
            //hdr->last_hop_time(ts); // TODO
            //hdr->elapsed(hdr->elapsed() + TSTP_Timer::ts_to_us(ts - _tx_pending->sfd_time_stamp())); // TODO

            // State: TX Data
            Radio::transmit_no_cca();
            while(!Radio::tx_done());

            _mf_time = Timer::read();
        } else { // Transition: [Is dest.]
            assert(_tx_pending);
            _tx_schedule.remove(_tx_pending);
            delete _tx_pending;
        }

        // State: Sleep S
        Radio::power(Power_Mode::SLEEP);
        Timer::interrupt(_mf_time + Timer::us2count(SLEEP_PERIOD), rx_mf);
    }

    static void free(Buffer * b);

    static Microframe _mf;
    static Time_Stamp _mf_time;
    static Frame_ID _receiving_data_id;
    static long _receiving_data_hint;
    static Buffer::List _tx_schedule;
    static Buffer * _tx_pending;
    static bool _in_rx_mf;
    static bool _in_rx_data;
};

template<typename Radio>
TSTP_Common::Microframe TSTP_MAC<Radio>::_mf;

template<typename Radio>
typename TSTP_MAC<Radio>::Time_Stamp TSTP_MAC<Radio>::_mf_time;

template<typename Radio>
TSTP_Common::Frame_ID TSTP_MAC<Radio>::_receiving_data_id;

template<typename Radio>
long TSTP_MAC<Radio>::_receiving_data_hint;

template<typename Radio>
TSTP_MAC<Radio>::Buffer::List TSTP_MAC<Radio>::_tx_schedule;

template<typename Radio>
typename TSTP_MAC<Radio>::Buffer * TSTP_MAC<Radio>::_tx_pending;

template<typename Radio>
bool TSTP_MAC<Radio>::_in_rx_mf;

template<typename Radio>
bool TSTP_MAC<Radio>::_in_rx_data;

__END_SYS

#endif
