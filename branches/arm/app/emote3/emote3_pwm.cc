#include <gpio.h>
#include <machine/cortex_m/emote3_pwm.h>
#include <machine/cortex_m/emote3_gptm.h>

using namespace EPOS;

const unsigned int PWM_FREQUENCY = 10000; // 10 KHz

OStream cout;
GPIO * led, * coil;
eMote3_PWM * pwm;

int main()
{
    int dc = 0;
    cout << "PWM test" << endl;
    cout << "Pins" << endl
         << "    LED: PC3" << endl
         << "    Coil: PD3" << endl
         << "    PWM: PD2" << endl;

    led = new GPIO('c',3, GPIO::OUTPUT);
    coil = new GPIO('d',3, GPIO::OUTPUT);
    pwm = new eMote3_PWM(1, PWM_FREQUENCY, 50, 'd', 2);

    led->set(true);
    coil->set(false);

    eMote3_GPTM::delay(2000000);
    coil->set(true);

    while(true) {
        pwm->set(PWM_FREQUENCY, dc);
        dc += 10;
        led->set(true);
        eMote3_GPTM::delay(500000);
        led->set(false);
        eMote3_GPTM::delay(500000);

        if(dc > 99)
            dc = 0;
    }

    return 0;
}
