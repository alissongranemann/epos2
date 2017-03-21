// EPOS ARM Cortex Smart Transducer Implementation

#include <system/config.h>
#ifdef __TRANSDUCER_H

#include <machine/cortex/transducer.h>

__BEGIN_SYS

// Attributes
Door_Sensor::Observed Door_Sensor::_observed;
Door_Sensor * Door_Sensor::_dev[Door_Sensor::MAX_DEVICES];

// Methods

__END_SYS

#endif
