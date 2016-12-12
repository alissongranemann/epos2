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
    typedef IEEE802_15_4 Phy_Layer;

    enum State {
        UPDATE_TX_SCHEDULE = 0,
        SLEEP_S            = 1,
        RX_MF              = 2,
        SLEEP_DATA         = 3,
        RX_DATA            = 4,
        BACKOFF            = 5,
        CCA                = 6,
        TX_MF              = 7,
        TX_DATA            = 8,
    };

public:
    using TSTP_Common::Address;
    using TSTP_Common::Header;
    using TSTP_Common::Frame;
    typedef typename Radio::Timer Timer;
    typedef typename Radio::Timer::Time_Stamp Time_Stamp;

    static const unsigned int MTU = Frame::MTU;

private:
public: //TODO: for debugging

    static const bool promiscuous = Traits<NIC>::promiscuous;

    static const unsigned int INT_HANDLING_DELAY = 9; // Time delay between scheduled tx_mf interrupt and actual Radio TX
    static const unsigned int TX_DELAY = INT_HANDLING_DELAY + Radio::RX_TO_TX_DELAY;

    static const unsigned int Tu = IEEE802_15_4::TURNAROUND_TIME;
    static const unsigned int Ti = Tu + Radio::RX_TO_TX_DELAY + INT_HANDLING_DELAY + 100; // 100us margin for delay between Microframes // FIXME
    static const unsigned int TIME_BETWEEN_MICROFRAMES = Ti;
    static const unsigned int Ts = (sizeof(Microframe) + Phy_Layer::PHY_HEADER_SIZE) * 1000000ull
                                    / Phy_Layer::BYTE_RATE
                                    + Radio::TX_TO_RX_DELAY; // Time to send a single Microframe (including PHY headers)
    static const unsigned int MICROFRAME_TIME = Ts;
    static const unsigned int Tr = 2*Ts + Ti + Tu;
    static const unsigned int RX_MF_TIMEOUT = Tr;

    static const unsigned int NMF = 1 + (((1000000ull * Tr) / Traits<System>::DUTY_CYCLE) + (Ti + Ts) - 1) / (Ti + Ts);
    static const unsigned int N_MICROFRAMES = NMF;

    static const unsigned int CI = Ts + (NMF - 1) * (Ts + Ti);
    static const unsigned int PERIOD = CI;
    static const unsigned int SLEEP_PERIOD = CI - RX_MF_TIMEOUT;

    static const typename IF<(Tr * 1000000ull / CI <= Traits<System>::DUTY_CYCLE), unsigned int, void>::Result
        DUTY_CYCLE = Tr * 1000000ull / CI; // in ppm. This line failing means that TSTP_MAC is unable to provide a duty cycle smaller than or equal to Traits<System>::DUTY_CYCLE

    static const unsigned int DATA_LISTEN_MARGIN = TIME_BETWEEN_MICROFRAMES / 2; // Subtract this amount when calculating time until data transmission
    static const unsigned int DATA_SKIP_TIME = DATA_LISTEN_MARGIN + 4500;

    static const unsigned int RX_DATA_TIMEOUT = DATA_SKIP_TIME + DATA_LISTEN_MARGIN;

    static const unsigned int G = IEEE802_15_4::CCA_TX_GAP + RX_MF_TIMEOUT;
    static const unsigned int CCA_TIME = G;

