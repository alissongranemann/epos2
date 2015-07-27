#include <alarm.h>
#include <utility/ostream.h>
#include <uart.h>
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

const auto MINUTES = 5u;
const auto MINUTE_IN_US = 60000000u;

const auto IDENTIFIER = "hidro1";

OStream cout;

class Watchdog {
    const static unsigned int BASE = 0x400d5000;

    enum Offset {
        SMWDTHROSC_WDCTL = 0x0
    };

    typedef unsigned int Reg32;

    static volatile Reg32 &reg(Offset o) {
        return *(reinterpret_cast<Reg32*>(BASE + o));
    }

public:
    static volatile Reg32 &wdctl() {
        return reg(SMWDTHROSC_WDCTL);
    }

    enum Mask {
        ENABLE_2MS = 0xB, // really 1.9ms
        ENABLE_15MS = 0xA, // really 15.625ms
        ENABLE_25MS = 0x9,
        ENABLE_1S = 0x8
    };

    Watchdog(Mask timeout = ENABLE_1S)
    {
        cout << BASE + SMWDTHROSC_WDCTL;
        cout << "wdctl is" << wdctl() << endl;
        wdctl() = timeout;
    }

    void kick() const
    {
        wdctl() = (wdctl() & 0xF) | (0xA << 4);
        wdctl() = (wdctl() & 0xF) | (0x5 << 4);
    }
};

unsigned int from_flash(unsigned int address)
{
    return *reinterpret_cast<unsigned int*>(address);
}

void blink_led(unsigned int times, unsigned int period = 50000)
{
    auto led = GPIO{'c', 3, GPIO::OUTPUT};
    for (auto i = 0u; i < times; ++i) {
        led.set();
        eMote3_GPTM::delay(period);
        led.clear();
        eMote3_GPTM::delay(period);
    }
}

volatile bool dog_status = false;

const unsigned int WD_TIMEOUT = 30000000;

void alarm_handler()
{
	cout << "watchdog overflow! rebooting...\n";
    eMote3_ROM::reboot();
}

Function_Handler reboot_handler{alarm_handler};

void software_watchdog()
{
    static Alarm *alarm = 0;
    if (dog_status) {
        if (!alarm) {
            cout << "watchdog started\n";
            alarm = new Alarm(WD_TIMEOUT, &reboot_handler);
        }
    } else {
        delete alarm;
        cout << "watchdog deleted\n";
        alarm = 0;
    }
}

Function_Handler wdt_handler{software_watchdog};
Alarm watchdog{1000000, &wdt_handler, Alarm::INFINITE};

int main()
{
    auto a = GPIO{'b', 0, GPIO::OUTPUT};
    auto b = GPIO{'b', 3, GPIO::OUTPUT};

    blink_led(10);

    a.clear();
    b.clear();

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
    auto uart = UART{9600, 8, 0, 1, 1};

    dog_status = true;
    auto gprs = eMote3_GPRS{pwrkey, status, uart};
    dog_status = false;

    cout << "gprs created and status is " << status.get() << "\n";
    blink_led(4, 100000);

    dog_status = true;
    gprs.on();
    dog_status = false;

    cout << "gprs on and status is " << status.get() << "\n";
    blink_led(4, 100000);

    gprs.use_dns();

    for (auto reading = index; reading < N_READINGS; ++reading) {
        a.set();
        b.set();

        eMote3_GPTM::delay(3000000);

        sensors[0] = ADC::get(ADC::SINGLE4);
        sensors[1] = ADC::get(ADC::SINGLE6);

        a.clear();
        b.clear();

        auto address = flash_address + (reading*sizeof(sensors));
        eMote3_Flash::write(address, sensors, sizeof(sensors));

        auto next = reading + 1;
        eMote3_Flash::write(INDEX_ADDRESS, &next, sizeof(next));

        char buf[100] = "data=";
        char aux[32] = "";
        strcat(buf, IDENTIFIER);
        strcat(buf, ",");
        aux[utoa(sensors[0], aux)] = '\0';
        strcat(buf, aux);
        strcat(buf, ",");
        aux[utoa(sensors[1], aux)] = '\0';
        strcat(buf, aux);

        dog_status = true;
        gprs.on();
        dog_status = false;

        auto send = gprs.send_http_post("http://sv13.lisha.ufsc.br/hidro", buf, strlen(buf));
        blink_led(send ? 1 : 3, 1000000);
        cout << "send:" << send << "\n";

        for (auto i = 0u; i < MINUTES; ++i) {
            eMote3_GPTM::delay(MINUTE_IN_US);
        }
    }

    cout << "out of flash memory! rebooting...\n";
    eMote3_ROM::reboot();

    while (true);
}
