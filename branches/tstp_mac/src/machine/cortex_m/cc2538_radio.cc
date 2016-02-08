// EPOS CC2538 IEEE 802.15.4 NIC Mediator Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/timer.h>
#include "../../../include/machine/cortex_m/cc2538_radio.h"
#include <utility/malloc.h>
#include <alarm.h>
#include <gpio.h>

__BEGIN_SYS

// Class attributes
CC2538::Device CC2538::_devices[UNITS];

void CC2538::set_tx() { _tx_pin->set(); } //TODO:REMOVE
void CC2538::clear_tx() { _tx_pin->clear(); } //TODO:REMOVE
void CC2538::set_rx() { _rx_pin->set(); } //TODO:REMOVE
void CC2538::clear_rx() { _rx_pin->clear(); } //TODO:REMOVE

// Methods
CC2538::~CC2538()
{
    db<CC2538>(TRC) << "~Radio(unit=" << _unit << ")" << endl;
}

bool CC2538::channel_busy()
{
    xreg(FRMCTRL0) |= (3 * RX_MODE); // Disable symbol search
    sfr(RFST) = ISRXON; // CCA requires the receiver to be enabled
    while(not (xreg(RSSISTAT) & RSSI_VALID));
    bool ret = !(xreg(FSMSTAT1) & XREG_FSMSTAT1::CCA);
    off(); // Disable receiver
    xreg(FRMCTRL0) &= ~(3 * RX_MODE); // Enable symbol search
    if(ret) {
        db<CC2538>(WRN) << "CC2538 : Channel is busy!" << endl;
    }
    return ret;
}

void CC2538::off() 
{ 
    sfr(RFST) = ISRFOFF; 
    clear_rxfifo();
    // Clear interrupts
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;
    _rx_pin->clear(); 
    _tx_pin->clear();
}

void CC2538::receive(const Frame_Handler & handler)
{
    _rx_pin->set();
    sfr(RFST) = ISRXON;
    fifop_handler = &handler;
}

int CC2538::send(Buffer * buf)
{
    unsigned int size = 0;

    const bool ack = Traits<CC2538>::ACK; 
    bool frame_sent = true;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

//        db<CC2538>(TRC) << "CC2538::send(buf=" << buf << ")" << endl;

        // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
        // For now, we'll just copy using the RFDATA register
        char * f = reinterpret_cast<char *>(buf->frame());
        sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
        for(int i=0; i<f[0]+1; i++) // First field is length of MAC
            sfr(RFDATA) = f[i];

        // Trigger an immediate send poll
        bool ok = send_and_wait(ack);
        if(ack and not ok) {
            db<CC2538>(INF) << "CC2538::no ack received!" << endl;
            frame_sent = false;
        } else { 
            _statistics.tx_packets++;            
            _statistics.tx_bytes += buf->size();            
        }

        size += buf->size();

//        db<CC2538>(INF) << "CC2538::send done" << endl;
    }

    return frame_sent ? size : -size;
}

void CC2538::reset()
{
    db<CC2538>(TRC) << "Radio::reset()" << endl;

    // Reset statistics
    new (&_statistics) Statistics;
}

void CC2538::address(const Address & address) { }

CC2538::Buffer * CC2538::alloc(NIC * nic, unsigned int payload)
{
    db<CC2538>(TRC) << "CC2538::alloc(pld=" << payload << ")" << endl;

    int max_data = MTU;

    if((payload) / max_data > TX_BUFS) {
        db<CC2538>(WRN) << "CC2538::alloc: sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * TX_BUFS!" << endl;
        return 0;
    }

    Buffer::List pool;

    // Calculate how many frames are needed to hold the transport PDU and allocate enough buffers
    for(int size = payload; size > 0; size -= max_data) {
        // Wait for the next buffer to become free and seize it
        unsigned int i = 1;
        for(bool locked = false; !locked; i++) {
            locked = _tx_buffer[_tx_cur]->lock();
            if(!locked) ++_tx_cur %= TX_BUFS;
            if(i >= 2*TX_BUFS) {
                db<CC2538>(WRN) << "CC2538::alloc: No buffer available! Dropping packet" << endl;
                _statistics.dropped_tx_packets++;
                _statistics.dropped_tx_bytes += size;
                return 0; // No buffer found
            }
        }
        Buffer * buf = _tx_buffer[_tx_cur];

        new (buf) Buffer(nic, (size > max_data) ? MTU : size, true);

        db<CC2538>(INF) << "CC2538::alloc[" << _tx_cur << "]" << endl;

        ++_tx_cur %= TX_BUFS;

        pool.insert(buf->link());
    }

    return pool.head()->object();
}



