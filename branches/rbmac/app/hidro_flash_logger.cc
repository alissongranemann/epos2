#include <utility/ostream.h>
#include <alarm.h>
#include <machine/cortex_m/emote3.h>
#include <machine/cortex_m/emote3_gprs.h>

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

    auto a = GPIO{'b', 0, GPIO::OUTPUT};
    auto b = GPIO{'b', 3, GPIO::OUTPUT};

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

    auto pwrkey = GPIO{'d', 3, GPIO::OUTPUT};
    auto status = GPIO{'d', 5, GPIO::INPUT};
    auto uart = UART{115200, 8, 0, 1, 1};
    auto gprs = eMote3_GPRS{pwrkey, status, uart};
    cout << "gprs created and status is " << status.get() << "\n";
    gprs.on();
    cout << "gprs on and status is " << status.get() << "\n";

    gprs.use_dns();

    for (auto reading = index; reading < N_READINGS; ++reading) {
        sensors[0] = ADC::get(ADC::SINGLE4);
        sensors[1] = ADC::get(ADC::SINGLE6);


        auto address = flash_address + (reading*sizeof(sensors));
        eMote3_Flash::write(address, sensors, sizeof(sensors));

        auto next = reading + 1;
        eMote3_Flash::write(INDEX_ADDRESS, &next, sizeof(next));

        char buf[100] = "data=";
        char aux[32] = "";
        aux[utoa(sensors[0], aux)] = '\0';
        strcat(buf, aux);
        strcat(buf, ",");
        aux[utoa(sensors[1], aux)] = '\0';
        strcat(buf, aux);

        auto send = gprs.send_http_post("http://sv13.lisha.ufsc.br/", buf, strlen(buf));
        cout << "send:" << send << "\n";
        for (auto i = 0u; i < 30; ++i) { // Sleeping five minutes at once doesn't work for some reason.
            Alarm::delay(60000000);
        }
    }

    while (true);
}
