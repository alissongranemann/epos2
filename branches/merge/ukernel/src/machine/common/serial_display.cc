// EPOS Serial Display Mediator Implementation

#include <display.h>

__BEGIN_SYS

// Class attributes
bool Serial_Display::_engine_started = false;
Serial_Display::Engine Serial_Display::_engine;
int Serial_Display::_line;
int Serial_Display::_column;

__END_SYS
