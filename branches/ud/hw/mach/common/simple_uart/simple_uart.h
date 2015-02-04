/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef UART_H_
#define UART_H_

#include <systemc.h>
#include <queue>
#include <pty.h>
#include <sys/time.h>

class uart_fifo : public std::queue<unsigned int>{
	typedef std::queue<unsigned int> base;

	sc_semaphore sem;

	public:
	uart_fifo() :base(),
		sem(1){

	}

	 bool empty() { sem.wait(); bool tmp = base::empty(); sem.post(); return tmp; }

	 unsigned int size() { sem.wait(); unsigned int tmp = base::size(); sem.post(); return tmp; }

	 unsigned int& front() { sem.wait(); unsigned int &tmp = base::front(); sem.post(); return tmp;}

	 unsigned int& back() { sem.wait(); unsigned int &tmp = base::back(); sem.post(); return tmp;}

	 void push(const unsigned int& __x) { sem.wait(); base::push(__x); sem.post();}

	 void pop() { sem.wait(); base::pop(); sem.post();}

};

template<unsigned int BUFFER_SIZE>
class pseudo_terminal {
public:
	int rtnVal;
	int mpty, spty, c, dev;
	char *pName;
	char *ptyBuff;
	int buff_st;
	int buff_st_max;

	pseudo_terminal(){
		rtnVal=0;
		c=0; dev=0;
		pName=NULL;

		ptyBuff = new char[BUFFER_SIZE];
		buff_st = 0;
		buff_st_max = 0;

		rtnVal = openpty(&mpty, &spty, NULL, NULL, NULL);

		// Check if Pseudo-Term pair was created
		if(rtnVal != -1)
		{
			pName = ptsname(mpty);
			std::cout << "UART: Opened pseudo terminal at " << pName << "\n";
			//std::cout << "UART: waiting 5 sec" << std::endl; sleep(5);

		}
		else
		{
			std::cout << "UART: Failed to openOpened pseudo terminal\n";
		}

	}

	~pseudo_terminal(){
		if (ptyBuff) delete[] ptyBuff;
	}


	char terminal_read(){
		char ret_val = 0;

		if(buff_st < buff_st_max){
			ret_val = ptyBuff[buff_st];
			++buff_st;
		}
		else{
			buff_st = 0;
			buff_st_max = 0;
			while(buff_st_max <= 0) buff_st_max = read(mpty, ptyBuff, BUFFER_SIZE);

			ret_val = terminal_read();
		}

		return ret_val;
	}

	bool pty_input_available() {
		struct timeval tv;
		fd_set fds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_SET(mpty, &fds);
		select(mpty+1, &fds, NULL, NULL, &tv);
		return FD_ISSET(mpty, &fds);
	}

	bool input_available() {
		return (buff_st < buff_st_max) ? true : pty_input_available();
	}


	void terminal_write(char val){
		write(mpty, &val, 1);
	}
};

SC_MODULE(simple_uart) {

	sc_in<bool>	clk_i;
	sc_in<bool>	rst_i;

	sc_in<bool> we_i;
	sc_in<bool> stb_i;
	sc_in<bool> cyc_i;
	sc_out<bool> ack_o;
	sc_in<sc_uint<3> > adr_i;
	sc_in<sc_uint<32> > dat_i;
	sc_out<sc_uint<32> >dat_o;
	sc_out<bool> rx_int_o;
	sc_out<bool> tx_int_o;
	sc_out<bool> tx_o;
	sc_in<bool> rx_i;
	sc_out<bool> baud_o;


	SC_CTOR(simple_uart)
	{
		SC_THREAD(tx_thread);

		SC_THREAD(rx_thread);

		SC_CTHREAD(rxtx_int, clk_i.pos());
		reset_signal_is(rst_i, true);

		SC_CTHREAD(rtl_proc, clk_i.pos());
		reset_signal_is(rst_i, true);


		real_time_start = get_real_time_ms();
		SC_THREAD(print_time);

	}

	enum{
		SUART_CLKDIV = 0,
		SUART_TXLEVEL = 1,
		SUART_RXLEVEL = 2,
		SUART_TXCHAR = 3,
		SUART_RXCHAR = 4
	};


private:

	pseudo_terminal<64*1024*1024> pty;

	uart_fifo tx_fifo;
	sc_event tx_fifo_add;
	void tx_thread();

	uart_fifo rx_fifo;
	void rx_thread();

	void rxtx_int();

	void rtl_proc();

	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	long int real_time_start;

	void print_time();

	long int get_real_time_ms(){
	    struct timeval now;
	    gettimeofday(&now, NULL);
	    long int ms_now = now.tv_sec * 1000 + now.tv_usec / 1000;
	    return ms_now;
	}
};

#endif /* UART_H_ */
