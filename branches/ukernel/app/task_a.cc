// EPOS Application

#include <system/config.h>

using namespace EPOS;

const int iterations = 10;

OStream cout;

int main()
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 79; i++)
            cout << "a";
        cout << endl;
        // Thread::yield();
    }

    // Thread::self()->suspend();
    return 'A';
}
