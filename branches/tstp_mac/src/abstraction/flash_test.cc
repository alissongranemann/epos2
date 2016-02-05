#include <utility/ostream.h>
#include <machine.h>
#include <alarm.h>
#include <flash.h>

using namespace EPOS;

int main()
{
    OStream cout;

    const unsigned int words_to_write = 128;
    const unsigned int flash_address = Flash::size() / 2;

    unsigned int data[words_to_write];

    for (auto i = 0u; i < words_to_write; ++i) {
        data[i] = 2*i;
    }

    cout << "Delaying two seconds to avoid writing to flash too much..." << endl;
    Alarm::delay(2000000);

    cout << "Reading flash" << endl;
    cout << "If this is the first time this test is running, expect garbage below." << endl;
    cout << "Otherwise, it should print the sequence [0, 2, 4 ... " << 2 * (words_to_write - 1)<< "]." << endl;

    unsigned int from_flash[words_to_write];

    for (auto i = 0u; i < words_to_write; ++i) {
        from_flash[i] = Flash::read(flash_address + 4*i);
    }

    bool test_passed = true;
    for (auto i = 0u; i < words_to_write; ++i) {
        if(from_flash[i] != data[i])
            test_passed = false;
        cout << from_flash[i] << ", ";
    }
    cout << endl;

    if(test_passed)
    {
        cout << "Test done! Flash content seems right!" << endl;
        while(true);
    }

    cout << "Writing to flash..." << endl;
    Flash::write(flash_address, data, words_to_write*4);
    cout << "Done!" << endl;

    cout << ">> If this line is printed more than once, the test failed!! <<" << endl;
    cout << "Rebooting and starting over." << endl;

    Machine::reboot();
}
