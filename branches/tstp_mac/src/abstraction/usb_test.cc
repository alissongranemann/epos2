#include <usb.h>

using namespace EPOS;

int main()
{
    while(true) {
        USB::put(USB::get());
    }

    return 0;
}
