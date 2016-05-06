// EPOS Keyboard Mediator Common Package

#ifndef __keyboard_h
#define __keyboard_h

#include <utility/observer.h>
#include <display.h>

__BEGIN_SYS

class Keyboard_Common
{
protected:
    Keyboard_Common() {}
};

class Serial_Keyboard: public Keyboard_Common
{
    friend class PC_Setup;
    friend class First_Object;

public:
    Serial_Keyboard() {}

    static char getc() {
        return Serial_Display::_uart.get();
    }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static void init() {}

private:
    static Observed _observed;
};

__END_SYS

#ifdef __KEYBOARD_H
#include __KEYBOARD_H
#endif

#endif
