// EPOS-- Plasma UART Mediator Declarations

#ifndef __plasma_uart_h
#define __plasma_uart_h

class Plasma_UART
{
public:

    Plasma_UART(unsigned int unit = 0) : _unit(unit) {
    }
    bool has_data() {
	irq_status = (volatile unsigned int *)0x20000020;
	return ((*irq_status & 0x01) != 0);
    }
    bool can_send() {
	irq_status = (volatile unsigned int *)0x20000020;
	return ((*irq_status & 0x02) != 0);
    }
    unsigned char get() {
            irq_status = (volatile unsigned int *)0x20000020;
            while((*irq_status & 0x01) == 0);
	    unsigned char byte = (unsigned char)*(volatile unsigned int*)0x20000000;
	    return byte;
    }
    void put(unsigned char c) {
	    irq_status = (volatile unsigned int*)0x20000020;
	    while((*irq_status & 0x02) == 0);
	    *(volatile unsigned int*)0x20000000 = (unsigned int)c;
    }

private:
    int _unit;
    volatile unsigned int * irq_status;
    volatile unsigned int * buffer;

};

#endif
