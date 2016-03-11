#include <machine/cortex_m/bootloader.h>
#include <machine.h>
#include <usb.h>
#include <gpio.h>

using namespace EPOS;

extern "C" { extern void _fini(); } // Defined in armv7_crtbegin.c

int main()
{
    eMote3_Bootloader bl;
    do {
        bl.run();
    } while(!eMote3_Bootloader::vector_table_present());


    eMote3_USB::disable();
    eMote3_GPTM::delay(1000000);

    _fini(); // Call global destructors

    eMote3_Bootloader::jump_to_epos();

    return 0;
}
