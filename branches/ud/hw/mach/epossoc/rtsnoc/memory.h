/*
 * cache.h
 *
 *  Created on: 03/03/2010
 *      Author: tiago
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>
#include <components/traits.h>

namespace memory_types {
enum{
    ON_CHIP,
    EXTERNAL
};
};


namespace Timed {

template<int TYPE>
class memory : public sc_module,
	public bus_slave_if {

public:

	memory(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			, unsigned int size_)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
		, mem_size(size_)
	{
		m = new unsigned char [size_];

		memset(m, 0, sizeof(char) * size_);
	}

	~memory() {
		if(m) {
			delete [] m;
			m = (unsigned char *)0;
		}
	}
	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);
	void write_during_elaboration(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const {return start_address;}
	unsigned int get_end_address() const {return end_address;}

	slave_type get_type() const {return MEMORY;}

	void read_delay(){
	    if(TYPE == memory_types::EXTERNAL){
	        Global::delay_cycles<0>(this, 3);
	    }else if (TYPE == memory_types::ON_CHIP){
	        Global::delay_cycles<0>(this, 1);
	    }else{
	        std::cout << "ERROR - Unknown memory type\n";
	        Global::safe_finish(this);
	    }
	}

	void write_delay(){
	    if(TYPE == memory_types::EXTERNAL){
	        Global::delay_cycles<0>(this, 6);
	    }else if (TYPE == memory_types::ON_CHIP){
	        Global::delay_cycles<0>(this, 1);
	    }else{
	        std::cout << "ERROR - Unknown memory type\n";
	        Global::safe_finish(this);
	    }
	}

private:

	unsigned char 	*m;

	unsigned int	start_address;
	unsigned int	end_address;
	unsigned int 	mem_size;
};

template<int TYPE>
void memory<TYPE>::read(unsigned int *data, unsigned int address, int size) {
    unsigned long int ptr;

#ifdef memory_debug
    std::cout << "memory::read() - address: " << std::setfill('0') << std::setw(8) << std::hex << address << "\n";
#endif

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address &3) == 0);
        *data = (unsigned int)ntohl(*(unsigned int*)ptr);
        break;
    case 2:
        //assert((address & 1) == 0);
        *data = (unsigned short)ntohs(*(unsigned int*)ptr);
        break;
    case 1:
        *data = *(unsigned char*) ptr;
        break;

    default: std::cout << "ERROR - READ MEMORY"; break;
    }

#ifdef memory_debug
    std::cout << "memory::read() - valor lido: " << std::setfill('0') << std::setw(8) << std::hex << data_ << "\n";
#endif

    read_delay();
}

template<int TYPE>
void memory<TYPE>::write(unsigned int data, unsigned int address, int size) {
    unsigned long int ptr;

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address & 3) == 0);
        *(unsigned int*) ptr = (unsigned int)htonl(data);
        break;
    case 2:
        //assert((address & 1) == 0);
        *(unsigned short*) ptr = (unsigned short)htons(data);
        break;
    case 1:
        *(unsigned char*) ptr = (unsigned char)data;
        break;
    default:
        std::cout << "ERROR - WRITE MEMORY";
    }

    write_delay();
}

template<int TYPE>
void memory<TYPE>::write_during_elaboration(unsigned int data, unsigned int address, int size) {
    unsigned long int ptr;

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address & 3) == 0);
        *(unsigned int*) ptr = (unsigned int)htonl(data);
        break;
    case 2:
        //assert((address & 1) == 0);
        *(unsigned short*) ptr = (unsigned short)htons(data);
        break;
    case 1:
        *(unsigned char*) ptr = (unsigned char)data;
        break;
    default:
        std::cout << "ERROR - WRITE MEMORY";
    }
}

};

namespace Untimed {
template<int TYPE>
class memory : public sc_module,
    public bus_slave_if {

public:

    memory(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_
            , unsigned int size_)
        : sc_module(nm_)
        , start_address(start_address_)
        , end_address(end_address_)
        , mem_size(size_)
    {
        m = new unsigned char [size_];

        memset(m, 0, sizeof(char) * size_);
    }

    ~memory() {
        if(m) {
            delete [] m;
            m = (unsigned char *)0;
        }
    }
    //interfaces
    void read(unsigned int *data, unsigned int address, int size);
    void write(unsigned int data, unsigned int address, int size);
    void write_during_elaboration(unsigned int data, unsigned int address, int size);

    unsigned int get_start_address() const {return start_address;}
    unsigned int get_end_address() const {return end_address;}

    slave_type get_type() const {return MEMORY;}

private:

    unsigned char   *m;

    unsigned int    start_address;
    unsigned int    end_address;
    unsigned int    mem_size;
};

template<int TYPE>
void memory<TYPE>::read(unsigned int *data, unsigned int address, int size) {
    unsigned long int ptr;

#ifdef memory_debug
    std::cout << "memory::read() - address: " << std::setfill('0') << std::setw(8) << std::hex << address << "\n";
#endif

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address &3) == 0);
        *data = (unsigned int)ntohl(*(unsigned int*)ptr);
        break;
    case 2:
        //assert((address & 1) == 0);
        *data = (unsigned short)ntohs(*(unsigned int*)ptr);
        break;
    case 1:
        *data = *(unsigned char*) ptr;
        break;

    default: std::cout << "ERROR - READ MEMORY"; break;
    }

#ifdef memory_debug
    std::cout << "memory::read() - valor lido: " << std::setfill('0') << std::setw(8) << std::hex << data_ << "\n";
#endif

}

template<int TYPE>
void memory<TYPE>::write(unsigned int data, unsigned int address, int size) {
    unsigned long int ptr;

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address & 3) == 0);
        *(unsigned int*) ptr = (unsigned int)htonl(data);
        break;
    case 2:
        //assert((address & 1) == 0);
        *(unsigned short*) ptr = (unsigned short)htons(data);
        break;
    case 1:
        *(unsigned char*) ptr = (unsigned char)data;
        break;
    default:
        std::cout << "ERROR - WRITE MEMORY";
    }

}

template<int TYPE>
void memory<TYPE>::write_during_elaboration(unsigned int data, unsigned int address, int size) {
    unsigned long int ptr;

    ptr = (unsigned long int) m + (address % mem_size);

    switch(size) {
    case 4:
        //assert((address & 3) == 0);
        *(unsigned int*) ptr = (unsigned int)htonl(data);
        break;
    case 2:
        //assert((address & 1) == 0);
        *(unsigned short*) ptr = (unsigned short)htons(data);
        break;
    case 1:
        *(unsigned char*) ptr = (unsigned char)data;
        break;
    default:
        std::cout << "ERROR - WRITE MEMORY";
    }
}

};

template<int TYPE>
class memory: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::memory<TYPE>,
                                Untimed::memory<TYPE> >::Result {
public:
    typedef  typename Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::memory<TYPE>,
                                Untimed::memory<TYPE> >::Result Base;

    memory(sc_module_name nm
            , unsigned int start_address_
            , unsigned int end_address_
            , unsigned int size_) :Base(nm,start_address_,end_address_,size_){}
};

#endif /* MEMORY_H_ */
