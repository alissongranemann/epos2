// EPOS Cortex-M USB Mediator Declarations

#ifndef __cortex_m_usb_h
#define __cortex_m_usb_h

#include __MODEL_H

__BEGIN_SYS

class Cortex_M_USB : private USB_Common, public Cortex_M_Model_USB
{
};

__END_SYS

#endif
