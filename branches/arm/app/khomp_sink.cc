#include <utility/string.h>
#include <network.h>
#include <uart.h>
#include <wiegand.h>

using namespace EPOS;

OStream cout;

static const unsigned int N_INTERESTS = 3;
static const unsigned int N_SENSORS = 4;

    struct Node_Address_1 { static const int X = 120; static const int Y = -380; static const int Z = -48; };
    struct Node_Address_2 { static const int X = 611; static const int Y = 0;    static const int Z = -148; };
    struct Node_Address_3 { static const int X = 0;   static const int Y = -740; static const int Z = -148; };
    struct Node_Address_4 { static const int X = 611; static const int Y = -545; static const int Z = 52; };
TSTP::Remote_Address addr1(120, -380, -48, 0);
TSTP::Remote_Address addr2(611, 0, -148, 0);
TSTP::Remote_Address addr3(0, -740, -148, 0);
TSTP::Remote_Address addr4(611, -545, 52, 0);
int ds1_count[N_SENSORS + 1];
int ds2_count[N_SENSORS + 1];
int ds3_count[N_SENSORS + 1];
int ds4_count[N_SENSORS + 1];
int id1_count[N_SENSORS + 1];
int id2_count[N_SENSORS + 1];
int id3_count[N_SENSORS + 1];
int id4_count[N_SENSORS + 1];

int turn = 0;

void door_print (TSTP::Interest * interest) {
    auto unit = interest->unit();

    auto addr = interest->last_update_address();
    int idx = N_SENSORS;
    if(addr1.contains(addr)) idx = 0;
    else if(addr2.contains(addr)) idx = 1;
    else if(addr3.contains(addr)) idx = 2;
    else if(addr4.contains(addr)) idx = 3;


    if (unit == Wiegand::Door_State_1::UNIT) {
        cout << "Wiegand::Door_State_1 ";
        ds1_count[idx]++;
    } else if (unit == Wiegand::Door_State_2::UNIT) {
        cout << "Wiegand::Door_State_2 ";
        ds2_count[idx]++;
    } else if (unit == Wiegand::Door_State_3::UNIT) {
        cout << "Wiegand::Door_State_3 ";
        ds3_count[idx]++;
    } else if (unit == Wiegand::Door_State_4::UNIT) {
        cout << "Wiegand::Door_State_4 ";
        ds4_count[idx]++;
    }
    cout << endl;
    cout << *interest->data<bool>() << endl;
    cout << interest->last_update() << endl;
    cout << interest->last_update_address() << endl;
    cout << endl;
}

void wiegand_print (TSTP::Interest * interest) {
    auto unit = interest->unit();

    auto addr = interest->last_update_address();
    int idx = N_SENSORS;
    if(addr1.contains(addr)) idx = 0;
    else if(addr2.contains(addr)) idx = 1;
    else if(addr3.contains(addr)) idx = 2;
    else if(addr4.contains(addr)) idx = 3;


    if (unit == Wiegand::RFID_1::UNIT) {
        cout << "Wiegand::RFID_1 ";
        id1_count[idx]++;
    } else if (unit == Wiegand::RFID_2::UNIT) {
        cout << "Wiegand::RFID_2 ";
        id2_count[idx]++;
    } else if (unit == Wiegand::RFID_3::UNIT) {
        cout << "Wiegand::RFID_3 ";
        id3_count[idx]++;
    } else if (unit == Wiegand::RFID_4::UNIT) {
        cout << "Wiegand::RFID_4 ";
        id4_count[idx]++;
    }
    cout << endl;
    auto id = interest->data<Wiegand::ID_Code_Msg>();
    cout << id->facility << ":" << id->serial << endl;
    cout << interest->last_update() << endl;
    cout << interest->last_update_address() << endl;
    cout << endl;
}

