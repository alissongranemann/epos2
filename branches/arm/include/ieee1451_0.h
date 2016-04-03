// EPOS IEEE 1451.0 Definitions Common Package

#include <nic.h>

#ifndef __ieee1451_0_h
#define __ieee1451_0_h

#include <cpu.h>
#include <utility/list.h>
#include <utility/observer.h>

__BEGIN_SYS

class IEEE1451_0
{
public:
    class Unit
    {
    public:
        enum RADIAN_T { RADIAN };
        enum STERADIAN_T { STERADIAN };
        enum METER_T { METER };
        enum KILOGRAM_T { KILOGRAM };
        enum SECOND_T { SECOND };
        enum AMPERE_T { AMPERE };
        enum KELVIN_T { KELVIN };
        enum MOLE_T { MOLE };
        enum CANDELA_T { CANDELA };

        enum ENU_T { 
            PRESENCE,
        };

        Unit() : enu(0), rad(0), raw(0) { };
        Unit(RADIAN_T x) : enu(0), rad(0), raw(0) { radian(); };
        Unit(STERADIAN_T x) : enu(0), rad(0), raw(0) { steradian(); };
        Unit(METER_T x) : enu(0), rad(0), raw(0) { meter(); };
        Unit(KILOGRAM_T x) : enu(0), rad(0), raw(0) { kilogram(); };
        Unit(SECOND_T x) : enu(0), rad(0), raw(0) { second(); };
        Unit(AMPERE_T x) : enu(0), rad(0), raw(0) { ampere(); };
        Unit(KELVIN_T x) : enu(0), rad(0), raw(0) { kelvin(); };
        Unit(MOLE_T x) : enu(0), rad(0), raw(0) { mole(); };
        Unit(CANDELA_T x) : enu(0), rad(0), raw(0) { candela(); };
        Unit(ENU_T x) : enu(x), rad(0), raw(0) { };

        void radian(float exponent = 1) { rad = exponent * 2 + 8; }
        void steradian(float exponent = 1) { sr = exponent * 2 + 8; }
        void meter(float exponent = 1) { m = exponent * 2 + 8; }
        void kilogram(float exponent = 1) { kg = exponent * 2 + 8; }
        void second(float exponent = 1) { kg = exponent * 2 + 8; }
        void ampere(float exponent = 1) { A = exponent * 2 + 8; }
        void kelvin(float exponent = 1) { K = exponent * 2 + 8; }
        void mole(float exponent = 1) { mol = exponent * 2 + 8; }
        void candela(float exponent = 1) { cd = exponent * 2 + 8; }
        
        operator int() const { return raw ^ (enu << 8) ^ rad; }

        friend Debug & operator<<(Debug & db, const Unit & u) {
            db << "IEEE1451_0::Unit{enu=" << u.enu << ",rad=" << u.rad << ",sr=" << u.sr << ",m=" << u.m << ",kg=" << u.kg << ",s=" << u.s << ",A=" << u.A << ",K=" << u.K << ",mol=" << u.mol << ",cd=" << u.cd  << "}";
            return db;
        }
        friend OStream & operator<<(OStream & db, const Unit & u) {
            db << "IEEE1451_0::Unit{enu=" << u.enu << ",rad=" << u.rad << ",sr=" << u.sr << ",m=" << u.m << ",kg=" << u.kg << ",s=" << u.s << ",A=" << u.A << ",K=" << u.K << ",mol=" << u.mol << ",cd=" << u.cd  << "}";
            return db;
        }

    private:
        unsigned enu : 4;
        unsigned rad : 4;
        union {
            int raw;
            struct {
                unsigned sr : 4;
                unsigned m : 4;
                unsigned kg : 4;
                unsigned s : 4;
                unsigned A : 4;
                unsigned K : 4;
                unsigned mol : 4;
                unsigned cd : 4;
            };
        };
    }__attribute__((packed));

    struct UNITS {
        typedef unsigned long long UNIT_CODE_T; 

        template<UNIT_CODE_T C>
        struct CODE { 
            typedef UNITS::UNIT_CODE_T UNIT_CODE_T;
            static const UNIT_CODE_T UNIT_CODE = C;
            static const unsigned int DATUM_SIZE = 4;
        };

        struct METER : public CODE<0xA000ull> { };
    };
};

__END_SYS

#endif
