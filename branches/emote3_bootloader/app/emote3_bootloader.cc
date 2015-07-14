#include <machine/cortex_m/bootloader.h>
#include <machine.h>

using namespace EPOS;

extern "C" { extern void _fini(); } // Defined in armv7_crtbegin.c

void usb_bootloader()
{
    if(Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::usb)
    {
        if(eMote3_Bootloader::vector_table_present())
        {
            eMote3_GPTM timer(1, Traits<Cortex_M_Bootloader>::USB_ALIVE_WAITING_LIMIT);
            timer.enable();
            while(((eMote3_USB::state() < USB_2_0::STATE::DEFAULT)) && (timer.running()));
            if(eMote3_USB::state() < USB_2_0::STATE::DEFAULT)
                return;
        }

        while(!eMote3_USB::initialized());

        do
        {
            eMote3_Bootloader bl;
            bl.run();
        } while(!eMote3_Bootloader::vector_table_present());

    }
}

void nic_bootloader()
{
    if(Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::nic)
    {
        do
        {
            eMote3_Bootloader bl;
            bl.run();
        } while(!eMote3_Bootloader::vector_table_present());
    }
}

int main()
{
    if(Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::usb)
        usb_bootloader();
    else if(Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::nic)
        nic_bootloader();

    if(Traits<Cortex_M_USB>::enabled)
    {
        eMote3_USB::disable();
        eMote3_GPTM::delay(1000000);
    }

    _fini(); // Call global destructors

    // Make sure that all destructors are called before this line
    eMote3_Bootloader::jump_to_epos();

    return 0;
}