int main()
{
    int n = 15000;
    while(n--) {
        cout << n << endl;
    }
    Network::init();
    auto tstp = TSTP::get_by_nic(0);
    tstp->bootstrap();

    Wiegand::Door_State_1 ds1[N_SENSORS];
    Wiegand::Door_State_2 ds2[N_SENSORS];
    Wiegand::Door_State_3 ds3[N_SENSORS];
    Wiegand::Door_State_4 ds4[N_SENSORS];
    Wiegand::RFID_1 rfid1[N_SENSORS];
    Wiegand::RFID_2 rfid2[N_SENSORS];
    Wiegand::RFID_3 rfid3[N_SENSORS];
    Wiegand::RFID_4 rfid4[N_SENSORS];

    for(int i = 0; i < N_SENSORS + 1; i++) {
        ds1_count[i] = 0;
        ds2_count[i] = 0; 
        ds3_count[i] = 0;
        ds4_count[i] = 0;
        id1_count[i] = 0;
        id2_count[i] = 0;
        id3_count[i] = 0;
        id4_count[i] = 0;
    }

    while(true) {
        auto period = Traits<TSTP_MAC>::PERIOD * 20 * N_INTERESTS * N_SENSORS;
        auto t0 = tstp->time() + 2 * period;
        auto end = t0 + period * 10;
        cout << "period = " << period << endl;
        cout << "t0 = " << t0 << " (" << (unsigned int)t0 << ")" << endl;
        cout << "end = " << end << " (" << (unsigned int)end << ")" << endl;

        TSTP::Interest door11(tstp, &(ds1[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door21(tstp, &(ds2[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door31(tstp, &(ds3[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door41(tstp, &(ds4[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        TSTP::Interest wiegand11(tstp, &(rfid1[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        TSTP::Interest wiegand21(tstp, &(rfid2[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand31(tstp, &(rfid3[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand41(tstp, &(rfid4[0]), addr1, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        //TSTP::Remote_Address addr2(-100, 100, -100, 0);
        TSTP::Interest door12(tstp, &(ds1[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door22(tstp, &(ds2[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door32(tstp, &(ds3[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door42(tstp, &(ds4[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        TSTP::Interest wiegand12(tstp, &(rfid1[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        TSTP::Interest wiegand22(tstp, &(rfid2[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand32(tstp, &(rfid3[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand42(tstp, &(rfid4[1]), addr2, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        //TSTP::Remote_Address addr3(-200, 200, -100, 0);
        TSTP::Interest door13(tstp, &(ds1[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door23(tstp, &(ds2[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door33(tstp, &(ds3[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door43(tstp, &(ds4[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        TSTP::Interest wiegand13(tstp, &(rfid1[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        TSTP::Interest wiegand23(tstp, &(rfid2[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand33(tstp, &(rfid3[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand43(tstp, &(rfid4[2]), addr3, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        TSTP::Interest door14(tstp, &(ds1[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door24(tstp, &(ds2[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door34(tstp, &(ds3[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        //TSTP::Interest door44(tstp, &(ds4[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &door_print);
        TSTP::Interest wiegand14(tstp, &(rfid1[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        TSTP::Interest wiegand24(tstp, &(rfid2[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand34(tstp, &(rfid3[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);
        //TSTP::Interest wiegand44(tstp, &(rfid4[3]), addr4, t0, end, period, 1, TSTP::SINGLE_EVENT_DRIVEN, &wiegand_print);

        for(int t = 0; t < 12; t++) {
            Alarm::delay(period);
            cout << "Turn " << ++turn << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "ds1[" << i << "] = " << ds1_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "ds2[" << i << "] = " << ds2_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "ds3[" << i << "] = " << ds3_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "ds4[" << i << "] = " << ds4_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "id1[" << i << "] = " << id1_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "id2[" << i << "] = " << id2_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "id3[" << i << "] = " << id3_count[i] << "  ";
            }
            cout << endl;
            for(int i = 0; i < N_SENSORS + 1; i++) {
                cout << "id4[" << i << "] = " << id4_count[i] << "  ";
            }
            cout << endl;
        }

        Alarm::delay(period * 2);
        cout << "Turn " << ++turn << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "ds1[" << i << "] = " << ds1_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "ds2[" << i << "] = " << ds2_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "ds3[" << i << "] = " << ds3_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "ds4[" << i << "] = " << ds4_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "id1[" << i << "] = " << id1_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "id2[" << i << "] = " << id2_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "id3[" << i << "] = " << id3_count[i] << "  ";
        }
        cout << endl;
        for(int i = 0; i < N_SENSORS + 1; i++) {
            cout << "id4[" << i << "] = " << id4_count[i] << "  ";
        }
        cout << endl;

        cout << "Sending new round of interests" << endl;
    }

    while (true);
    return 0;
}
