#include <http.h>
#include <tstp.h>
#include <gpio.h>
#include <machine.h>
#include <smart_data.h>
#include <transducer.h>
#include <utility/ostream.h>

using namespace EPOS;

const RTC::Microsecond INTEREST_PERIOD = 10 * 60 * 1000000;
const RTC::Microsecond INTEREST_EXPIRY = 2 * INTEREST_PERIOD;
const char STATION_NAME[] = "f_99";

class DB_Entry
{
    public:
        DB_Entry(unsigned int i = 0) :
            _zero(0), _timestamp(i), _level(i), _turbidity(i), _pluviometer(i), _signal_level(i) {}
        DB_Entry(const char * station_name, unsigned int timestamp, unsigned short level, unsigned short turbidity, unsigned char pluviometer, char signal_level) :
            _zero(0), _timestamp(timestamp), _level(level), _turbidity(turbidity), _pluviometer(pluviometer), _signal_level(signal_level)
    {
        _station[0] = station_name[0];
        _station[1] = station_name[1];
        _station[2] = station_name[2];
        _station[3] = station_name[3];
    }

    private:
        char _station[4];
        char _zero;
        unsigned int _timestamp;
        unsigned short _level;
        unsigned short _turbidity;
        unsigned char _pluviometer;
        char _signal_level;
}__attribute__((packed));

int work()
{
    unsigned int a = 0;
    OStream cout;

    M95 * m95 = M95::get(0);

    TSTP::Time t = m95->now();
    TSTP::adjust(t);

    m95->off();

    Coordinates center_station0(-6000,4500,0);
    Region region_station0(center_station0, 0, TSTP::now(), -1);

    Water_Level level(region_station0, INTEREST_EXPIRY, INTEREST_PERIOD);
    Water_Turbidity turbidity(region_station0, INTEREST_EXPIRY, INTEREST_PERIOD);
    Rain rain(region_station0, INTEREST_EXPIRY, INTEREST_PERIOD, true);

    while(Periodic_Thread::wait_next()) {

        TSTP::Time first = level.expired() ? -1ull : level.time();
        if(!turbidity.expired() && (turbidity.time() < first))
            first = turbidity.time();
        if(!rain.expired() && (rain.time() < first))
            first = rain.time();

        cout << "first = " << first << endl;

        DB_Entry e(STATION_NAME, first / 1000000, level, turbidity, rain, m95->rssi());

        if(first == -1ull)
            continue;

        m95->on();
        int ret = Quectel_HTTP::post("http://150.162.62.3/data/hydro/put.php", &e, sizeof(DB_Entry));
        cout << "Post = " << ret << endl;
        m95->off();
    }

    return 0;
}

int main()
{
    Periodic_Thread * t = new Periodic_Thread(INTEREST_EXPIRY + INTEREST_EXPIRY / 100, work);
    t->join();

    return 0;
}
