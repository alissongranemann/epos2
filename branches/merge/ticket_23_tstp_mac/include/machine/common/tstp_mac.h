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

__BEGIN_SYS

template<typename Radio>
class TSTP_MAC: public TSTP_Common, public TSTP_Common::Observed, public Radio
{
public:
    using TSTP_Common::Address;

protected:
    TSTP_MAC() {}

    // Called after the Radio's constructor
    void constructor_epilogue() {
        Radio::power(Power_Mode::FULL);
        //state_machine(); // TODO
    }

public:
    unsigned int marshal(Buffer * buf) {
        /*
        IEEE802_15_4::Phy_Frame * frame = buf->frame();
        int size = frame->length() - sizeof(Header) + sizeof(Phy_Header) - sizeof(CRC); // Phy_Header is included in Header, but is already discounted in frame_length
        if(size > 0) {
            buf->size(size);
            return buf->size();
        } else
            return 0;
            */
        return 0;
    }

    unsigned int marshal(Buffer * buf, const Address & src, const Address & dst, const Type & type, const void * data, unsigned int size) {
        /*
        if(size > Frame::MTU)
            size = Frame::MTU;
        Frame * frame = new (buf->frame()) Frame(type, src, dst, data, size);
        frame->ack_request(acknowledged && dst != broadcast());
        buf->size(size);
        return size;
            */
        return 0;
    }

    unsigned int unmarshal(Buffer * buf, Address * src, Address * dst, Type * type, void * data, unsigned int size) {
        /*
        Frame * frame = reinterpret_cast<Frame *>(buf->frame());
        unsigned int data_size = frame->length() - sizeof(Header) - sizeof(CRC) + sizeof(Phy_Header); // Phy_Header is included in Header, but is already discounted in frame_length
        if(size > data_size)
            size = data_size;
        *src = frame->src();
        *dst = frame->dst();
        *type = frame->type();
        memcpy(data, frame->data<void>(), size);
        return size;
        */
        return 0;
    }

    int send(Buffer * buf) {
        /*
        bool do_ack = acknowledged && reinterpret_cast<Frame *>(buf->frame())->ack_request();

        Radio::power(Power_Mode::LIGHT);

        Radio::copy_to_nic(buf->frame());
        bool sent, ack_ok;
        ack_ok = sent = backoff_and_send();

        if(do_ack) {
            if(sent) {
                Radio::power(Power_Mode::FULL);
                ack_ok = Radio::wait_for_ack(ACK_TIMEOUT);
            }

            for(unsigned int i = 0; !ack_ok && (i < CSMA_CA_RETRIES); i++) {
                Radio::power(Power_Mode::LIGHT);
                db<IEEE802_15_4_MAC>(TRC) << "IEEE802_15_4_MAC::retransmitting" << endl;
                ack_ok = sent = backoff_and_send();
                if(sent) {
                    Radio::power(Power_Mode::FULL);
                    ack_ok = Radio::wait_for_ack(ACK_TIMEOUT);
                }
            }

            if(!sent)
                Radio::power(Power_Mode::FULL);

        } else {
            if(sent)
                while(!Radio::tx_done());
            Radio::power(Power_Mode::FULL);
        }

        return ack_ok ? buf->size() : 0;
        */
        return 0;
    }

private:
    Microframe _mf;
};

__END_SYS

#endif
