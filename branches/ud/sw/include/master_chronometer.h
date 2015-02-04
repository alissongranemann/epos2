#ifndef __master_chronometer_h
#define __master_chronometer_h

#include <chronometer.h>

__BEGIN_SYS

// Singleton
class Master_Chronometer {
public:
    static Chronometer * instance() {
        if (!_instance)
            _instance = new Chronometer();

        return _instance;
    }

private:
    Master_Chronometer() { }

private:
    static Chronometer * _instance;
};

__END_SYS

#endif

