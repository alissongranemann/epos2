// EPOS EPOSSOC_RTC Declarations

#ifndef __epossoc_rtc_h
#define __epossoc_rtc_h

#include <rtc.h>

__BEGIN_SYS

class EPOSSOC_RTC: public RTC_Common
{
private:
   static const unsigned int EPOCH_DAYS = 0;

public:
    EPOSSOC_RTC() { }

    static Date date();
    static void date(const Date & d);

    static Second seconds_since_epoch() { 
        return date().to_offset(EPOCH_DAYS); 
    }
};

__END_SYS

#endif
