#ifndef __assert_aux_h
#define __assert_aux_h

#include <utility/ostream.h>

__BEGIN_UTIL

template<typename T>
void assert_equals(T alpha, T beta)
{
    if (alpha != beta) {
        db<void>(ERR) << "Assertion fail. "
                        << alpha << " (" << reinterpret_cast<void *>(alpha) << ")"
                        << " is not equal to "
                        << beta << " (" << reinterpret_cast<void *>(beta) << ")" << endl;
    }
}

__END_UTIL

#endif
