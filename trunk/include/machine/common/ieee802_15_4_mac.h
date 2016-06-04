// EPOS IEEE 802.15.4 MAC Declarations

#ifndef __ieee802_15_4_mac_h
#define __ieee802_15_4_mac_h

#include <utility/random.h>
#include <ieee802_15_4.h>

__BEGIN_SYS

template<typename Radio>
class IEEE802_15_4_MAC: public IEEE802_15_4, public IEEE802_15_4::Observed, public Radio
{
private:
    static const unsigned int CSMA_CA_MIN_BACKOFF_EXPONENT = 3;
    static const unsigned int CSMA_CA_MAX_BACKOFF_EXPONENT = 5;
    static const unsigned int CSMA_CA_UNIT_BACKOFF_PERIOD = 320; // us
    static const unsigned int CSMA_CA_RETRIES = 4;

    static const unsigned int ACK_TIMEOUT = 4;
//    typedef CPU::Log_Addr Log_Addr;
//    typedef CPU::Phy_Addr Phy_Addr;
//    static const unsigned int MTU = IEEE802_15_4::MTU;

public:
    using IEEE802_15_4::Address;

protected:
    IEEE802_15_4_MAC() {}

public:

    void marshal(IEEE802_15_4::Phy_Frame * frame, const Address & src, const Address & dst, const Type & type, const void * data, unsigned int size) {
        new (frame) Frame(type, src, dst, data, size);
    }

    int send(Buffer * buf) {
        Frame * frame = reinterpret_cast<Frame *>(buf->data()); // Buffer uses Physical Frames
        bool do_ack = Traits<_API::ELP>::acknowledged && (frame->dst() != broadcast());
//        Reg32 saved_filter_settings = 0;
//        if(do_ack) {
//            saved_filter_settings = xreg(FRMFILT1);
//            xreg(RFIRQM0) &= ~INT_FIFOP; // Disable FIFOP int. We'll poll the interrupt flag
//            xreg(FRMFILT1) = ACCEPT_FT2_ACK; // Accept only ACK frames now
//        }

        bool sent = backoff_and_send();

        if(do_ack) {
            bool acked = sent && Radio::wait_for_ack(ACK_TIMEOUT);

            for(unsigned int i = 0; !acked && (i < Traits<_API::ELP>::RETRIES); i++) {
                db<CC2538>(TRC) << "CC2538::retransmitting" << endl;
                sent = backoff_and_send();
                acked = sent && Radio::wait_for_ack(ACK_TIMEOUT);
            }

            if(acked) {
//                sfr(RFIRQF0) &= ~INT_FIFOP; // Clear FIFOP flag
//                clear_rxfifo();
            }

            // TODO: how does the following maps to higher level protocols?
    //        if(not Traits<CC2538>::auto_listen) {
    //            xreg(RFST) = ISRFOFF;
    //        }

//            xreg(FRMFILT1) = saved_filter_settings; // Done with ACKs
//            xreg(RFIRQM0) |= INT_FIFOP; // Enable FIFOP int
            return acked;
        } else if(sent)
            while(!Radio::tx_done());

        return sent;
    }


    bool backoff_and_send() {
        unsigned int exp = 0;
        unsigned int backoff = 1;
        for(; exp < CSMA_CA_MIN_BACKOFF_EXPONENT; exp++)
            backoff *= 2;

        unsigned int retry = 0;
        for(; (retry < CSMA_CA_RETRIES) ; retry++) {
            unsigned int time = (Random::random() % backoff) * CSMA_CA_UNIT_BACKOFF_PERIOD;
            if(time < CSMA_CA_UNIT_BACKOFF_PERIOD)
                time = CSMA_CA_UNIT_BACKOFF_PERIOD;

            if(Radio::cca(time) && Radio::transmit())
                break; // Success

            if(exp < CSMA_CA_MAX_BACKOFF_EXPONENT) {
                exp++;
                backoff *= 2;
            }
        }
        // Radio::normal();
        return (retry < CSMA_CA_RETRIES);
    }
};

__END_SYS

#endif
