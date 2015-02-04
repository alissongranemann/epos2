#ifndef CATAPULT__H_
#define CATAPULT__H_

#include <ac_int.h>
#include <ac_channel.h>

namespace Catapult {

typedef ac_int<8,false> RMI_Msg_Header_Elem;

typedef struct {
    RMI_Msg_Header_Elem X;
    RMI_Msg_Header_Elem Y;
    RMI_Msg_Header_Elem local;
} RMI_Msg_PHY_Addr_Info;

typedef struct{
    RMI_Msg_Header_Elem   msg_type;
    RMI_Msg_Header_Elem   instance_id;
    RMI_Msg_Header_Elem   type_id;
} RMI_Msg_Data_Header;


//TODO size should be Traits<System>::pkt_size
typedef ac_int<32,false> RMI_Msg_Data_Payload;

typedef struct {
    RMI_Msg_Data_Payload   payload;
    RMI_Msg_Data_Header    header;
} RMI_Msg_PHY_Data;

typedef struct {
    RMI_Msg_PHY_Data      phy_data;
    RMI_Msg_PHY_Addr_Info phy_addr;//src addr for received msgs; dest addr for transmitted
} RMI_Msg;


//ATTENTION: the msg's members order above will yield the following pkt in the channel:
//  56         . . .            32 31    0
//  h_type_id h_inst_id h_msg_type payload
#ifdef SYSTEMC_SIM
class Channel_t : private ac_channel<RMI_Msg> {
public:
    sc_module *parent;
    sc_event write_event;

    Channel_t(sc_module *_parent):parent(_parent){

    }


    void read(RMI_Msg &msg){
        while(!ac_channel<RMI_Msg>::available(1)) sc_core::wait(write_event, parent->simcontext());

        ac_channel<RMI_Msg>::read(msg);
    }

    void write(RMI_Msg &msg){
        ac_channel<RMI_Msg>::write(msg);
        write_event.notify();
    }
};
#else
typedef ac_channel<RMI_Msg> Channel_t;
#endif

class Msg_Parser {
protected:
    RMI_Msg msg;

protected:
    unsigned int msg_payload_get_op_id(){
        return msg.phy_data.payload.to_uint();
    }
    void msg_payload_set_op_id(unsigned int val){
        ac_int<32,false> tmp(val);
        msg.phy_data.payload.set_slc(0,tmp);
    }

   unsigned int msg_payload_get_data(){
        return msg.phy_data.payload.to_uint();
    }
    void msg_payload_set_data(unsigned int val){
        ac_int<32,false> tmp(val);
        msg.phy_data.payload.set_slc(0,tmp);
    }
};

};


#endif /* MSG_PARSER_H_ */
