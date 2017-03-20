// EPOS ARM Cortex RFID Reader Mediator Declarations

#include <system/config.h>

#ifndef __cortex_rfid_reader_h
#define __cortex_rfid_reader_h

#include <rfid_reader.h>
#include <machine.h>
#include <mifare.h>
#include <spi.h>
#include <gpio.h>

__BEGIN_SYS

// MFRC522 MIFARE Reader chip from NXP
class MFRC522: private Machine_Model
{
    static const unsigned int BLOCK_SIZE = 16;

public:
    enum PCD_Status_Code {
        OK             = 0, // Success
        ERROR          = 1, // Error in communication
        COLLISION      = 2, // Collission detected
        NO_ROOM        = 3, // A buffer is not big enough.
        INVALID        = 4, // Invalid argument.
        CRC_WRONG      = 5, // The CRC_A does not match
        MIFARE_NACK    = 6, // A MIFARE PICC responded with NAK.
        TIMEOUT        = 7, // Request time exceeded
        INTERNAL_ERROR = 8, // Error in the module code
    };

    // MFRC522 reader commands
    enum PCD_Command {
        IDLE               = 0x00,  // no action, cancels current command execution
        MEM                = 0x01,  // stores 25 bytes into the internal buffer
        GENERATE_RANDOM_ID = 0x02,  // generates a 10-byte random ID number
        CALC_CRC           = 0x03,  // activates the CRC coprocessor or performs a self-test
        TRANSMIT           = 0x04,  // transmits data from the FIFO buffer
        NO_CMD_CHANGE      = 0x07,  // no command change, can be used to modify the CommandRegbits without affecting the command
        RECEIVE            = 0x08,  // activates the receiver circuits
        TRANSCEIVE         = 0x0C,  // transmits data from FIFO buffer to antenna and activates the receiver after transmission
        MF_AUTHENT         = 0x0E,  // performs the MIFARE standard authentication as a reader
        SOFT_RESET         = 0x0F,  // resets the MFRC522
    };

    // MFRC522 reader registers addresses
    enum PCD_Register {
        // Command and status
        COMMAND           = 0x01 << 1,  // starts and stops command execution
        COM_I_EN          = 0x02 << 1,  // enable and disable interrupt request control bits
        DIV_I_EN          = 0x03 << 1,  // enable and disable interrupt request control bits
        COM_IRQ           = 0x04 << 1,  // interrupt request bits
        DIV_IRQ           = 0x05 << 1,  // interrupt request bits
        ERROR_REG         = 0x06 << 1,  // error bits showing the error status of the last command executed
        STATUS_1          = 0x07 << 1,  // communication status bits
        STATUS_2          = 0x08 << 1,  // receiver and transmitter status bits
        FIFO_DATA         = 0x09 << 1,  // input and output of 64 byte FIFO buffer
        FIFO_LEVEL        = 0x0A << 1,  // number of bytes stored in the FIFO buffer
        WATER_LEVEL       = 0x0B << 1,  // level for FIFO underflow and overflow warning
        CONTROL           = 0x0C << 1,  // miscellaneous control registers
        BIT_FRAMING       = 0x0D << 1,  // adjustments for bit-oriented frames
        COLL              = 0x0E << 1,  // bit position of the first bit-collision detected on the RF interface
        // Command
        MODE              = 0x11 << 1,  // defines general modes for transmitting and receiving
        TX_MODE           = 0x12 << 1,  // defines transmission data rate and framing
        RX_MODE           = 0x13 << 1,  // defines reception data rate and framing
        TX_CONTROL        = 0x14 << 1,  // controls the logical behavior of the antenna driver pins TX1 and TX2
        TX_ASK            = 0x15 << 1,  // controls the setting of the transmission modulation
        TX_SEL            = 0x16 << 1,  // selects the internal sources for the antenna driver
        RX_SEL            = 0x17 << 1,  // selects internal receiver settings
        RX_THRESHOLD      = 0x18 << 1,  // selects thresholds for the bit decoder
        DEMOD             = 0x19 << 1,  // defines demodulator settings
        MF_TX             = 0x1C << 1,  // controls some MIFARE communication transmit parameters
        MF_RX             = 0x1D << 1,  // controls some MIFARE communication receive parameters
        SERIAL_SPEED      = 0x1F << 1,  // selects the speed of the serial UART interface
        // Configuration
        CRC_RESULT_H      = 0x21 << 1,  // shows the MSB and LSB values of the CRC calculation
        CRC_RESULT_L      = 0x22 << 1,  // shows the MSB and LSB values of the CRC calculation
        MOD_WIDTH         = 0x24 << 1,  // controls the ModWidth setting
        RF_CFG            = 0x26 << 1,  // configures the receiver gain
        GS_N              = 0x27 << 1,  // selects the conductance of the antenna driver pins TX1 and TX2 for modulation
        CW_GS_P           = 0x28 << 1,  // defines the conductance of the p-driver output during periods of no modulation
        MOD_GS_P          = 0x29 << 1,  // defines the conductance of the p-driver output during periods of modulation
        T_MODE            = 0x2A << 1,  // defines settings for the internal timer
        T_PRESCALER       = 0x2B << 1,  // defines settings for the internal timer
        T_RELOAD_H        = 0x2C << 1,  // defines the 16-bit timer reload value
        T_RELOAD_L        = 0x2D << 1,  // defines the 16-bit timer reload value
        T_COUNTER_VALUE_H = 0x2E << 1,  // shows the 16-bit timer value
        T_COUNTER_VALUE_L = 0x2F << 1,  // shows the 16-bit timer value
        // Test registers
        TEST_SEL_1        = 0x31 << 1,  // general test signal configuration
        TEST_SEL_2        = 0x32 << 1,  // general test signal configuration
        TEST_PIN_EN       = 0x33 << 1,  // enables pin output driver on pins D1 to D7
        TEST_PIN_VALUE    = 0x34 << 1,  // defines the values for D1 to D7 when it is used as an I/O bus
        TEST_BUS          = 0x35 << 1,  // shows the status of the internal test bus
        AUTO_TEST         = 0x36 << 1,  // controls the digital self-test
        VERSION           = 0x37 << 1,  // shows the software version
        ANALOG_TEST       = 0x38 << 1,  // controls the pins AUX1 and AUX2
        TEST_DAC_1        = 0x39 << 1,  // defines the test value for TestDAC1
        TEST_DAC_2        = 0x3A << 1,  // defines the test value for TestDAC2
        TEST_ADC          = 0x3B << 1,  // shows the value of ADC I and Q channels
    };

protected:
    typedef MIFARE::UID UID;
    typedef MIFARE::Key Key;

