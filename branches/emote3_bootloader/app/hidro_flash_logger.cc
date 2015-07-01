#include <utility/ostream.h>
#include <alarm.h>
#include <machine/cortex_m/emote3.h>

// This application logs two ADC conversions on the flash memory every five
// minutes. On reset, it dumps the flash contents and the last position that
// was actually read on the current run.

using namespace EPOS;

const auto FLASH_LOW     = 0x220000u;
const auto FLASH_HIGH    = 0x27f700u;
const auto INDEX_ADDRESS = 0x27f704u;
const auto N_READINGS    = 4032;


unsigned int from_flash(unsigned int address)
{
    return *reinterpret_cast<unsigned int*>(address);
}

int main()
{
    OStream cout;

    GPIO a{'b', 0, true};
    GPIO b{'b', 3, true};

    a.set(true);
    b.set(true);

    unsigned int sensors[2];
    auto index = (unsigned int){from_flash(INDEX_ADDRESS)};

    cout << "=== Begin flash memory dump ===" << endl;
    cout << "last read index = " << index << endl;
    for (auto i = FLASH_LOW; i < FLASH_LOW + N_READINGS*sizeof(sensors); i += sizeof(sensors)) {
        sensors[0] = from_flash(i);
        sensors[1] = from_flash(i + sizeof(sensors[0]));
        cout << sensors[0] << ", " << sensors[1] << endl;
    }

    cout << "=== End flash memory dump ===" << endl;

    if (index >= N_READINGS) {
        index = 0;
    }

    auto flash_address = FLASH_LOW;

    for (auto reading = index; reading < N_READINGS; ++reading) {
        sensors[0] = ADC::get(ADC::SINGLE4);
        sensors[1] = ADC::get(ADC::SINGLE6);

        auto address = flash_address + (reading*sizeof(sensors));
        eMote3_Flash::write(address, sensors, sizeof(sensors));

        auto next = reading + 1;
        eMote3_Flash::write(INDEX_ADDRESS, &next, sizeof(next));

        for (auto i = 0u; i < 5; ++i) { // Sleeping five minutes at once doesn't work for some reason.
            Alarm::delay(60000000);
        }
    }

    while (true);
}
