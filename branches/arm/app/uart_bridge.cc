#include <utility/string.h>
#include <network.h>
#include <uart.h>
#include <wiegandradiotstp.h>

using namespace EPOS;

RFID_1 Wiegand::id; // TODO: unit

void door_print (TSTP::Interest * interestdata) {
    auto unit = interestdata->unit();

    if (unit == Door_State_1::UNIT) {
        cout << "Door_State<1> ";
    } else if (unit == Door_State_2::UNIT) {
        cout << "Door_State<2> ";
    } else if (unit == Door_State_3::UNIT) {
        cout << "Door_State<3> ";
    } else if (unit == Door_State_4::UNIT) {
        cout << "Door_State<4> ";
    }
    cout << *interestdata->data<bool>() << endl;
}

void wiegand_print (TSTP::Interest * interestdata) {
    auto id = interestdata->data<ID_Code_Msg>();
    cout << id->facility_data << ":" << id->user_data << endl;
}

int main()
{
    Network::init();
    auto tstp = TSTP::get_by_nic(0);

    while(true) {
        Door_State_1 ds1;
        RFID_1 rfid1;
        auto t0 = tstp->time();
        TSTP::Interest door1(tstp, &ds1, TSTP::Remote_Address(611, -545, 52, 0), t0 , t0+60000000, 10000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

        /*
           Door_State_2 ds2;
           TSTP::Interest door2(tstp, &ds2, TSTP::Remote_Address(611, -545, 52, 0), t0 , INT_MAX, 1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

           Door_State_3 ds3;
           TSTP::Interest door3(tstp, &ds3, TSTP::Remote_Address(611, -545, 52, 0), t0 , INT_MAX, 1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);

           Door_State_4 ds4;
           TSTP::Interest door4(tstp, &ds4, TSTP::Remote_Address(611, -545, 52, 0),t0 , INT_MAX, 1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
           */

           TSTP::Interest wiegand1(tstp, &rfid1, TSTP::Remote_Address(611, -545, 52, 0),t0, t0+60000000, 10000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

           RFID_2 rfid2;
           TSTP::Interest wiegand2(tstp, &rfid2, TSTP::Remote_Address(611, -545, 52, 0),t0, t0+60000000, 1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

           RFID_3 rfid3;
           TSTP::Interest wiegand3(tstp, &rfid3, TSTP::Remote_Address(611, -545, 52, 0),t0, t0+60000000, 1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

           RFID_4 rfid4;
           TSTP::Interest wiegand4(tstp, &rfid4, TSTP::Remote_Address(611, -545, 52, 0),t0, t0+60000000,1000000, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        User_Timer_1::delay(5 * 60000000);
        cout << "Waking up!" << endl;
    }

    while (true);
    return 0;
}
