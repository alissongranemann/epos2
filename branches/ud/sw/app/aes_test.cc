#include <components/aes.h>
#include <utility/ostream.h>
#include <system/types.h>
#include <chronometer.h>
#include <master_chronometer.h>
#include <alarm.h>

__USING_SYS

OStream cout;

void call_aes(AES &aes) {
    Chronometer chrono;
    Implementation::safe_pkt_t pkt;
    unsigned int result;

    cout << "Calling aes.cipher(pkt)" << endl;

    chrono.reset();
    chrono.start();
    result = aes.dummy_cipher(pkt);
    chrono.stop();

    cout << "Result = " << result << " (in " << chrono.ticks() << " ticks)" << endl;
    cout << "ticks_calc_feasability " << aes.ticks_calc_feasability << endl;
    cout << "ticks_acquire          " << aes.ticks_acquire << endl;
    cout << "ticks_change_domain_hw " << aes.ticks_change_domain_hw << endl;
    cout << "ticks_recfg            " << aes.ticks_recfg << endl;
    cout << "ticks_method           " << aes.ticks_method << endl;
    cout << "ticks_get_state        " << aes.ticks_get_state << endl;
    cout << "ticks_change_domain_sw " << aes.ticks_change_domain_sw << endl;
    cout << "ticks_set_state        " << aes.ticks_set_state << endl;
    cout << "ticks_free_hw_res      " << aes.ticks_free_hw_res << endl;
    cout << "ticks_release          " << aes.ticks_release << endl;
}

int main()
{
    volatile unsigned int * gpio = (volatile unsigned int *)(Traits<Machine>::LEDS_ADDRESS);

    *gpio = (1<<31) | (1<<7);

    AES aes(Component_Manager::dummy_channel, Component_Manager::dummy_channel, 0);

    cout << "AES Test" << endl;

    call_aes(aes);

    cout << "The End" << endl;

    return 0;
}
