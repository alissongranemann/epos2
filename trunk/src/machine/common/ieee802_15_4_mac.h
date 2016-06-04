// EPOS IEEE 802.15.4 MAC Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <nic.h>

__BEGIN_SYS

// Class attributes

// Methods
bool IEEE802_15_4_MAC::wait_for_ack()
{
//    while(!(sfr(RFIRQF1) & INT_TXDONE));
//    sfr(RFIRQF1) &= ~INT_TXDONE;
//
////    if(not Traits<CC2538>::auto_listen) {
//        xreg(RFST) = ISRXON;
////    }
//
//    bool acked = false;
//    eMote3_GPTM timer(2, Traits<CC2538>::ACK_TIMEOUT);
//    timer.enable();
//    while(timer.running() and not (acked = (sfr(RFIRQF0) & INT_FIFOP)));
//
//    return acked;
    return true;
}

bool IEEE802_15_4_MAC::send_and_wait(bool ack)
{
    bool do_ack = Traits<_API::ELP>::acknowledged && ack;
    Reg32 saved_filter_settings = 0;
    if(do_ack) {
        saved_filter_settings = xreg(FRMFILT1);
        xreg(RFIRQM0) &= ~INT_FIFOP; // Disable FIFOP int. We'll poll the interrupt flag
        xreg(FRMFILT1) = ACCEPT_FT2_ACK; // Accept only ACK frames now
    }

    bool sent = backoff_and_send();

    if(do_ack) {
        bool acked = sent and wait_for_ack();

        for(unsigned int i = 0; !acked && (i < Traits<_API::ELP>::RETRIES); i++) {
            db<CC2538>(TRC) << "CC2538::retransmitting" << endl;
            sent = backoff_and_send();
            acked = sent && wait_for_ack();
        }

        if(acked) {
            sfr(RFIRQF0) &= ~INT_FIFOP; // Clear FIFOP flag
            clear_rxfifo();
        }

        // TODO: how does the following maps to higher level protocols?
//        if(not Traits<CC2538>::auto_listen) {
//            xreg(RFST) = ISRFOFF;
//        }

        xreg(FRMFILT1) = saved_filter_settings; // Done with ACKs
        xreg(RFIRQM0) |= INT_FIFOP; // Enable FIFOP int
        return acked;
    }
    else if(sent) {
        while(!tx_ok());
    }

    return sent;
}

bool IEEE802_15_4_MAC::backoff_and_send()
{
    bool ret = true;
    if(Traits<_API::ELP>::avoid_collisions) {
        start_cca();

        unsigned int two_raised_to_be = 1;
        unsigned int BE;
        for(BE = 0; BE < CSMA_CA_MIN_BACKOFF_EXPONENT; BE++) {
            two_raised_to_be *= 2;
        }

        unsigned int trials;
        for(trials = 0u; trials < CSMA_CA_MAX_TRANSMISSION_TRIALS; trials++) {
            const auto ubp = CSMA_CA_UNIT_BACKOFF_PERIOD;
            auto delay_time = (Random::random() % two_raised_to_be) * ubp;
            delay_time = delay_time < ubp ? ubp : delay_time;

//            eMote3_GPTM::delay(delay_time, 2);
            for(volatile unsigned int i = 0; i < delay_time; i++); //TODO: replace with timer
            while(!cca_valid());
            if(tx_if_cca()) {
                break; // Success
            }

            if(BE < CSMA_CA_MAX_BACKOFF_EXPONENT) {
                BE++;
                two_raised_to_be *= 2;
            }
        }

        end_cca();

        if(trials >= Traits<_API::ELP>::RETRIES) {
            db<CC2538>(WRN) << "CC2538::backoff_and_send() failed!" << endl;
            ret = false;
        }
    }
    else {
        tx();
    }

    return ret;
}

__END_SYS

#endif
