#include <machine.h>
#include <alarm.h>
#include <smart_data.h>

using namespace EPOS;

OStream cout;

typedef TSTP::Coordinates Coordinates;
typedef TSTP::Region Region;

const unsigned int MAC_PERIOD = 145;
const unsigned int INTEREST_PERIOD = 1000000;
const unsigned int INTEREST_EXPIRY = 2 * INTEREST_PERIOD;

int main()
{
    cout << "TSTP IAC Gateway test" << endl;

    cout << "My coordinates are " << TSTP::here() << endl;
    cout << "The time now is " << TSTP::now() << endl;
    cout << "I am" << (TSTP::here() == TSTP::sink() ? " " : " not ") << "the sink" << endl;

    Coordinates center_sensor(10,10,0);
    Region region_sensor(center_sensor, 0, TSTP::now(), -1);

    Acceleration acceleration1(region_sensor, INTEREST_EXPIRY, INTEREST_PERIOD); //1% burden
    Acceleration acceleration2(region_sensor, INTEREST_EXPIRY, MAC_PERIOD * 2); //50% burden
    Acceleration acceleration3(region_sensor, INTEREST_EXPIRY, MAC_PERIOD * 2); //50% burden

    return 0;
}
