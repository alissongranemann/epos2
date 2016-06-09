#include <utility/bignum.h>

__BEGIN_UTIL

template<> const unsigned char Bignum<16>::default_mod[Bignum<16>::sz_word] = {
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFD, 0xFF, 0xFF, 0xFF
};
template<> const unsigned char Bignum<16>::default_barrett_u[Bignum<16>::sz_word + Bignum<16>::sz_digit] = {
    17, 0, 0, 0, 
    8, 0, 0, 0, 
    4, 0, 0, 0, 
    2, 0, 0, 0, 
    1, 0, 0, 0
};

__END_UTIL
