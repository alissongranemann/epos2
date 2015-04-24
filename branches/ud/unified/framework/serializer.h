#ifndef __serializer_h
#define __serializer_h

#include <system/config.h>

// This file is included by both g++ in EPOS compilation and Catapult in UD
// components synthesis. We use only C++ standard integer types in order to keep
// it portable. Remember that for Catapult, char has 8 bits, int has 16 bits and
// long has 32 bits; in g++, the standard states that they have AT LEAST this
// width.

__BEGIN_SYS

class Serializer_Common
{
public:
    typedef unsigned long Packet;

    // FIXME: npkt* can be simplified using variadic templates but Catapult
    // doesn't support it
    template<typename T0>
    struct npkt1 {
        enum { Result = DIV_ROUNDUP<sizeof(T0), sizeof(Packet)>::Result };
    };

    template<typename T0, typename T1>
    struct npkt2 {
        enum { Result = npkt1<T0>::Result + npkt1<T1>::Result };
    };

    template<typename T0, typename T1, typename T2>
    struct npkt3 {
        enum { Result = npkt1<T0>::Result + npkt1<T1>::Result +
            npkt1<T2>::Result };
    };

    template<typename T0, typename T1, typename T2, typename T3>
    struct npkt4 {
        enum {
            Result = npkt1<T0>::Result + npkt1<T1>::Result +
                npkt1<T2>::Result + npkt1<T3>::Result
        };
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7>
    struct npkt8 {
        enum {
            Result = npkt1<T0>::Result + npkt1<T1>::Result + npkt1<T2>::Result +
                npkt1<T3>::Result + npkt1<T4>::Result + npkt1<T5>::Result +
                npkt1<T6>::Result + npkt1<T7>::Result
        };
    };

protected:
    template<typename P, typename A>
    static void pack(P * p, A & a);

    template<typename P, typename A>
    static void unpack(P * p, A & a);
};

// TODO: Put the specializations in a .cc file
// unsigned long p
template<>
void Serializer_Common::pack(unsigned long * p, const int & a) { *p = a; }

template<>
void Serializer_Common::unpack(unsigned long * p, int & a) { a = *p; }

template<>
void Serializer_Common::pack(unsigned long * p, const unsigned int & a) { *p = a; }

template<>
void Serializer_Common::unpack(unsigned long * p, unsigned int & a) { a = *p; }

__END_SYS

#endif