void CC2538::free(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::free(buf=" << buf << ")" << endl;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

        // Release the buffer to the OS
        buf->unlock();

        db<CC2538>(INF) << "CC2538::free" << endl;
    }
}

void CC2538::set_channel(unsigned int channel)
{
    if((channel < 11) || (channel > 26)) return;
    /*
       The carrier frequency is set by programming the 7-bit frequency word in the FREQ[6:0] bits of the
       FREQCTRL register. Changes take effect after the next recalibration. Carrier frequencies in the range
       from 2394 to 2507 MHz are supported. The carrier frequency f C , in MHz, is given by
       f C = (2394 + FREQCTRL.FREQ[6:0]) MHz, and is programmable in 1-MHz steps.
       IEEE 802.15.4-2006 specifies 16 channels within the 2.4-GHz band. These channels are numbered 11
       through 26 and are 5 MHz apart. The RF frequency of channel k is given by Equation 1.
       f c = 2405 + 5(k –11) [MHz] k [11, 26]
       (1)
       For operation in channel k, the FREQCTRL.FREQ register should therefore be set to
       FREQCTRL.FREQ = 11 + 5 (k – 11).
       */
    xreg(FREQCTRL) = 11+5*(channel-11);
}

bool CC2538::rxfifo_crc_check()
{
    auto end = xreg(RXFIFOCNT); // Number of bytes currently in RXFIFO
    if(end == 0) {
        return false;
    }

    const bool auto_crc_disabled = !(xreg(FRMCTRL0) & AUTO_CRC);
    if(auto_crc_disabled) {
        return true;
    }

    volatile int * rxfifo = reinterpret_cast<volatile int*>(RXFIFO); // RXFIFO mapped in memory
    return rxfifo[end - 1] & AUTO_CRC_OK;
}

// TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
// For now, we'll just copy using the RFDATA register
bool CC2538::copy_from_rxfifo(Buffer * buf)
{
    auto * data = buf->raw<char>();
    auto end = xreg(RXFIFOCNT); // Number of bytes currently in RXFIFO
    //volatile int * rxfifo = reinterpret_cast<volatile int*>(RXFIFO); // RXFIFO mapped in memory
    //const bool auto_crc_disabled = !(xreg(FRMCTRL0) & AUTO_CRC);
    bool ret = false;

    // Check CRC           // Last byte in the frame
    //if (auto_crc_disabled || ((rxfifo[end-1] & AUTO_CRC_OK)))
    //{
        data[0] = sfr(RFDATA); // First field is length of MAC

        if (data[0] > 127) {// Force size to at most 127
            data[0] = 127;
        }

        for(auto i = 1u; i < end; ++i) {// Copy rest of data
            data[i] = sfr(RFDATA);
        }

        ret = true;
    //}

    clear_rxfifo();
    return ret;
}

//bool CC2538::wait_for_ack(Reg32 filter_restore_value)
//{
//    bool acked = false;
//
//    if(!Traits<CC2538>::auto_listen) {
//        xreg(RFST) = ISRXON;
//    }
//
//    eMote3_GPTM timer(2, Traits<CC2538>::ACK_TIMEOUT); 
//    timer.enable();
//    while(timer.running() and not (acked = sfr(RFIRQF0) & INT_FIFOP));
//
//    //clear_rxfifo();
//    //xreg(FRMFILT1) = filter_restore_value; // Done with ACKs
//    //sfr(RFIRQF0) &= ~INT_FIFOP; // Clear FIFOP flag
//    //xreg(RFIRQM0) |= INT_FIFOP; // Enable FIFOP int
//
//    return acked;
//}

