#include <machine/cortex_m/emote3.h>

using namespace EPOS;

int main()
{    
    auto frequency_hertz = 10000;
    auto duty_cycle_percent = 50;

    eMote3_PWM pwm(1, frequency_hertz, duty_cycle_percent);
    pwm.enable();

    while(1);

    return 0;
}
