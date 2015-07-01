#include <utility/ostream.h>
#include <usb.h>

using namespace EPOS;

OStream cout;

int main()
{
    char buffer[512];
    while(true)
    {
        unsigned int sz = eMote3_USB::get_data(buffer, 512);
        for(unsigned int i = 0; i<sz; i++)
            eMote3_USB::put(buffer[i]);
    }
    
    return 0;
}