bool CC2538::send_and_wait(bool ack)
{    
    bool do_ack = Traits<CC2538>::ACK and ack;
    Reg32 saved_filter_settings;
    if(do_ack) {
        saved_filter_settings = xreg(FRMFILT1);
        xreg(RFIRQM0) &= ~INT_FIFOP; // Disable FIFOP int. We'll poll the interrupt flag
        xreg(FRMFILT1) = ACCEPT_FT2_ACK; // Accept only ACK frames now
    }
    
    bool sent = do_send();

    if(do_ack) {
        /*
        bool acked = sent and wait_for_ack(saved_filter_settings);

        for(auto i = 0u; (i < Traits<CC2538>::RETRANSMISSIONS) and not acked; i++) {
            db<CC2538>(TRC) << "CC2538::retransmitting" << endl;
            sent = do_send();

            acked = sent and wait_for_ack(saved_filter_settings);
        }

        clear_rxfifo();
        xreg(FRMFILT1) = saved_filter_settings; // Done with ACKs
        sfr(RFIRQF0) &= ~INT_FIFOP; // Clear FIFOP flag
        xreg(RFIRQM0) |= INT_FIFOP; // Enable FIFOP int
        return acked;
        */
    }
    else if (sent) {
        while(!(sfr(RFIRQF1) & INT_TXDONE));
        sfr(RFIRQF1) &= ~INT_TXDONE;
        _tx_pin->clear();
    }
    return sent;
}

bool CC2538::do_send()
{
    if(Traits<CC2538>::CSMA_CA) {
        unsigned int two_raised_to_be = 1;
        unsigned int BE;
        for(BE = 0; BE < Traits<CC2538>::CSMA_CA_MIN_BACKOFF_EXPONENT; BE++) {
            two_raised_to_be *= 2;
        }

        unsigned int trials;
        for(trials = 0u; trials < Traits<CC2538>::CSMA_CA_MAX_TRANSMISSION_TRIALS; trials++) {
            if(!Traits<CC2538>::auto_listen)
                xreg(RFST) = ISRXON;

            //unsigned int delay_time = 0;
            //while(delay_time < 192) {
            //    delay_time = Random::random() % Traits<CC2538>::CSMA_CA_DELAY_MAX;
            //}
            //auto delay_time = (1 + (Random::random() % (Traits<CC2538>::CSMA_CA_DELAY_MAX / 192))) * 192;

            auto delay_time = (Random::random() % (two_raised_to_be - 1)) * Traits<CC2538>::CSMA_CA_UNIT_BACKOFF_PERIOD;

            eMote3_GPTM::delay(delay_time, 2);
            sfr(RFST) = ISTXONCCA;
            if(xreg(FSMSTAT1) & SAMPLED_CCA) {
                clear_rxfifo();
                break; // Success
            }
            
            if(BE < Traits<CC2538>::CSMA_CA_MAX_BACKOFF_EXPONENT) {
                BE++;
                two_raised_to_be *= 2;
            }
        }
        if(trials >= Traits<CC2538>::CSMA_CA_MAX_TRANSMISSION_TRIALS) {
            db<CC2538>(WRN) << "CC2538::do_send() FAILED" << endl;
            return false;
        }
    }
    else {
        _tx_pin->set();
        sfr(RFST) = ISTXON;
    }

    return true;

    //if(Traits<CC2538>::ACK) {
    //    xreg(RFIRQM0) &= ~INT_FIFOP; // Disable FIFOP int. We'll poll the interrupt flag
    //    xreg(FRMFILT1) = ACCEPT_FT2_ACK; // Accept only ACK frames now
    //}
    //db<CC2538>(TRC) << "bytes in RXFIFO after CCA = " << xreg(RXFIFOCNT) << endl;

    //sfr(RFST) = ISCLEAR; // Clear program memory
    //sfr(RFST) = STXON;
    //sfr(RFST) = ISSTART; // Start TX

    //while(!(sfr(RFIRQF1) & INT_TXDONE));
    //sfr(RFIRQF1) &= ~INT_TXDONE;
    //db<CC2538>(TRC) << "bytes in RXFIFO after TX = " << xreg(RXFIFOCNT) << endl;

//    if(Traits<CC2538>::auto_listen)
//        xreg(RFST) = ISRXON;
    //db<CC2538>(TRC) << "bytes in RXFIFO after RX = " << xreg(RXFIFOCNT) << endl;
}

