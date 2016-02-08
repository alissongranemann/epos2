// EPOS Network Interface Mediator Common Package

#ifndef __nic_h
#define __nic_h

#include <utility/string.h>
#include <utility/random.h>
#include <cpu.h>

__BEGIN_SYS

class NIC_Common
{
protected:
    NIC_Common() {}

public:
    typedef int Observed;

    // NIC physical address (e.g. MAC)
    template<unsigned int LENGTH>
    class Address
    {
    public:
        enum Null { NULL = 0 };
        enum Broadcast { BROADCAST = 255 };
        enum Random_Address { RANDOM = 42 };

    public:
        Address() {}

        Address(const Null &) {
            for(unsigned int i = 0; i < LENGTH; i++)
                _address[i] =  NULL;
        }

        Address(const Broadcast &) {
            for(unsigned int i = 0; i < LENGTH; i++)
                _address[i] = BROADCAST;
        }

        Address(const Random_Address &) {
            for(unsigned int i = 0; i < LENGTH; i++)
                _address[i] = Random::random();
        }

        Address(const char * str) { // String formated as A.B.C.D or A:B:C:D:E:F
            static const char sep = (LENGTH == 4) ? '.' : ':';
            char * token = strchr(str, sep);
            for(unsigned int i = 0; i < LENGTH; i++, ++token, str = token, token = strchr(str, sep))
                _address[i] = atol(str);
        }

        Address(unsigned long a) {
            assert(LENGTH == sizeof(long));
            a = htonl(a);
            memcpy(this, &a, sizeof(long));
        }

        Address operator=(const Address & a) {
            for(unsigned int i = 0; i < LENGTH; ++i)
                _address[i] = a._address[i];
            return a;
        }
        Address operator=(const Address & a) volatile {
            for(unsigned int i = 0; i < LENGTH; ++i)
                _address[i] = a._address[i];
            return a;
        }

        operator bool() const {
            for(unsigned int i = 0; i < LENGTH; ++i) {
                if(_address[i])
                    return true;
            }
            return false;
        }
        operator bool() const volatile {
            for(unsigned int i = 0; i < LENGTH; ++i) {
                if(_address[i])
                    return true;
            }
            return false;
        }

        bool operator==(const Address & a) const {
            for(unsigned int i = 0; i < LENGTH; ++i) {
                if(_address[i] != a._address[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const Address & a) const {
            for(unsigned int i = 0; i < LENGTH; ++i) {
                if(_address[i] != a._address[i])
                    return true;
            }
            return false;
        }

        Address operator&(const Address & a) const {
            Address ret;
            for(unsigned int i = 0; i < LENGTH; ++i)
                ret[i] = _address[i] & a._address[i];
            return ret;
        }

        Address operator|(const Address & a) const {
            Address ret;
            for(unsigned int i = 0; i < LENGTH; ++i)
                ret[i] = _address[i] | a._address[i];
            return ret;
        }

        Address operator~() const {
            Address ret;
            for(unsigned int i = 0; i < LENGTH; ++i)
                ret[i] = ~_address[i];
            return ret;
        }

        unsigned int operator%(unsigned int i) const {
            return _address[LENGTH - 1] % i;
        }

        unsigned char & operator[](const size_t i) { return _address[i]; }
        const unsigned char & operator[](const size_t i) const { return _address[i]; }

        friend OStream & operator<<(OStream & db, const Address & a) {
            db << hex;
            for(unsigned int i = 0; i < LENGTH; i++) {
                db << a._address[i];
                if(i < LENGTH - 1)
                    db << ((LENGTH == 4) ? "." : ":");
            }
            db << dec;
            return db;
        }

    private:
        unsigned char _address[LENGTH];
    } __attribute__((packed));

    // NIC protocol id
    typedef unsigned short Protocol;

    // NIC CRCs
    typedef unsigned short CRC16;
    typedef unsigned long CRC32;

    // NIC statistics
    struct Statistics
    {
        Statistics(): rx_packets(0), tx_packets(0), rx_bytes(0), tx_bytes(0) { }

        unsigned int rx_packets;
        unsigned int tx_packets;
        unsigned int rx_bytes;
        unsigned int tx_bytes;
    };

    // Polymorphic (or not) NIC wrapper
    template<typename NIC>
    class NIC_Base
    {
    private:
        typedef typename NIC::Address Address;
        typedef typename NIC::Buffer Buffer;

        typedef void (Frame_Handler)(Buffer *);

    public:
        NIC_Base(unsigned int unit = 0) {}

        virtual ~NIC_Base() {}
    
        virtual int send(Buffer * buf) = 0; 
        virtual void receive(const Frame_Handler & handler) = 0;
        virtual void off() = 0;
    
        virtual Buffer * alloc(NIC * nic, unsigned int payload) = 0; 
        virtual void free(Buffer * buf) = 0;

        virtual const Address & address() = 0;
        virtual void address(const Address &) = 0;
        virtual bool channel_busy() = 0;
    };

    template<typename NIC, bool polymorphic>
    class NIC_Wrapper: public NIC_Base<NIC>, private NIC
    {
    private:
        typedef typename NIC::Address Address;
        typedef typename NIC::Buffer Buffer;

        typedef void (Frame_Handler)(Buffer *);

    public:
        NIC_Wrapper(unsigned int unit = 0): NIC(unit) {}

        virtual ~NIC_Wrapper() {}

        virtual int send(Buffer * buf) { 
            return NIC::send(buf);
        }
        virtual void receive(const Frame_Handler & handler) {
            NIC::receive(handler);
        }
        virtual void off() { 
            NIC::off();
        }
        virtual Buffer * alloc(NIC * nic, unsigned int payload) {
           return NIC::alloc(nic, payload);
        }
        virtual void free(Buffer * buf) { 
            NIC::free(buf); 
        }
        virtual const Address & address() {
            return NIC::address();
        }
        virtual void address(const Address & addr) {
            NIC::address(addr);
        }
        virtual bool channel_busy() {
            return NIC::channel_busy();
        }
    };

    template<typename NIC>
    class NIC_Wrapper<NIC, false>: public NIC
    {
    public:
        NIC_Wrapper(unsigned int unit = 0): NIC(unit) {}
    };

    // Meta_NIC (efficiently handles polymorphic or monomorphic lists of NICs
    template<typename NICS>
    class Meta_NIC
    {
    private:
        static const bool polymorphic = NICS::Polymorphic;

        typedef typename NICS::template Get<0>::Result T;

    public:
        typedef typename IF<polymorphic, NIC_Base<T>, T>::Result Base;

        template<int Index>
        struct Get { typedef NIC_Wrapper<typename NICS::template Get<Index>::Result, polymorphic> Result; };
    };
};

__END_SYS

#ifdef __NIC_H
#include __NIC_H
#endif

#endif
