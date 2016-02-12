#include <timer.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

int main()
{
    TSTP_MAC::address(TSTP_MAC::Address(0,0,0));

    cout << "TSTP Gateway test" << endl;

    while(true) {
        eMote3_GPTM::delay(1000000);

        TSTP::Data temperature;
        TSTP::Kelvin kelvin;
        TSTP::Region region(0, 0, 0, 10000);
        int n = 6;
        TSTP::Interest in(&temperature, region, TSTP::time_now() + 3000000, n * 1000000, 1000000, kelvin, 1, TSTP::RESPONSE_MODE::SINGLE);

        eMote3_GPTM::delay(1010000);

        while(n--) {
            eMote3_GPTM::delay(1010000);
            cout << temperature << endl;
        }
    }

    return 0;
}
