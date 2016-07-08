#ifndef __serializer_h
#define __serializer_h

#include <system/config.h>

// This file is included by both g++ in EPOS compilation and Vivado HLS in UD
// components synthesis. We use only C++ standard integer types in order to keep
// it portable. Remember that for Vivado HLS, char has 8 bits, short has 16 bits
// and int has 32 bits. in g++, the standard states that they have AT LEAST this
// width except for int that has at least 16 bits.

__BEGIN_SYS

class Serializer_Common
{
public:
    typedef unsigned long Buffer;

    // FIXME: NPKT* can be simplified using variadic templates but Catapult
    // doesn't support it
    template<typename T0>
    struct NPKT1 {
        enum { Result = DIV_ROUNDUP<sizeof(T0), sizeof(Buffer)>::Result };
    };

    template<typename T0, typename T1>
    struct NPKT2 {
        enum { Result = NPKT1<T0>::Result + NPKT1<T1>::Result };
    };

    template<typename T0, typename T1, typename T2>
    struct NPKT3 {
        enum { Result = NPKT1<T0>::Result + NPKT1<T1>::Result +
            NPKT1<T2>::Result };
    };

    template<typename T0, typename T1, typename T2, typename T3>
    struct NPKT4 {
        enum {
            Result = NPKT1<T0>::Result + NPKT1<T1>::Result +
                NPKT1<T2>::Result + NPKT1<T3>::Result
        };
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7>
    struct npkt8 {
        enum {
            Result = NPKT1<T0>::Result + NPKT1<T1>::Result + NPKT1<T2>::Result +
                NPKT1<T3>::Result + NPKT1<T4>::Result + NPKT1<T5>::Result +
                NPKT1<T6>::Result + NPKT1<T7>::Result
        };
    };

protected:
    template<typename B, typename A>
    static void pack(B * b, A & a);

    template<typename B, typename A>
    static void unpack(B * b, A & a);
};

// TODO: Put the specializations in a .cc file
// unsigned long b
template<>
void Serializer_Common::pack(unsigned long * b, const int & a) { *b = a; }

template<>
void Serializer_Common::unpack(unsigned long * b, int & a) { a = *b; }

template<>
void Serializer_Common::pack(unsigned long * b, const unsigned int & a) { *b = a; }

template<>
void Serializer_Common::unpack(unsigned long * b, unsigned int & a) { a = *b; }

__END_SYS

#endif
