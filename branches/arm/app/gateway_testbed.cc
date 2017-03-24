// TSTP Gateway to be used with tools/eposiotgw/eposiotgw

#include <transducer.h>
#include <smart_data.h>
#include <tstp.h>
#include <utility/ostream.h>
#include <gpio.h>
#include <semaphore.h>

using namespace EPOS;

// TODO
Door_Sensor::Observed Door_Sensor::_observed;
Door_Sensor * Door_Sensor::_dev[Door_Sensor::MAX_DEVICES];

IF<Traits<USB>::enabled, USB, UART>::Result io;

typedef Smart_Data_Common::DB_Series DB_Series;
typedef Smart_Data_Common::DB_Record DB_Record;
typedef TSTP::Coordinates Coordinates;
typedef TSTP::Region Region;

const unsigned int INTEREST_PERIOD = 1 * 60 * 1000000;
const unsigned int INTEREST_EXPIRY = 2 * INTEREST_PERIOD;

void print(const DB_Series & d)
{
    for(unsigned int i = 0; i < sizeof(DB_Series); i++)
        io.put(reinterpret_cast<const char *>(&d)[i]);
    io.put('X'); io.put('X'); io.put('X'); io.put('Z');
}

void print(const DB_Record & d)
{
    CPU::int_disable();
    for(unsigned int i = 0; i < sizeof(Smart_Data_Common::DB_Record); i++)
        io.put(reinterpret_cast<const char *>(&d)[i]);
    io.put('X'); io.put('X'); io.put('X'); io.put('Z');
    CPU::int_enable();
}

template<typename T>
class Printer: public Smart_Data_Common::Observer
{
public:
    Printer(T * t) : _data(t) {
        _data->attach(this);
    }
    ~Printer() { _data->detach(this); }

    void update(Smart_Data_Common::Observed * obs) {
        print(_data->db_record());
    }

private:
    T * _data;
};

class Door_Transform
{
public:
    void apply(Door * source, Door * destination) {
        DB_Record r = source->db_record();

        Door_Sensor::Data d = r.value;

        if(d.authorized()) {
            if(!authorize(d.uid())) {
                d.authorized(false);
                d.open(false);
                *destination = d;
            }
        } else {
            if(authorize(d.uid())) {
                d.authorized(true);
                d.open(true);
                *destination = d;
            }
        }

        r.value = d.uid();
        print(r);
    }

    bool authorize(const Door_Sensor::UID & u) {
        return u != 0; // Accept everyone
    }
};

int main()
{
    // Get epoch time from serial
    TSTP::Time epoch = 0;
    char c = io.get();
    if(c != 'X') {
        epoch += c - '0';
        c = io.get();
        while(c != 'X') {
            epoch *= 10;
            epoch += c - '0';
            c = io.get();
        }
        TSTP::epoch(epoch);
    }

    Alarm::delay(5000000);
    io.put('X'); io.put('X'); io.put('X'); io.put('Z');


    // Interest center points
    Coordinates center_dummy0(10,5,0);
    Coordinates center_dummy1(10,10,0);
    Coordinates center_dummy2(5,15,0);
    //Coordinates center_dummy3(0,15,0);
    //Coordinates center_dummy4(-5,10,0);
    Coordinates center_dummy5(-5,5,0);
    Coordinates center_outlet0(460-730, -250-80, -15);
    Coordinates center_outlet1(-5-730, -30-80, -15);
    Coordinates center_lights1(305-730, -170-80, 220);
    Coordinates center_lux0(10-730,-10-80, 0);
    Coordinates center_door0(-200,150,200);


    // Regions of interest
    Region region_dummy0(center_dummy0, 0, 0, -1);
    Region region_dummy1(center_dummy1, 0, 0, -1);
    Region region_dummy2(center_dummy2, 0, 0, -1);
    //Region region_dummy3(center_dummy3, 0, 0, -1);
    //Region region_dummy4(center_dummy4, 0, 0, -1);
    Region region_dummy5(center_dummy5, 0, 0, -1);
    Region region_outlet0(center_outlet0, 0, 0, -1);
    Region region_outlet1(center_outlet1, 0, 0, -1);
    Region region_lights1(center_lights1, 0, 0, -1);
    Region region_lux0(center_lux0, 0, 0, -1);
    Region region_door0(center_door0, 0, 0, -1);


    // Data of interest
    Current data_dummy0(region_dummy0, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_dummy1(region_dummy1, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_dummy2(region_dummy2, INTEREST_EXPIRY, INTEREST_PERIOD);
    //Current data_dummy3(region_dummy3, INTEREST_EXPIRY, INTEREST_PERIOD);
    //Current data_dummy4(region_dummy4, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_dummy5(region_dummy5, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_outlet0(region_outlet0, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_outlet1(region_outlet1, INTEREST_EXPIRY, INTEREST_PERIOD);
    Current data_lights1(region_lights1, INTEREST_EXPIRY, INTEREST_PERIOD);
    Luminous_Intensity data_lux0(region_lux0, INTEREST_EXPIRY, INTEREST_PERIOD);
    Door data_door0(region_door0, INTEREST_EXPIRY, 0);


    // Output interests to serial
    print(data_dummy0.db_series());
    print(data_dummy1.db_series());
    print(data_dummy2.db_series());
    print(data_dummy5.db_series());
    print(data_outlet0.db_series());
    print(data_outlet1.db_series());
    print(data_lights1.db_series());
    print(data_lux0.db_series());
    print(data_door0.db_series());


    // Data transforms and aggregators
    Percent_Transform<Luminous_Intensity> lights1_transform(150, 1833);
    Door_Transform door0_transform;


    // Event-driven actuators
    Actuator<Current, Percent_Transform<Luminous_Intensity>, Luminous_Intensity> lights1_actuator(&data_lights1, &lights1_transform, &data_lux0);
    Actuator<Door, Door_Transform, Door> door_actuator(&data_door0, &door0_transform, &data_door0);


    // Printers to output received messages to serial
    Printer<Current> p0(&data_dummy0);
    Printer<Current> p1(&data_dummy1);
    Printer<Current> p2(&data_dummy2);
    Printer<Current> p5(&data_dummy5);
    Printer<Current> p6(&data_outlet0);
    Printer<Current> p7(&data_outlet1);
    Printer<Current> p8(&data_lights1);
    Printer<Luminous_Intensity> p9(&data_lux0);
    // Door_Transform already prints Door messages


    // And we're done! TSTP will do all the work for us.
    Thread::self()->suspend();

    return 0;
}