protected:
    TSTP_MAC() {}

    // Called after the Radio's constructor
    void constructor_epilogue() {
        update_tx_schedule(0);
    }

    // Filter and assemble RX Buffer Metainformation
    bool pre_notify(Buffer * buf) {
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "pre_notify(buf=" << buf << ")" << endl;

        if(_in_rx_mf) { // State: RX MF (part 2/3)
            if(buf->size() == sizeof(Microframe)) {

                Timer::int_disable();

                Radio::power(Power_Mode::SLEEP);
                buf->sfd_time_stamp = Timer::sfd();

                _in_rx_mf = false;

                Microframe * mf = buf->frame()->data<Microframe>();
                Frame_ID id = mf->id();

                // Clear scheduled messages with same ID
                for(Buffer::Element * el = _tx_schedule.head(); el; el = el->next()) {
                    Buffer * b = el->object();
                    if(b->id == id) {

                        // Frame ID seen before. Skip it.

                        if(!b->destined_to_me) {
                            db<TSTP_MAC<Radio>>(INF) << "TSTP_MAC::pre_notify: ACK received: " << *mf << " at " << Radio::Timer::count2us(buf->sfd_time_stamp) << endl;
                            _tx_schedule.remove(el);
                            delete b;
                        }

                        Time_Stamp data_time = buf->sfd_time_stamp + Timer::us2count(TIME_BETWEEN_MICROFRAMES) + mf->count() * Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME) - Timer::us2count(DATA_LISTEN_MARGIN);

                        //kout << SLEEP_DATA ;
                        // State: Sleep until Data
                        Timer::interrupt(data_time + Timer::us2count(DATA_SKIP_TIME), update_tx_schedule);

                        return false;
                    }
                }

                // Initialize Buffer Metainformation
                buf->id = id;
                buf->downlink = mf->all_listen();
                buf->is_new = false;
                buf->is_microframe = true;
                buf->relevant = promiscuous || mf->all_listen();
                buf->trusted = false;
                buf->sender_distance = mf->hint();

                // Forge a TSTP identifier to make the radio notify listeners
                mf->id(TSTP << 8);

                return true;
            }
            return false;
        } else if(_in_rx_data) { // State: RX Data (part 2/3)

            if(buf->size() == sizeof(Microframe))
                return false;

            Radio::power(Power_Mode::SLEEP);

            // Initialize Buffer Metainformation
            buf->sfd_time_stamp = Timer::sfd();
            buf->id = _receiving_data_id;
            buf->sender_distance = _receiving_data_hint;
            buf->is_new = false;
            buf->is_microframe = false;
            buf->trusted = false;
            buf->freed = false;

            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::pre_notify: Frame received: " << buf->frame() << " at " << Radio::Timer::count2us(buf->sfd_time_stamp) << endl;

            return true;
        } else
            return false;
    }

    bool post_notify(Buffer * buf) {
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "post_notify(buf=" << buf << ")" << endl;

        if(buf->is_microframe) { // State: RX MF (part 3/3)
            Microframe * mf = buf->frame()->data<Microframe>();
            Time_Stamp data_time = buf->sfd_time_stamp + Timer::us2count(TIME_BETWEEN_MICROFRAMES) + mf->count() * Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME) - Timer::us2count(DATA_LISTEN_MARGIN);

            //kout << SLEEP_DATA;

            if(buf->relevant) { // Transition: [Relevant MF]
                _receiving_data_id = buf->id;
                _receiving_data_hint = mf->hint();
                // State: Sleep until Data
                Timer::interrupt(data_time, rx_data);
            } else // Transition: [Irrelevant MF]
                Timer::interrupt(data_time + Timer::us2count(DATA_SKIP_TIME), update_tx_schedule);

            if(Traits<TSTP_MAC>::debugged) {
                mf->id(_receiving_data_id);
                db<TSTP_MAC<Radio>>(INF) << "[RX][TSTP Microframe] " << *mf << endl;
            }

            free(buf);
            return true;
        } else { // State: RX Data (part 3/3)
            // Next state was already scheduled at pre_notify
            //if(buf->freed) { // TODO
            //    return false;
            //} else {
                free(buf);
                return true;
            //}
        }
    }

