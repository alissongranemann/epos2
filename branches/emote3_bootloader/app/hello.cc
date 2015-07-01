#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    while(1)
        cout << "Hello, World! I am the main thread, and I am printing this considerably long sentence until something stops me." << endl;
    
    return 0;
}
