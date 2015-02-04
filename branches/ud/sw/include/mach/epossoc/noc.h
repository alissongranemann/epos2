// EPOS EPOSSOC NoC Mediator

#ifndef __epossoc_noc_h
#define __epossoc_noc_h

#include <machine.h>
#include <noc.h>
#include <utility/channel.h>

__BEGIN_SYS

class EPOSSOC_NOC: public NOC_Common
{
public:
    typedef Implementation::Address Address;

    typedef NOC_Common::Info Info;

    typedef struct {
        unsigned int data[Traits<EPOSSOC_NOC>::PROC_NOC_BUS_RATE];
    } Packet;


private:
    EPOSSOC_NOC();

    ~EPOSSOC_NOC(){ }

    static EPOSSOC_NOC *_instantance;

public:
    static EPOSSOC_NOC& get_instance(){
        if(_instantance == 0)
            _instantance = new (SYSTEM) EPOSSOC_NOC();
        return *_instantance;
    }

public:

    void send_header(Address const* address);
    void send(Address const* address, Packet const* data);
    void send(Packet const* data);

    bool receive_available() { return nd();}
    void receive_header(Address* address);
    void receive(Address* address, Packet* data);
    void receive(Packet* data);

    void receive_int(IC::Interrupt_Handler h);

    //inline void lock(){ disable_receive_int(); _lock_sem.p(); }
    //inline void unlock(){ _lock_sem.v(); enable_receive_int(); }

private:
    inline void disable_receive_int(){ IC::disable(IC::IRQ_NOC); }
    inline void enable_receive_int(){ IC::enable(IC::IRQ_NOC); }

//private:
//    Busy_Semaphore<Traits<EPOSSOC_NOC>::CONCURRENCY_LIMIT>   _lock_sem;

private: //HW registers
    enum{
        NOC_BASE = Traits<EPOSSOC_NOC>::BASE_ADDRESS,
    };

    enum{
        REG_LOCAL_ADDR = 0,
        REG_ROUTER_X_ADDR,
        REG_ROUTER_Y_ADDR,
        REG_NET_X_SIZE,
        REG_NET_Y_SIZE,
        REG_NET_DATA_WIDTH,
        REG_STATUS,
        REG_HEADER_DST_LOCAL_ADDR,
        REG_HEADER_DST_ROUTER_Y_ADDR,
        REG_HEADER_DST_ROUTER_X_ADDR,
        REG_HEADER_SRC_LOCAL_ADDR,
        REG_HEADER_SRC_ROUTER_Y_ADDR,
        REG_HEADER_SRC_ROUTER_X_ADDR,
        REG_DATA_BASE
    };


    static inline unsigned int reg(unsigned int offset){
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
        return aux[offset];
    }

    static inline void reg(unsigned int offset, unsigned int val){
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
        aux[offset] = val;
    }

    static inline unsigned int header_src_router_x_addr(){return reg(REG_HEADER_SRC_ROUTER_X_ADDR);}
    static inline unsigned int header_src_router_y_addr(){return reg(REG_HEADER_SRC_ROUTER_Y_ADDR);}
    static inline unsigned int header_src_local_addr(){return reg(REG_HEADER_SRC_LOCAL_ADDR);}
    static inline unsigned int header_dst_router_x_addr(){return reg(REG_HEADER_DST_ROUTER_X_ADDR);}
    static inline unsigned int header_dst_router_y_addr(){return reg(REG_HEADER_DST_ROUTER_Y_ADDR);}
    static inline unsigned int header_dst_local_addr(){return reg(REG_HEADER_DST_LOCAL_ADDR);}

    static inline void header_src_router_x_addr(unsigned int val){reg(REG_HEADER_SRC_ROUTER_X_ADDR, val);}
    static inline void header_src_router_y_addr(unsigned int val){reg(REG_HEADER_SRC_ROUTER_Y_ADDR, val);}
    static inline void header_src_local_addr(unsigned int val){reg(REG_HEADER_SRC_LOCAL_ADDR, val);}
    static inline void header_dst_router_x_addr(unsigned int val){reg(REG_HEADER_DST_ROUTER_X_ADDR, val);}
    static inline void header_dst_router_y_addr(unsigned int val){reg(REG_HEADER_DST_ROUTER_Y_ADDR, val);}
    static inline void header_dst_local_addr(unsigned int val){reg(REG_HEADER_DST_LOCAL_ADDR, val);}

    static inline unsigned int data(unsigned int offset){ return reg(REG_DATA_BASE+offset);}
    static inline void data(unsigned int offset, unsigned int val){ reg(REG_DATA_BASE+offset, val);}

    static inline void wr(){ reg(REG_STATUS, 0x1); }
    static inline void rd(){ reg(REG_STATUS, 0x2); }
    static inline bool wait(){return reg(REG_STATUS) & 0x4; }
    static inline bool nd(){return reg(REG_STATUS) & 0x8; }

    static inline unsigned int local_addr(){return reg(REG_LOCAL_ADDR);}
    static inline unsigned int router_x_addr(){return reg(REG_ROUTER_X_ADDR);}
    static inline unsigned int router_y_addr(){return reg(REG_ROUTER_Y_ADDR);}
    static inline unsigned int net_x_size(){return reg(REG_NET_X_SIZE);}
    static inline unsigned int net_y_size(){return reg(REG_NET_Y_SIZE);}
    static inline unsigned int net_data_width(){return reg(REG_NET_DATA_WIDTH);}


};

__END_SYS

#endif