public:
    // Assemble TX Buffer Metainformation
    void marshal(Buffer * buf, const Address & src, const Address & dst, const Type & type) {
        buf->id = Random::random() & 0x0fff;// TODO
        buf->is_microframe = false;
        buf->trusted = false;
        buf->is_new = true;
    }

    unsigned int unmarshal(Buffer * buf, Address * src, Address * dst, Type * type, void * data, unsigned int size) {
        *src = Address::BROADCAST;
        *dst = Address::BROADCAST;
        *type = buf->frame()->data<Header>()->version();
        memcpy(data, buf->frame()->data<Frame>(), min(buf->size(), size));
        return buf->size();
    }

    int send(Buffer * buf) {
        // Components calculate the offset in microseconds according to their own metrics.
        // We finish the calculation here to keep SLEEP_PERIOD, G, and Timestamps
        // encapsulated by the MAC, and MAC::marshal() happens before the other components' marshal methods
        if(buf->destined_to_me)
            buf->offset = Timer::us2count(CCA_TIME);
        else {
            buf->offset = Timer::us2count(((buf->offset * SLEEP_PERIOD) / (G*RADIO_RANGE)) * G);

            if(buf->offset < Timer::us2count(2*CCA_TIME))
                buf->offset = Timer::us2count(2*CCA_TIME);
            else if(buf->offset > Timer::us2count(SLEEP_PERIOD - CCA_TIME))
                buf->offset = Timer::us2count(SLEEP_PERIOD - CCA_TIME);
        }

        _tx_schedule.insert(buf->link());

        return buf->size();
    }

