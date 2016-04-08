// EPOS cout Test Program

#include <utility/ostream.h>
#include <network.h>

using namespace EPOS;

OStream cout;
TSTP * tstp;

void meter_handler(TSTP::Sensor * s)
{
    cout << "Measuring meter!" << endl;
    unsigned int measurement = 1;//do_meter_measurement();
    *(s->data<unsigned int>()) = measurement;
}

int main()
{
    Network::init();
    tstp = TSTP::get_by_nic(0);
    tstp->bootstrap();

    TSTP::Meter m;
    TSTP::Sensor meter(tstp, &m, 1000, 2000, 3, &meter_handler);

    while(true);

    return 0;
}

