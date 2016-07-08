// EPOS eMote3 Interrupt Controller Initialization

#include <cpu.h>
#include <ic.h>

__BEGIN_SYS

/* 
   This is a workaround to run in privileged mode.
   For some reason, the control register is updated 
   when main() is entered, making it unprivileged, which 
   will cause some instructions to generate a hard fault
   (like trying to write to STCTRL register for enabling/disabling
   interrupts). The manuals never say that the privilege bit is updated.
   The only way to update the register back to privileged 
   is in an interrupt handler (this one, in our case).
   This implies that in the beginning of main(), there must be 
   an "svc <immediate>" instruction, which makes this a very bad workaround.

   TODO: find out a way to prevent the control register from being 
   updated upon entering main(), then remove this function.
*/
void _handle_svc(const unsigned int &whatever)
{
	int c;
	ASM("mrs %0, control" : "=r"(c));
	db<Init, IC>(WRN) << "control = " << c << endl;
	ASM("mrs r0, control \n"
		"and r0, r0, #2 \n"
		"msr control, r0 \n"
		"isb \n"
		"mrs %0, control" :"=r"(c)::"r0");
	db<Init, IC>(WRN) << "control = " << c << endl;
}

void Cortex_M_IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable(); // will be reenabled at Thread::init()
    db<Init, IC>(TRC) << "IC::init:CCR = " << scs(CCR) << endl;
    scs(CCR) |= BASETHR; // BUG
    db<Init, IC>(TRC) << "IC::init:CCR = " << scs(CCR) << endl;

    disable(); // will be enabled on demand as handlers are registered
    
    // Set all interrupt handlers to int_not()
    for(Interrupt_Id i = 0; i < INTS; i++)
        _int_vector[i] = int_not;

    // SVCall. See the note above.
    _int_vector[11] = _handle_svc;
}

__END_SYS
