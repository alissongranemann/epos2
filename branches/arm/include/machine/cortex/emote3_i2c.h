// EPOS eMote3 I2C Mediator Declarations

#ifndef __emote3_i2c_h
#define __emote3_i2c_h

#include <i2c.h>

__BEGIN_SYS

class CC2538_I2C : public Model {
    friend class eMote3;
public:

    volatile Reg32 & i2c(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(_base)[o / sizeof(Reg32)]; }

public:

    //master = 0 (master), master = 1 (slave)
    CC2538_I2C(Reg32 master = 0)
    {
        if(master == 0) {
            _base = reinterpret_cast<Log_Addr *>(I2C_MASTER_BASE);
        } else
            _base = reinterpret_cast<Log_Addr *>(I2C_SLAVE_BASE);
    }

    CC2538_I2C(Reg32 master, char port_sda, unsigned int pin_sda, char port_scl, unsigned int pin_scl)
    {
        if(master == 0) {
            _base = reinterpret_cast<Log_Addr *>(I2C_MASTER_BASE);
        } else
            _base = reinterpret_cast<Log_Addr *>(I2C_SLAVE_BASE);

        config(port_sda, pin_sda, port_scl, pin_scl);
    }

    void config(char port_sda = 'b', unsigned int pin_sda = 1, char port_scl = 'b', unsigned int pin_scl = 0) {
        //disable();
        Model::config_I2C(_base, port_sda, pin_sda, port_scl, pin_scl);
        if(_base == reinterpret_cast<Log_Addr *>(I2C_MASTER_BASE))
            config_master();
        else
            config_slave();
        //enable();
    }

    void enable() {
    }

    void disable() {
    }

    bool is_busy() {
        return (i2c(I2CM_STAT) & BUSY);
    }

    //returns true if an error has occurred, false otherwise
    bool put(unsigned char adr_rw, char data[], unsigned int size) {
        bool ret = false;
        for(unsigned int i = 0; i < size; i++) {
            if(i == 0) //first byte to be sent
                ret = send_byte(adr_rw, data[i], 0);
            else if(i + 1 == size) //last byte to be sent
                ret = send_byte(adr_rw, data[i], 2);
            else
                ret = send_byte(adr_rw, data[i], 1);

            if(ret)
                return true; //an error has occurred
        }
        return ret;
    }

    //returns true if an error has occurred, false otherwise.
    bool put(unsigned char adr_rw, char data) {
        // Specify the slave address and the next operation (last bit = 0)
        i2c(I2CM_SA) = (adr_rw << 1) & 0xFE;
        // Place the data to be transmitted
        i2c(I2CM_DR) = data;
        // Initiate a single-byte transmit of the data
        i2c(I2CM_CTRL) = START | RUN | STOP; //0x7 -  START = 1, RUN = 1, STOP = 1
        // Wait until the transmission completes by polling the BUSY bit of the I2CM_STAT register
        while(is_busy()) ;
        // Check the ERROR bit in the I2CM_STAT register
        return (i2c(I2CM_STAT) & ERROR);  // 0 = no error detected
                                          // 1 = an error occurred
    }

    bool get(char adr_rw, char *data) {
        // Specify the slave address and the next operation (last bit = 1)
        i2c(I2CM_SA) = (adr_rw << 1) | 0x01;
        // Initiate a single-byte receive of the data
        i2c(I2CM_CTRL) = START | RUN | STOP; //0x07 - START = 1, RUN = 1 STOP = 1
        while(is_busy()) ;
        if(i2c(I2CM_STAT) & ERROR)
            return true;
        // Read and return the data
        *data = i2c(I2CM_DR);
        return false;
    }

    bool get(char adr_rw, char *data, unsigned int size) {
        unsigned int i;
        bool ret = false;
        for(i = 0; i < size; i++) {
            if(i == 0) {
                i2c(I2CM_SA) = (adr_rw << 1) | 0x01;
                i2c(I2CM_CTRL) = START | RUN | ACK;
            } else if(i + 1 == size) {
                i2c(I2CM_CTRL) = STOP | RUN;
            } else {
                i2c(I2CM_CTRL) = RUN | ACK;
            }

            while(is_busy()) ;
            if(i2c(I2CM_STAT) & ERROR)
                return true;
            data[i] = i2c(I2CM_DR);
        }
        return ret;
    }

private:

    //mode = 0 - START | RUN
    //mode = 1 - RUN
    //mode = 2 - RUN | STOP
    bool send_byte(unsigned char adr_rw, char data, int mode) {
        // Specify the slave address and the next operation (last bit = 0)
        if(mode == 0)
            i2c(I2CM_SA) = (adr_rw << 1) & 0xFE;
        // Place the data to be transmitted
        i2c(I2CM_DR) = data;
        if(mode == 0)
            i2c(I2CM_CTRL) = START | RUN; //0x7 -  START = 1, RUN = 1, STOP = 1
        else if(mode == 1)
            i2c(I2CM_CTRL) = RUN;
        else
            i2c(I2CM_CTRL) = RUN | STOP;

        // Wait until the transmission completes by polling the BUSY bit of the I2CM_STAT register
        while(is_busy()) ;
        // Check the ERROR bit in the I2CM_STAT register
        return (i2c(I2CM_STAT) & ERROR);  // 0 = no error detected
                                          // 1 = an error occurred
    }

    //configure I2C to work as master
    void config_master() {
        i2c(I2CM_CR) = MFE; //0x10;
        i2c(I2CM_TPR) = 0x3; // For a system clock = 32MHz, 400.000
    }

    //configure I2C to work as slave
    void config_slave() {
        i2c(I2CM_CR) = SFE; //0x20;
    }

private:
    volatile Log_Addr * _base;
};

__END_SYS

#endif
