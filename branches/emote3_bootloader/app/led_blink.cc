#include <machine.h>

using namespace EPOS;

int main()
{
    GPIO g('c',3, GPIO::OUTPUT);
    for(bool b=false;;b=(b+1)%2)
    {
        g.set(b);
        for(volatile int t=0;t<0xfffff;t++);
    }
    return 0;
}