    typedef unsigned char Block[BLOCK_SIZE];

    MFRC522(SPI * spi, GPIO * select, GPIO * reset) : _spi(spi), _select(select), _reset(reset) {
        _spi->disable();
        _select->direction(GPIO::OUT);
        _select->set();
        _reset->direction(GPIO::INOUT);
        _reset->clear();
    }

    void initialize();
    void reset();
    bool card_present();
    bool read_card(UID * uid, unsigned int valid_bits = 0);
    bool select(UID & uid) { return read_card(&uid, uid.size() * 8); }
    void halt_card();
    unsigned int put(unsigned int block, const Block data);
    unsigned int read(unsigned int block, Block data);
    bool authenticate(unsigned int sector, const Key & key, const UID & UID);

    void deauthenticate() {
        // clear MFCrypto1On bit
        write_reg(PCD_Register::STATUS_2, read_reg(PCD_Register::STATUS_2) & (~0x08));
    }


public:
    static unsigned int sector(unsigned int block) {
        if(block < 128)
            return block / 4;
        return 32 + (block - 128) / 16;
    }
    static unsigned int key_block(unsigned int sector) {
        if(sector < 32)
            return sector * 4 + 3;
        return 128 + (sector - 32) * 16 + 15;
    }

private:
    PCD_Status_Code crc(const void * data, unsigned int length, CPU::Reg8 * result);

    PCD_Status_Code communicate_with_picc(
        CPU::Reg8 command, CPU::Reg8 wait_irq, CPU::Reg8 * send_data, CPU::Reg8 send_len, CPU::Reg8 * back_data = 0,
        CPU::Reg8 * back_len = 0, CPU::Reg8 * valid_bits = 0, CPU::Reg8 rx_align = 0, bool check_crc = false
    );

    PCD_Status_Code mifare_transceive(const CPU::Reg8* data, CPU::Reg8 length, bool accept_timeout = false);

    CPU::Reg8 read_reg(PCD_Register reg);
    void read_reg(PCD_Register reg, CPU::Reg8 count, CPU::Reg8 * values, CPU::Reg8 rx_align);
    void write_reg(PCD_Register reg, CPU::Reg8 value);
    void write_reg(PCD_Register reg, unsigned int count, const void * values);

    SPI * _spi;
    GPIO * _select, * _reset;
};

typedef MFRC522 RFID_Reader_Engine;


// MIFARE RFID Reader/Writer
class RFID_Reader: public RFID_Reader_Common, public MIFARE, private RFID_Reader_Engine
{
    typedef RFID_Reader_Engine Engine;

public:
    typedef Engine::UID UID;
    typedef Engine::Key Key;

public:
    RFID_Reader(SPI * spi, GPIO * select, GPIO * reset) : MFRC522(spi, select, reset) {
        Engine::initialize();
    }

    UID get() {
        UID u;
        while(!ready_to_get());
        Engine::read_card(&u);
        return u;
    }

    bool halt(UID & u) {
        if(ready_to_put() && select(u)) {
            Engine::halt_card();
            return true;
        }
        return false;
    }

    unsigned int put(UID & u, unsigned int block, const Block data, const Key * key = 0) {
        unsigned int ret = 0;
        if(select(u)) {
            unsigned int s = sector(block);
            if(key) {
                if(Engine::authenticate(s, *key, u))
                    ret = Engine::put(block, data);
                Engine::deauthenticate();
            } else
                ret = Engine::put(block, data);
        }
        return ret;
    }

    unsigned int read(UID & u, unsigned int block, Block data, const Key * key = 0) {
        unsigned int ret = 0;
        if(select(u)) {
            unsigned int s = sector(block);
            if(key) {
                if(Engine::authenticate(s, *key, u))
                    ret = Engine::put(block, data);
                Engine::deauthenticate();
            } else
                ret = Engine::read(block, data);
        }
        return ret;
    }

    void reset() { Engine::reset(); }

    bool ready_to_get() { return Engine::card_present(); }
    bool ready_to_put() { return Engine::card_present(); }

private:
    bool select(UID & u) {
        while(!ready_to_put());
        return Engine::select(u);
    }
};

__END_SYS

#endif
