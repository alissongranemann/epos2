#include <modbus_ascii.h>
#include <utility/ostream.h>
#include <utility/random.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

void Modbus::act(char * modbus_message, unsigned int message_size)
{
    cout << "Modbus::act()" << endl;

    // 1) Parse the modbus message (it was already checked to be syntactically correct)
    if(message_size == 13) { // Remember to account for \r\n
        auto id = Modbus_ASCII::decode(modbus_message[1], modbus_message[2]);
        auto command = Modbus_ASCII::decode(modbus_message[3], modbus_message[4]);
        auto number_of_coils = Modbus_ASCII::decode(modbus_message[5], modbus_message[6]);
        auto data = Modbus_ASCII::decode(modbus_message[7], modbus_message[8]);
        bool presence_sensor_state;

        if(id == 0xA5 and command == Modbus_ASCII::READ_COILS and number_of_coils > 0) {            
            // 2) Act upon it! you can use the method send(char * modbus_message, unsigned int size) to send (syntactically correct) modbus messages you create
            presence_sensor_state = data & 1;

            // Build the modbus command message
            char command[17];
            unsigned char lrc = 0;

            command[0] = ':';

            Modbus_ASCII::encode(0xA0, &command[1], &command[2]);
            lrc += 0xA0;

            Modbus_ASCII::encode(Modbus_ASCII::WRITE_SINGLE_COIL, &command[3], &command[4]);
            lrc += Modbus_ASCII::WRITE_SINGLE_COIL;

            Modbus_ASCII::encode(0, &command[5], &command[6]);
            Modbus_ASCII::encode(0, &command[7], &command[8]);
            Modbus_ASCII::encode(0, &command[9], &command[10]);

            Modbus_ASCII::encode(presence_sensor_state, &command[11], &command[12]);
            lrc += presence_sensor_state;

            lrc = ((lrc ^ 0xff) + 1) & 0xff;
            Modbus_ASCII::encode(lrc, &command[13], &command[14]);

            command[15] = '\r';
            command[16] = '\n';

            // Send the command
            send(command, 17);
        }
    }

    cout << "/Modbus::act()" << endl;
}

int main()
{
    Modbus modbus;

    while(true) {
        static const char poll_presence[] = ":A5010000000159\r\n";
        Alarm::delay(1000000 + (Random::random() % 2000000));
        modbus.send(poll_presence, sizeof poll_presence);
    }

    return 0;
}
