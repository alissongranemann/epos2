#include <alarm.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP Gateway test" << endl;

    TSTP::Data presence;
    TSTP::Unit presence_u(TSTP::Unit::PRESENCE);
    TSTP::Region region(100, 100, 0, 10);
    TSTP::Interest in(&presence, region, TSTP::time_now() + 3000000, 60000000, 1000000, presence_u, 1, TSTP::RESPONSE_MODE::SINGLE);

    while(true) {
        Alarm::delay(1000000);
        cout << presence << endl;
    }

    return 0;
}
