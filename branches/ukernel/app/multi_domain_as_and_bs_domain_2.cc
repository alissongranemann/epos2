/*
 * Implementation notes:
 * This file implements Domain 2.
 * Assuming Domain 0 is implemented in pc_loader.cc and
 * Domain 1 is implemented in multi_domain_as_and_bs.cc
 *
 * */
#include <utility/ostream.h>

using namespace EPOS;


const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;

OStream cout;

int main()
{
    cout << "This is Domain 2" << endl;
    cout << "Task on Guest OS 2 (Domain 2) starting..." << endl;
    cout << "Iterations = " << ITERATIONS << endl;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        for(int j = 0; j < 79; j++) {
            cout << "b";
        }
        cout << "" << endl;
    }

    cout << "Task on Guest OS 2 (Domain 2) finishing..." << endl;

    return 0;
}