private:
    // State Machine

    static void update_tx_schedule(const IC::Interrupt_Id & id) {
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::update_tx_schedule(id=" << id << ")" << endl;

        // State: Update TX Schedule
        Radio::power(Power_Mode::SLEEP);
        _in_rx_data = false;
        _in_rx_mf = false;

        //kout << UPDATE_TX_SCHEDULE ;

        _tx_pending = 0;

        Time_Stamp now_ts = Timer::read();
        Microsecond now_us = Timer::count2us(now_ts);

        // Fetch next message and remove expired ones
        // TODO: Turn _tx_schedule into an ordered list
        for(Buffer::Element * el = _tx_schedule.head(); el; ) {
            Buffer::Element * next = el->next();
            Buffer * b = el->object();
            if(drop_expired && (b->deadline <= now_us)) {
                _tx_schedule.remove(el);
                delete b;
            } else if((!_tx_pending) || ((!_tx_pending->destined_to_me) && ((b->destined_to_me) || (_tx_pending->deadline > b->deadline))))
                if(!promiscuous)
                    _tx_pending = b;
            el = next;
        }

        if(_tx_pending) { // Transition: [TX pending]

            // State: Backoff CCA (Backoff part)
            new (&_mf) Microframe((!_tx_pending->destined_to_me) && _tx_pending->downlink, _tx_pending->id, N_MICROFRAMES - 1, _tx_pending->my_distance);
            Radio::power(Power_Mode::LIGHT);
            Radio::copy_to_nic(&_mf, sizeof(Microframe));

            //kout << BACKOFF ;
            Timer::interrupt(now_ts + _tx_pending->offset, cca);
        } else { // Transition: [No TX pending]
            // State: Sleep S
            //kout << SLEEP_S ;
            Timer::interrupt(now_ts + Timer::us2count(SLEEP_PERIOD), rx_mf);
        }
    }

    // State: Backoff CCA (CCA part)
    static void cca(const IC::Interrupt_Id & id) {
        //kout << CCA ;
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::cca(id=" << id << ")" << endl;

        assert(N_MICROFRAMES > 1);

        Radio::listen();

        // Try to send the first Microframe
        if(Radio::cca(CCA_TIME)) {
            _mf_time = Timer::read();
            if(Radio::transmit()) { // Transition: [Channel free]
                _mf_time += Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME);
                _mf.dec_count();
                // Prioritize retransmissions of this buffer by a random ammount to avoid permanent collision with another node
                if(_tx_pending->offset > 2 * Timer::us2count(CCA_TIME))
                    _tx_pending->offset -= Random::random() % (2 * Timer::us2count(CCA_TIME));
                //kout << TX_MF ;
                while(!Radio::tx_done());
                Radio::copy_to_nic(&_mf, sizeof(Microframe));
                Timer::interrupt(_mf_time, tx_mf);
            } else { // Transition: [Channel busy]
                // Prioritize retransmissions of this buffer by a random ammount to avoid permanent collision with another node
                if(_tx_pending->offset > 2 * Timer::us2count(CCA_TIME))
                    _tx_pending->offset -= Random::random() % (2u * Timer::us2count(CCA_TIME));
                rx_mf(0);
            }
        } else { // Transition: [Channel busy]
            // Prioritize retransmissions of this buffer by a random ammount to avoid permanent collision with another node
            if(_tx_pending->offset > 2 * Timer::us2count(CCA_TIME))
                _tx_pending->offset -= Random::random() % (2u * Timer::us2count(CCA_TIME));
            rx_mf(0);
        }
    }

    // State: RX MF (part 1/3)
    static void rx_mf(const IC::Interrupt_Id & id) {
        //kout << RX_MF ;
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::rx_mf(id=" << id << ")" << endl;

        _in_rx_mf = true;

        // If timeout is reached, Transition: [No MF]
        Timer::interrupt(Timer::read() + Timer::us2count(RX_MF_TIMEOUT), update_tx_schedule);

        Radio::power(Power_Mode::FULL);
        Radio::listen();
    }

    // State: RX Data (part 1/3)
    static void rx_data(const IC::Interrupt_Id & id) {
        //kout << RX_DATA ;
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::rx_data(id=" << id << ")" << endl;

        _in_rx_data = true;
        _in_rx_mf = false;

        // Set timeout
        Timer::interrupt(Timer::read() + Timer::us2count(RX_DATA_TIMEOUT), update_tx_schedule);

        Radio::power(Power_Mode::FULL);
        Radio::listen();
    }

    // State: TX MFs
    static void tx_mf(const IC::Interrupt_Id & id) {
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::tx_mf(id=" << id << ")" << endl;

        // The first Microframe is sent at cca()
        Radio::transmit_no_cca();

        _mf_time += Timer::us2count(TIME_BETWEEN_MICROFRAMES + MICROFRAME_TIME);

        if(_mf.dec_count() > 0) {
            while(!Radio::tx_done());
            Radio::copy_to_nic(&_mf, sizeof(Microframe));
            Timer::interrupt(_mf_time, tx_mf);
        } else {
            // The precise time when this frame is actually sent by the physical layer
            _tx_pending->frame()->data<Header>()->last_hop_time(_mf_time + Timer::us2count(TX_DELAY + Tu));
            while(!Radio::tx_done());
            Radio::copy_to_nic(_tx_pending->frame(), _tx_pending->size());
            Timer::interrupt(_mf_time, tx_data);
        }
    }

    static void tx_data(const IC::Interrupt_Id & id) {
        //kout << TX_DATA;
        if(Traits<TSTP_MAC>::hysterically_debugged)
            db<TSTP_MAC<Radio>>(TRC) << "TSTP_MAC::tx_data(id=" << id << ")" << endl;

        if(!_tx_pending->destined_to_me) { // Transition: [Is not dest.]
            // State: TX Data
            Radio::transmit_no_cca();
            while(!Radio::tx_done());

            _mf_time = Timer::read();
        } else { // Transition: [Is dest.]
            _tx_schedule.remove(_tx_pending);
            delete _tx_pending;
        }

        // State: Sleep S
        Radio::power(Power_Mode::SLEEP);
        //kout << SLEEP_S;
        Timer::interrupt(_mf_time + Timer::us2count(SLEEP_PERIOD), rx_mf);
    }

    static void free(Buffer * b);

    static Microframe _mf;
    static Time_Stamp _mf_time;
    static Frame_ID _receiving_data_id;
    static Hint _receiving_data_hint;
    static Buffer::List _tx_schedule;
    static Buffer * _tx_pending;
    static bool _in_rx_mf;
    static bool _in_rx_data;
};

// The compiler makes sure that template static variables are only defined once

// Class attributes
template<typename Radio>
TSTP_Common::Microframe TSTP_MAC<Radio>::_mf;

template<typename Radio>
typename TSTP_MAC<Radio>::Time_Stamp TSTP_MAC<Radio>::_mf_time;

template<typename Radio>
TSTP_Common::Frame_ID TSTP_MAC<Radio>::_receiving_data_id;

template<typename Radio>
typename TSTP_MAC<Radio>::Hint TSTP_MAC<Radio>::_receiving_data_hint;

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