void CC2538::handle_int()
{
    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);

    if((irqrf0 & INT_FIFOP)) { // Frame received
        // Note that ISRs in EPOS are reentrant, that's why locking was carefully made atomic
        // Therefore, several instances of this code can compete to handle received buffers
        sfr(RFIRQF0) &= ~INT_FIFOP;

        if(!fifop_handler) {
            clear_rxfifo(); // No one wants this frame. Just drop it.        
            _statistics.dropped_rx_packets++;
        } else {
            if(!rxfifo_crc_check()) {
                clear_rxfifo(); // Bad CRC. Drop frame.
                _statistics.dropped_rx_packets++;
            } else {
                Buffer * buf = 0;

                // NIC received a frame in the RXFIFO, so we need to find an unused buffer for it
                for (auto i = 0u; (i < RX_BUFS) and not buf; ++i) {
                    if (_rx_buffer[_rx_cur]->lock()) {
                        db<CC2538>(INF) << "CC2538::handle_int: found buffer: " << _rx_cur << endl;
                        buf = _rx_buffer[_rx_cur]; // Found a good one
                    } else {
                        ++_rx_cur %= RX_BUFS;
                    }
                }

                if (not buf) {
                    db<CC2538>(WRN) << "CC2538::handle_int: no buffers left" << endl;
                    db<CC2538>(WRN) << "CC2538::handle_int: dropping fifo contents" << endl;
                    clear_rxfifo();
                    _statistics.dropped_rx_packets++;
                } else {
                    // We have a buffer, so we fetch a packet from the fifo
                    if (copy_from_rxfifo(buf)) {
                        buf->size(buf->frame()->header()->frame_length());// - (sizeof(Phy_Header))); // Phy_Header is included in Header, but is already discounted in frame_length

                        auto * frame = buf->frame();

                        db<CC2538>(TRC) << "CC2538::int:receive(d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

                        db<CC2538>(INF) << "CC2538::handle_int[" << _rx_cur << "]" << endl;

                        _statistics.rx_packets++;
                        _statistics.rx_bytes += buf->size();
                        
                        //IC::disable(_irq);
                        (*fifop_handler)(buf); // We've already checked that there is a handler there
                        // TODO: this serialization is much too restrictive. It was done this way for students to play with
                        //IC::enable(_irq);
                    } else { // something went wrong when checking the received data
                        free(buf);
                        _statistics.dropped_rx_packets++;
                    }
                }
            }
        }
    }

    db<CC2538>(TRC) << "CC2538::int: " << endl << "RFIRQF0 = " << irqrf0 << endl;
    if(irqrf0 & INT_RXMASKZERO) db<CC2538>(TRC) << "RXMASKZERO" << endl;
    if(irqrf0 & INT_RXPKTDONE) db<CC2538>(TRC) << "RXPKTDONE" << endl;
    if(irqrf0 & INT_FRAME_ACCEPTED) db<CC2538>(TRC) << "FRAME_ACCEPTED" << endl;
    if(irqrf0 & INT_SRC_MATCH_FOUND) db<CC2538>(TRC) << "SRC_MATCH_FOUND" << endl;
    if(irqrf0 & INT_SRC_MATCH_DONE) db<CC2538>(TRC) << "SRC_MATCH_DONE" << endl;
    if(irqrf0 & INT_SFD) db<CC2538>(TRC) << "SFD" << endl;
    if(irqrf0 & INT_ACT_UNUSED) db<CC2538>(TRC) << "ACT_UNUSED" << endl;

    db<CC2538>(TRC) << "RFIRQF1 = " << irqrf1 << endl;
    if(irqrf1 & INT_CSP_WAIT) db<CC2538>(TRC) << "CSP_WAIT" << endl;
    if(irqrf1 & INT_CSP_STOP) db<CC2538>(TRC) << "CSP_STOP" << endl;
    if(irqrf1 & INT_CSP_MANINT) db<CC2538>(TRC) << "CSP_MANINT" << endl;
    if(irqrf1 & INT_RFIDLE) db<CC2538>(TRC) << "RFIDLE" << endl;
    if(irqrf1 & INT_TXDONE) db<CC2538>(TRC) << "TXDONE" << endl;
    if(irqrf1 & INT_TXACKDONE) db<CC2538>(TRC) << "TXACKDONE" << endl;


    if(false) { // Error
        db<CC2538>(WRN) << "CC2538::int:error =>";
        db<CC2538>(WRN) << endl;
    }
}
//
//
void CC2538::int_handler(const IC::Interrupt_Id & interrupt)
{
    CC2538 * dev = get_by_interrupt(interrupt);

    db<CC2538>(TRC) << "Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<CC2538>(WRN) << "Radio::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

__END_SYS

#endif
