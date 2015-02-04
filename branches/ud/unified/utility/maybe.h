/**
* Laboratory for Software/Hardware Integration - Federal University of Santa Catarina
* Engineer: João Paulo Pizani Flor, B.Sc - joaopizani@lisha.ufsc.br
* 
* Create Date: 10:00 28/05/2011
* Design Name: maybe.h
* Description: The Maybe Monad (http://en.wikipedia.org/wiki/Option_Type)
**/
#ifndef __MAYBE_UNIFIED_H__
#define __MAYBE_UNIFIED_H__

namespace Implementation {

template<typename T>
class Maybe {
public:
    Maybe(): _thing(T()), _exists(false) {}

    explicit Maybe(T thing, bool exists = true): _thing(thing), _exists(exists) {}

    bool exists() const { return _exists; }

    T get(T backup = T()) const { return _exists ? _thing : backup; }

    bool operator==(const Maybe<T>& other) const {
        return (!_exists && !other._exists) || (_exists && other._exists && _thing == other._thing);
    }

    bool operator!=(const Maybe<T>& other) const { return !(*this == other); }

private:
    T _thing;
    bool _exists;
};

}

#endif /* __MAYBE_H__ */
