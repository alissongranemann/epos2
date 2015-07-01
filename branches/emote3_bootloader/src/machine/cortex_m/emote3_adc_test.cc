// EPOS ADC Test Program

#include <utility/ostream.h>
#include <machine/cortex_m/emote3.h>

using namespace EPOS;

int main()
{
    OStream cout;

    while (true) {
        auto value = ADC::get(ADC::SINGLE6);
        cout << value;
        cout << "\n";
    }
}
