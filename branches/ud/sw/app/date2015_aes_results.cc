// AES evaluation tests for DATE15 conference. It compares the number of ticks
// needed for encrypt 4M bytes of data to compare this infrastructure with
// SPREAD. It also counts the number of ticks for each operation in the
// Component_Manager::recfg() method.

#include <components/aes.h>
#include <utility/ostream.h>
#include <system/types.h>
#include <chronometer.h>
#include <master_chronometer.h>
#include <alarm.h>

__USING_SYS

OStream cout;

int main()
{
    Implementation::safe_pkt_t pkt;
    unsigned int result;
    Chronometer chrono;
    AES aes(Component_Manager::dummy_channel, Component_Manager::dummy_channel, 0);
    volatile unsigned int * gpio = (volatile unsigned int *)(Traits<Machine>::LEDS_ADDRESS);
    int i;

    *gpio = (1<<31) | (1<<7);

    cout << "AES evaluation tests for DATE15 conference" << endl;

    //aes.add_key_2(pkt);

    chrono.reset();
    chrono.start();
    result = aes.dummy_cipher(pkt);
    chrono.stop();

    cout << "Number of ticks for each operation" << endl;
    cout << "ticks_calc_recfg_time  " << aes.ticks_calc_recfg_time << endl;
    cout << "ticks_acquire          " << aes.ticks_acquire << endl;
    cout << "ticks_alloc_hw_res     " << aes.ticks_alloc_hw_res << endl;
    cout << "ticks_recfg            " << aes.ticks_recfg << endl;
    cout << "ticks_get_state        " << aes.ticks_get_state << endl;
    cout << "ticks_change_domain_hw " << aes.ticks_change_domain_hw << endl;
    cout << "ticks_set_state        " << aes.ticks_set_state << endl;
    cout << "ticks_method           " << aes.ticks_method << endl;
    cout << "ticks_release          " << aes.ticks_release << endl;

    cout << "Encrypting 4M bytes of data" << endl;

    chrono.reset();
    chrono.start();

    for(i = 0; i < (4*1024*1024)*8/128; i++)
        result = aes.dummy_cipher(pkt);

    chrono.stop();
    cout << "4M bytes of data encrypted in " << chrono.ticks() << " ticks" << endl;

    cout << "End" << endl;

    return 0;
}
