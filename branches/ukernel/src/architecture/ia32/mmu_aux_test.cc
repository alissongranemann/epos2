#include <utility/ostream.h>
#include <architecture/ia32/mmu_aux.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "MMU_Aux Test." << endl;

    MMU_Aux::check_memory_mapping();

    // MMU_Aux::dump_memory_mapping();
    cout << "Bye." << endl;
}
