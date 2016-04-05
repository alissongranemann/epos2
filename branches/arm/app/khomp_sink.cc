#include <utility/string.h>
#include <network.h>
#include <uart.h>
#include <wiegand.h>

using namespace EPOS;

OStream cout;

void door_print (TSTP::Interest * interestdata) {
    auto unit = interestdata->unit();

    if (unit == Wiegand::Door_State_1::UNIT) {
        cout << "Wiegand::Door_State_1 ";
    } else if (unit == Wiegand::Door_State_2::UNIT) {
        cout << "Wiegand::Door_State_2 ";
    } else if (unit == Wiegand::Door_State_3::UNIT) {
        cout << "Wiegand::Door_State_3 ";
    } else if (unit == Wiegand::Door_State_4::UNIT) {
        cout << "Wiegand::Door_State_4 ";
    }
    cout << *interestdata->data<bool>() << endl;
}

void wiegand_print (TSTP::Interest * interestdata) {
    auto unit = interestdata->unit();

    if (unit == Wiegand::RFID_1::UNIT) {
        cout << "Wiegand::RFID_1 ";
    } else if (unit == Wiegand::RFID_2::UNIT) {
        cout << "Wiegand::RFID_2 ";
    } else if (unit == Wiegand::RFID_3::UNIT) {
        cout << "Wiegand::RFID_3 ";
    } else if (unit == Wiegand::RFID_4::UNIT) {
        cout << "Wiegand::RFID_4 ";
    }
    auto id = interestdata->data<Wiegand::ID_Code_Msg>();
    cout << id->facility << ":" << id->serial << endl;
}

int main()
{
    int n = 25000;
    while(n--) {
        cout << n << endl;
    }
    Network::init();
    auto tstp = TSTP::get_by_nic(0);

    Wiegand::Door_State_1 ds1;
    Wiegand::Door_State_2 ds2;
    Wiegand::Door_State_3 ds3;
    Wiegand::Door_State_4 ds4;
    Wiegand::RFID_1 rfid1;
    Wiegand::RFID_2 rfid2;
    Wiegand::RFID_3 rfid3;
    Wiegand::RFID_4 rfid4;
    while(true) {
        auto t0 = tstp->time() + 2000000;
        TSTP::Interest door1(tstp, &ds1, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

        TSTP::Interest door2(tstp, &ds2, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

        TSTP::Interest door3(tstp, &ds3, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

        TSTP::Interest door4(tstp, &ds4, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

        TSTP::Interest wiegand1(tstp, &rfid1, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        TSTP::Interest wiegand2(tstp, &rfid2, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        TSTP::Interest wiegand3(tstp, &rfid3, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        TSTP::Interest wiegand4(tstp, &rfid4, TSTP::Remote_Address(611, -545, 52, 1000), t0, t0 + 60000000, 5000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        Alarm::delay(70000000);
        cout << "Waking up!" << endl;
    }

    while (true);
    return 0;
}
