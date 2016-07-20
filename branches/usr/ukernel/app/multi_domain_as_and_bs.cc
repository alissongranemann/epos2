/*
 * Implementation notes:
 * This file implements Domain 1.
 * Assuming Domain 0 is implemented in pc_loader.cc and
 * Domain 2 is implemented in domain_2.cc
 *
 * */
#include <utility/ostream.h>

using namespace EPOS;


const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;

OStream cout;

int main()
{
    cout << "This is Domain 1" << endl;
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;
    cout << "Iterations = " << ITERATIONS << endl;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        for(int j = 0; j < 79; j++) {
            cout << "a";
        }
        cout << "" << endl;
    }

    cout << "Task on Guest OS 1 (Domain 1) finishing..." << endl;

    return 0;
}
