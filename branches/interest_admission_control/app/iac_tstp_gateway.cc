#include <machine.h>
#include <alarm.h>
#include <smart_data.h>

using namespace EPOS;

OStream cout;

typedef TSTP::Coordinates Coordinates;
typedef TSTP::Region Region;

const unsigned int PERIOD = 145;
const unsigned int INTEREST_PERIOD = 1000000;
const unsigned int INTEREST_EXPIRY = 2 * INTEREST_PERIOD;

int main()
{
    cout << "TSTP Gateway test" << endl;

    cout << "My machine ID is:";
    for(unsigned int i = 0; i < 8; i++)
        cout << " " << hex << Machine::id()[i];
    cout << endl;
    cout << "You can set this value at src/component/tstp_init.cc to set initial coordinates for this mote." << endl;

    cout << "My coordinates are " << TSTP::here() << endl;
    cout << "The time now is " << TSTP::now() << endl;
    cout << "I am" << (TSTP::here() == TSTP::sink() ? " " : " not ") << "the sink" << endl;

    Coordinates center_sensor(10,10,0);
    Region region_sensor(center_sensor, 0, TSTP::now(), -1);

    cout << "I will now ask for Acceleration data from any sensor located in " << region_sensor << endl;

    Acceleration acceleration3(region_sensor, INTEREST_EXPIRY, INTEREST_PERIOD);
    Acceleration acceleration1(region_sensor, INTEREST_EXPIRY, 145 * 2);
    Acceleration acceleration2(region_sensor, INTEREST_EXPIRY, 145 * 2);

    while(true) {
        Alarm::delay(INTEREST_PERIOD);
        cout << "Acceleration in " << acceleration1.location() << " at " << acceleration1.time() << " was " << acceleration1 << endl;
        cout << "Acceleration in " << acceleration2.location() << " at " << acceleration2.time() << " was " << acceleration2 << endl;
        cout << "Acceleration in " << acceleration3.location() << " at " << acceleration3.time() << " was " << acceleration3 << endl;
    }

    return 0;
}
