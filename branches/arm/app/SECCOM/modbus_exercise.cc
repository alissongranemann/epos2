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
    // 2) Act upon it! you can use the method send(char * modbus_message, unsigned int size) to send (syntactically correct) modbus messages you create
    // Hint: You can use the definitions in include/modbus_ascii.h, specially the static method decode()

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
