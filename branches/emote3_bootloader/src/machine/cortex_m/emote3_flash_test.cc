#include <utility/ostream.h>
#include <machine/cortex_m/emote3.h>

using namespace EPOS;

unsigned int read_flash(unsigned int address)
{
    return *reinterpret_cast<unsigned int *>(address);
}

int main()
{
    OStream cout;

    unsigned int data[1024];

    for (auto i = 0; i < 1024; ++i) {
        data[i] = 2*i;
    }

    unsigned int flash_address = 0x27ec00u;

    eMote3_Flash::write(flash_address, data, 1024*4);

    unsigned int from_flash[1024];

    for (auto i = 0; i < 1024; ++i) {
        from_flash[i] = read_flash(flash_address + i);
    }

    while (true) {
        for (auto i = 0; i < 1024; ++i) {
            cout << from_flash[i] << "\n";
        }
    }
}
