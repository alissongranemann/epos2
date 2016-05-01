// EPOS Keyboard Mediator Common Package

#ifndef __keyboard_h
#define __keyboard_h

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

private:
    static void init() {}
};

__END_SYS

#ifdef __KEYBOARD_H
#include __KEYBOARD_H
#endif

#endif
