// EPOS EPOSSOC_IC Implementation

#include <machine.h>
#include <ic.h>

__BEGIN_SYS

// Class attributes
EPOSSOC_IC::MM_Reg * EPOSSOC_IC::_regs = reinterpret_cast<EPOSSOC_IC::MM_Reg *>(EPOSSOC_IC::BASE_ADDRESS);
EPOSSOC_IC::Interrupt_Handler EPOSSOC_IC::_int_vector[EPOSSOC_IC::INTS];

void EPOSSOC_IC::int_no_handler(Interrupt_Id interrupt) {
	db<EPOSSOC_IC>(WRN) << "Intr " << interrupt << " occurred but had no handler associated !\n";
}

void EPOSSOC_IC::int_handler(Interrupt_Id i) {
	db<EPOSSOC_IC>(TRC) << "\n\n$EPOSSOC_IC::int_handler$\n\n";
	
	// Check pending and not masked interrupts
    unsigned int int_pending = _regs->pending & ~_regs->mask;

    // handle the first one set
    unsigned int int_no, int_bit;
    for (int_no=0, int_bit=1; int_no < INTS; int_no++, int_bit <<= 1){
      if (int_pending & int_bit){
	    //Ack Interrupt
	    _regs->pending = int_bit; 
        //dispatch specific handler
		void (*h)(unsigned int);
	    h = EPOSSOC_IC::int_vector(int_no);
	    h(int_no);
	    if (Traits<EPOSSOC_IC>::ATTEND_ONLY_ONE) {
			return;
		} 
      }
    }
	
}

__END_SYS
