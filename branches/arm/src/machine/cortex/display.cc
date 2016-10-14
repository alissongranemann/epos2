// EPOS Cortex-M3 Serial Display Mediator Implementation

#include <display.h>

__BEGIN_SYS

// Class attributes
Cortex_M_USB_Serial_Display::Engine Cortex_M_USB_Serial_Display::_engine;
int Cortex_M_USB_Serial_Display::_line;
int Cortex_M_USB_Serial_Display::_column;

__END_SYS

