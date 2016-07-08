/*
 * noc_to_catapult.h
 *
 *  Created on: Jan 25, 2012
 *      Author: tiago
 */

#ifndef NOC_TO_CATAPULT_H_
#define NOC_TO_CATAPULT_H_

#include <systemc.h>
#include <iostream>

#define HIGH_LEVEL_SYNTHESIS

#include <system/types_hw.h>

#include <system/resource_table.h>
using Implementation::PHY_Table;
using Implementation::MSG_TYPE_CALL;
using Implementation::MSG_TYPE_CALL_DATA;
using Implementation::MSG_TYPE_ERROR;
using Implementation::MSG_TYPE_RESP;
using Implementation::MSG_TYPE_RESP_DATA;

#include <../unified/framework/ctti.h>
#include <framework/catapult/agent.h>

#include "rtsnoc_to_achannel_rtl.h"
#include <common/debugs.h>


////////////////////////////////

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
SC_MODULE(rtsnoc_to_achannel_db){

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };

    sc_in<bool>     clk;
    sc_in<bool>     rst;

    sc_out<sc_biguint<BUS_SIZE> > din;
    sc_in<sc_biguint<BUS_SIZE> > dout;
    sc_out<bool> wr;
    sc_out<bool> rd;
    sc_in<bool> wait;
    sc_in<bool> nd;

    sc_out< sc_lv<RMI_MSG_SIZE> > rx_ch_z;
    sc_out< sc_logic > rx_ch_vz;
    sc_in< sc_logic > rx_ch_lz;

    sc_in< sc_lv<RMI_MSG_SIZE> > tx_ch_z;
    sc_out< sc_logic > tx_ch_vz;
    sc_in< sc_logic > tx_ch_lz;

public:
    SC_CTOR(rtsnoc_to_achannel_db){
        SC_CTHREAD(rx_process, clk.pos());
        reset_signal_is(rst, true);
        SC_CTHREAD(tx_process, clk.pos());
        reset_signal_is(rst, true);

        last_rx_call_X = 0;
        last_rx_call_Y = 0;
        last_rx_call_local = 0;

        last_tx_call_X = 0;
        last_tx_call_Y = 0;
        last_tx_call_local = 0;
    }

private:
    void rx_process();
    void tx_process();

    typedef struct{
        unsigned int   msg_type;
        unsigned int   instance_id;
        unsigned int   type_id;

        unsigned int   phyaddr_X;
        unsigned int   phyaddr_Y;
        unsigned int   phyaddr_local;
    } Msg_Header_Parsed;

    template<typename DATA_TYPE>
    void parse_header(Msg_Header_Parsed &header, DATA_TYPE &data, bool has_addr){
        header.msg_type = data(39,32).to_uint();
        header.instance_id = data(47,40).to_uint();
        header.type_id = data(55,48).to_uint();
        if(has_addr){
            header.phyaddr_X = data(63,56).to_uint();
            header.phyaddr_Y = data(71,64).to_uint();
            header.phyaddr_local = data(79,72).to_uint();
        }
    }

    unsigned int last_rx_call_X;
    unsigned int last_rx_call_Y;
    unsigned int last_rx_call_local;

    unsigned int last_tx_call_X;
    unsigned int last_tx_call_Y;
    unsigned int last_tx_call_local;
};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel_db<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::tx_process(){
    tx_ch_vz = sc_logic_0;
    wr = false;
    din = 0;
    sc_module::wait();

    while(true){

        while(tx_ch_lz.read() == sc_logic_0) sc_module::wait();
        tx_ch_vz = sc_logic_1;
        sc_lv<RMI_MSG_SIZE> rx_data = tx_ch_z.read();
        sc_module::wait();
        tx_ch_vz = sc_logic_0;

        Msg_Header_Parsed header; parse_header(header, rx_data, true);

        sc_biguint<SIZE_DATA>   tx_data = rx_data;
        sc_biguint<3>           tx_local_orig = LOCAL_ADDR;
        sc_biguint<SIZE_Y>      tx_Y_orig = Y;
        sc_biguint<SIZE_X>      tx_X_orig = X;
        sc_biguint<3>           tx_local_dst = header.phyaddr_local;
        sc_biguint<SIZE_Y>      tx_Y_dst = header.phyaddr_X;
        sc_biguint<SIZE_X>      tx_X_dst = header.phyaddr_Y;


        if(header.msg_type==MSG_TYPE_RESP){
            //FIXME RESP initiation msgs are not really needed
            std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Ignoring msg of type MSG_TYPE_RESP\n";
            sc_module::wait();
            continue;
        }
        else if(header.msg_type==MSG_TYPE_RESP_DATA){
            if((header.phyaddr_local != last_rx_call_local) ||
               (header.phyaddr_Y != last_rx_call_Y) ||
               (header.phyaddr_X != last_rx_call_X)){
                std::cout << "##ERROR NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Dest addr of MSG_TYPE_RESP_DATA is wrong\n";
                std::cout << "\t\t node yielded: Y=" << header.phyaddr_X << ", Y=" << header.phyaddr_Y << ", local=" << header.phyaddr_local << "\n";
                std::cout << "\t\t but it should be: Y=" << last_rx_call_X << ", Y=" << last_rx_call_Y << ", local=" << last_rx_call_local << "\n";
                sc_module::wait();
                continue;
            }
        }
        else if((header.msg_type==MSG_TYPE_CALL) || (header.msg_type==MSG_TYPE_CALL_DATA)){
            int idx = PHY_Table::type2IDX(header.type_id, header.instance_id);
            if(idx >= 0){
                if((header.phyaddr_local != PHY_Table::LOCAL[idx]) ||
                   (header.phyaddr_Y != PHY_Table::Y[idx]) ||
                   (header.phyaddr_X != PHY_Table::X[idx])){
                    std::cout << "##ERROR NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Dest addr of MSG_TYPE_RESP_DATA is wrong\n";
                    std::cout << "\t\t node yielded: Y=" << header.phyaddr_X << ", Y=" << header.phyaddr_Y << ", local=" << header.phyaddr_local << "\n";
                    std::cout << "\t\t but it should be: Y=" << PHY_Table::X[idx] << ", Y=" << PHY_Table::Y[idx] << ", local=" << PHY_Table::LOCAL[idx] << "\n";
                    sc_module::wait();
                    continue;
                }

                last_tx_call_local = header.phyaddr_local;
                last_tx_call_Y = header.phyaddr_Y;
                last_tx_call_X = header.phyaddr_X;
            }
            else{
                std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Target TX addr not found\n";
                sc_module::wait();
                continue;
            }
        }
        else{
            std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Wrong msg - Protocol error\n";
            sc_module::wait();
            continue;
        }

        while(wait.read()) sc_module::wait();
        din = (tx_X_orig,
                tx_Y_orig,
                tx_local_orig,
                tx_X_dst,
                tx_Y_dst,
                tx_local_dst,
                tx_data);
        wr = true;

        sc_module::wait();

        wr = false;

        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - TX: " << tx_data.to_string(SC_HEX) << " to " << "(" << tx_X_dst << "," << tx_Y_dst << "," << tx_local_dst << ")\n";
        std::cout << "\t\t msg_type=" << header.msg_type << ", iid=" << header.instance_id << ", tid=" << header.type_id << "\n";
        std::cout << "\t\t to: Y=" << header.phyaddr_X << ", Y=" << header.phyaddr_Y << ", local=" << header.phyaddr_local << "\n";

        sc_module::wait();
    }
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel_db<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::rx_process(){
    rd = false;
    rx_ch_z = 0;
    rx_ch_vz = sc_logic_0;
    sc_module::wait();

    while(true){

        while(!nd.read()) sc_module::wait();

        sc_biguint<SIZE_DATA>   rx_data;
        sc_biguint<3>           rx_local_dst;
        sc_biguint<SIZE_Y>      rx_Y_dst;
        sc_biguint<SIZE_X>      rx_X_dst;
        sc_biguint<3>           rx_local_orig;
        sc_biguint<SIZE_Y>      rx_Y_orig;
        sc_biguint<SIZE_X>      rx_X_orig;
        (rx_X_orig,
         rx_Y_orig,
         rx_local_orig,
         rx_X_dst,
         rx_Y_dst,
         rx_local_dst,
         rx_data) = dout.read();

        Msg_Header_Parsed header; parse_header(header, rx_data, false);

        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - RX: " << rx_data.to_string(SC_HEX) << "\n";
        std::cout << "\t\t msg_type=" << header.msg_type << ", iid=" << header.instance_id << ", tid=" << header.type_id << "\n";
        std::cout << "\t\t from " << "(" << rx_X_orig << "," << rx_Y_orig << "," << rx_local_orig << ")\n";

        sc_lv<8> local_orig(rx_local_orig);
        sc_lv<8> Y_orig(rx_Y_orig);
        sc_lv<8> X_orig(rx_X_orig);
        sc_lv<SIZE_DATA> data(rx_data);
        sc_lv<RMI_MSG_SIZE> tx_data = (local_orig, Y_orig, X_orig, data);

        rd = true;
        sc_module::wait();
        rd = false;

       if(header.msg_type==MSG_TYPE_RESP){
            //FIXME RESP initiation msgs are not really needed
            std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Ignoring msg of type MSG_TYPE_RESP\n";
            sc_module::wait();
            continue;
       }
       else if(header.msg_type==MSG_TYPE_RESP_DATA){
            bool addr_match = (rx_X_orig.to_uint() == last_tx_call_X) &&
                              (rx_Y_orig.to_uint() == last_tx_call_Y) &&
                              (rx_local_orig.to_uint() == last_tx_call_local);
            if(!addr_match){
                std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - RX resp addrs does not match TX call addr\n";
                sc_module::wait();
                continue;
            }
        }
        else if((header.msg_type==MSG_TYPE_CALL) || (header.msg_type==MSG_TYPE_CALL_DATA)){
            last_rx_call_X = rx_X_orig.to_uint();
            last_rx_call_Y = rx_Y_orig.to_uint();
            last_rx_call_local = rx_local_orig.to_uint();
        }
        else{
            std::cout << "##WARNING NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - Wrong msg - Protocol error\n";
            sc_module::wait();
            continue;
        }

        while(rx_ch_lz.read() == sc_logic_0) sc_module::wait();
        rx_ch_z = tx_data;
        rx_ch_vz = sc_logic_1;
        sc_module::wait();
        rx_ch_vz = sc_logic_0;

        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << ") - RX: " << tx_data.to_string(SC_HEX) << " forwarded\n";

        sc_module::wait();
    }
}


////////////////////////////////

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
SC_MODULE(rtsnoc_to_achannel){

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };

    sc_in<bool>     clk;
    sc_in<bool>     rst;

    sc_out<sc_biguint<BUS_SIZE> > din;
    sc_in<sc_biguint<BUS_SIZE> > dout;
    sc_out<bool> wr;
    sc_out<bool> rd;
    sc_in<bool> wait;
    sc_in<bool> nd;

    sc_out< sc_lv<RMI_MSG_SIZE> > rx_ch_z;
    sc_out< sc_logic > rx_ch_vz;
    sc_in< sc_logic > rx_ch_lz;

    sc_in< sc_lv<RMI_MSG_SIZE> > tx_ch_z;
    sc_out< sc_logic > tx_ch_vz;
    sc_in< sc_logic > tx_ch_lz;

public:
    SC_CTOR(rtsnoc_to_achannel){
        SC_CTHREAD(rx_process, clk.pos());
        reset_signal_is(rst, true);
        SC_CTHREAD(tx_process, clk.pos());
        reset_signal_is(rst, true);
    }

private:
    void rx_process();
    void tx_process();

    typedef struct{
        unsigned int   phyaddr_X;
        unsigned int   phyaddr_Y;
        unsigned int   phyaddr_local;

        unsigned int   msg_type;
        unsigned int   instance_id;
        unsigned int   type_id;

        unsigned int   msg_data;

    } Msg_Header_Parsed;

    template<typename DATA_TYPE>
    void parse_header(Msg_Header_Parsed &header, DATA_TYPE &data){
        header.phyaddr_X = data(63,56).to_uint();
        header.phyaddr_Y = data(71,64).to_uint();
        header.phyaddr_local = data(79,72).to_uint();
#ifdef profile_catapult_nodes
        header.msg_type = data(39,32).to_uint();
        header.instance_id = data(47,40).to_uint();
        header.type_id = data(55,48).to_uint();
        header.msg_data = data(31,0).to_uint();
#endif
    }
};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::tx_process(){
    tx_ch_vz = sc_logic_0;
    wr = false;
    din = 0;
    sc_module::wait();

    while(true){

        while(tx_ch_lz.read() == sc_logic_0) sc_module::wait();
        tx_ch_vz = sc_logic_1;
        sc_lv<RMI_MSG_SIZE> rx_data = tx_ch_z.read();
        sc_module::wait();
        tx_ch_vz = sc_logic_0;

        Msg_Header_Parsed header; parse_header(header, rx_data);

#ifdef profile_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX msg\n";
        std::cout << "\ttype = ";
        switch (header.msg_type) {
            case Implementation::MSG_TYPE_CALL:
                std::cout << "MSG_TYPE_CALL";
                break;
            case Implementation::MSG_TYPE_CALL_DATA:
                std::cout << "MSG_TYPE_CALL_DATA";
                break;
            case Implementation::MSG_TYPE_RESP_DATA:
                std::cout << "MSG_TYPE_RESP_DATA";
                break;
            default:
                break;
        }
        std::cout << ", data = " << (void*)(header.msg_data) << "\n";
        std::cout << "\tTSC = " << Global::get_global_timer_ticks() << "\n";
#endif


        sc_biguint<SIZE_DATA>   tx_data = rx_data;
        sc_biguint<3>           tx_local_orig = LOCAL_ADDR;
        sc_biguint<SIZE_Y>      tx_Y_orig = Y;
        sc_biguint<SIZE_X>      tx_X_orig = X;
        sc_biguint<3>           tx_local_dst = header.phyaddr_local;
        sc_biguint<SIZE_Y>      tx_Y_dst = header.phyaddr_X;
        sc_biguint<SIZE_X>      tx_X_dst = header.phyaddr_Y;

        while(wait.read()) sc_module::wait();
        din = (tx_X_orig,
                tx_Y_orig,
                tx_local_orig,
                tx_X_dst,
                tx_Y_dst,
                tx_local_dst,
                tx_data);
        wr = true;

        sc_module::wait();

        wr = false;
    }
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::rx_process(){
    rd = false;
    rx_ch_z = 0;
    rx_ch_vz = sc_logic_0;
    sc_module::wait();

    while(true){

        while(!nd.read()) sc_module::wait();

        sc_biguint<SIZE_DATA>   rx_data;
        sc_biguint<3>           rx_local_dst;
        sc_biguint<SIZE_Y>      rx_Y_dst;
        sc_biguint<SIZE_X>      rx_X_dst;
        sc_biguint<3>           rx_local_orig;
        sc_biguint<SIZE_Y>      rx_Y_orig;
        sc_biguint<SIZE_X>      rx_X_orig;
        (rx_X_orig,
         rx_Y_orig,
         rx_local_orig,
         rx_X_dst,
         rx_Y_dst,
         rx_local_dst,
         rx_data) = dout.read();


        sc_lv<8> local_orig(rx_local_orig);
        sc_lv<8> Y_orig(rx_Y_orig);
        sc_lv<8> X_orig(rx_X_orig);
        sc_lv<SIZE_DATA> data(rx_data);
        sc_lv<RMI_MSG_SIZE> tx_data = (local_orig, Y_orig, X_orig, data);

        rd = true;
        sc_module::wait();
        rd = false;

#ifdef profile_catapult_nodes
        Msg_Header_Parsed header; parse_header(header, tx_data);
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX msg\n";
        std::cout << "\ttype = ";
        switch (header.msg_type) {
            case Implementation::MSG_TYPE_CALL:
                std::cout << "MSG_TYPE_CALL";
                break;
            case Implementation::MSG_TYPE_CALL_DATA:
                std::cout << "MSG_TYPE_CALL_DATA";
                break;
            case Implementation::MSG_TYPE_RESP_DATA:
                std::cout << "MSG_TYPE_RESP_DATA";
                break;
            default:
                break;
        }
        std::cout << ", data = " << (void*)(header.msg_data) << "\n";
        std::cout << "\tTSC = " << Global::get_global_timer_ticks() << "\n";
#endif

        while(rx_ch_lz.read() == sc_logic_0) sc_module::wait();
        rx_ch_z = tx_data;
        rx_ch_vz = sc_logic_1;
        sc_module::wait();
        rx_ch_vz = sc_logic_0;
    }
}

////////////////////////////////


template<
	class Top_Level,
	unsigned int X_CALL,
	unsigned int Y_CALL,
	unsigned int LOCAL_ADDR_CALL,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y,
	unsigned int RMI_MSG_SIZE,
	unsigned int IID_SIZE,
	unsigned int IID_N_IDS
>
SC_MODULE(adapter_rtsnoc_to_achannel){

	Top_Level toplevel;
#ifndef debug_catapult_nodes
	//rtsnoc_to_achannel_rtl<X_CALL,Y_CALL,LOCAL_ADDR_CALL,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE> convert;
	rtsnoc_to_achannel<X_CALL,Y_CALL,LOCAL_ADDR_CALL,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE> convert;
#else
	rtsnoc_to_achannel_db<X_CALL,Y_CALL,LOCAL_ADDR_CALL,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE> convert;
#endif

	enum{
		BUS_SIZE = rtsnoc_to_achannel<X_CALL,Y_CALL,LOCAL_ADDR_CALL,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::BUS_SIZE
	};

	sc_in<bool>		clk;
	sc_in<bool>		rst;

	sc_out<sc_biguint<BUS_SIZE> > din;
	sc_in<sc_biguint<BUS_SIZE> > dout;
	sc_out<bool> wr;
	sc_out<bool> rd;
	sc_in<bool> wait;
	sc_in<bool> nd;
	sc_in<unsigned int> iid[IID_N_IDS];

	sc_signal<sc_logic> rst_logic;

	sc_signal< sc_lv<RMI_MSG_SIZE> > rx_ch_rsc_z;
	sc_signal< sc_logic > rx_ch_rsc_vz;
	sc_signal< sc_logic > rx_ch_rsc_lz;

	sc_signal< sc_lv<RMI_MSG_SIZE> > tx_ch_rsc_z;
	sc_signal< sc_logic > tx_ch_rsc_vz;
	sc_signal< sc_logic > tx_ch_rsc_lz;

	sc_signal< sc_lv<IID_N_IDS*IID_SIZE> > iid_rsc;

	SC_CTOR(adapter_rtsnoc_to_achannel)
		:toplevel("Top"),
		 convert("Convert"){

		std::cout << "## INFO: Elaborating TIMED node " << this->name() << "<"
				<< "ADDR_X_CALL= " << X_CALL << ","
				<< "ADDR_Y_CALL= " << Y_CALL << ","
				<< "ADDR_LOCAL= " << LOCAL_ADDR_CALL << ","
				<< "SIZE_DATA= " << SIZE_DATA << ","
				<< "SIZE_X= " << SIZE_X << ","
				<< "SIZE_Y= " << SIZE_Y << ","
				<< "BUS_SIZE= " << BUS_SIZE << ","
				<< "RMI_MSG_SIZE= " << RMI_MSG_SIZE << ">\n"
				<< "\t\tnode_size " << sizeof(Top_Level) << "bytes"
				<< std::endl;

		convert.clk(clk);
		convert.rst(rst);
		convert.din(din);
		convert.dout(dout);
		convert.wr(wr);
		convert.rd(rd);
		convert.wait(wait);
		convert.nd(nd);
		convert.rx_ch_z(rx_ch_rsc_z);
		convert.rx_ch_vz(rx_ch_rsc_vz);
		convert.rx_ch_lz(rx_ch_rsc_lz);
		convert.tx_ch_z(tx_ch_rsc_z);
		convert.tx_ch_vz(tx_ch_rsc_vz);
		convert.tx_ch_lz(tx_ch_rsc_lz);

		toplevel.clk(clk);
		toplevel.rst(rst_logic);
		toplevel.rx_ch_rsc_z(rx_ch_rsc_z);
		toplevel.rx_ch_rsc_vz(rx_ch_rsc_vz);
		toplevel.rx_ch_rsc_lz(rx_ch_rsc_lz);
		toplevel.tx_ch_rsc_z(tx_ch_rsc_z);
		toplevel.tx_ch_rsc_vz(tx_ch_rsc_vz);
		toplevel.tx_ch_rsc_lz(tx_ch_rsc_lz);
		toplevel.iid(iid_rsc);

		SC_METHOD(rst_convert); sensitive << rst;

		SC_METHOD(iid_convert); for (unsigned int i = 0; i < IID_N_IDS; ++i) sensitive << iid[i];
	}

	void rst_convert(){
		rst_logic = rst.read() ? sc_logic_1 : sc_logic_0;
	}

	void iid_convert(){
		sc_lv<IID_N_IDS*IID_SIZE> tmp;
		for (unsigned int i = 0; i < IID_N_IDS; ++i) {
			tmp.range(((i+1)*IID_SIZE)-1,(i*IID_SIZE)) = iid[i];
		}
		iid_rsc.write(tmp);
	}

};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int IID_N_IDS,
    class Adapted
>
class adapter_rtsnoc_tlm_to_rtl : public sc_module{
public:
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6
    };
    sc_in<bool>     rst;
    sc_in<unsigned int> iid[IID_N_IDS];
    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;

public:
    rtsnoc_tlm_to_rtl<X, Y, LOCAL_ADDR, SIZE_DATA, SIZE_X, SIZE_Y> wrapper;
    Adapted adapted;

    sc_signal<sc_biguint<BUS_SIZE> > din_s;
    sc_signal<sc_biguint<BUS_SIZE> > dout_s;
    sc_signal<bool> wr;
    sc_signal<bool> rd;
    sc_signal<bool> wait;
    sc_signal<bool> nd;

public:
    SC_HAS_PROCESS(adapter_rtsnoc_tlm_to_rtl);
    adapter_rtsnoc_tlm_to_rtl(sc_module_name nm) :sc_module(nm),
        wrapper("rtsnoc_tlm_to_rtl"),
        adapted("adapted")

    {
        wrapper.rst(rst);
        wrapper.din(din_s);
        wrapper.dout(dout_s);
        wrapper.wr(wr);
        wrapper.rd(rd);
        wrapper.wait(wait);
        wrapper.nd(nd);
        wrapper.fifo_din(dout);
        wrapper.fifo_dout(din);

        adapted.rst(rst);
        for (unsigned int i = 0; i < IID_N_IDS; ++i) {
            adapted.iid[i](iid[i]);
        }
        adapted.clk(wrapper.clk);
        adapted.din(din_s);
        adapted.dout(dout_s);
        adapted.wr(wr);
        adapted.rd(rd);
        adapted.wait(wait);
        adapted.nd(nd);
    }


};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int IID_N_IDS,
    class Adapted
>
class adapter_rtsnoc_tlm_to_achannel_functional : public sc_module{
public:
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };
    sc_in<bool>     rst;
    sc_in<unsigned int> iid[IID_N_IDS];
    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;
private:
    Implementation::Channel_t rx_channel;
    Implementation::Channel_t tx_channel;

    unsigned char _iid[Implementation::Traits<Adapted>::n_ids];

    typename Implementation::IF<Implementation::Traits<Adapted>::hardware,\
                       Implementation::Agent<Adapted>,\
                       Implementation::Agent_Dummy<Adapted> >::Result
                  adapted;

    void set_msg_ac(Catapult::RMI_Msg &msg, sc_biguint<SIZE_DATA> &data){
        msg.phy_data.payload = data(31,0).to_uint();
        msg.phy_data.header.msg_type = data(39,32).to_uint();
        msg.phy_data.header.instance_id = data(47,40).to_uint();
        msg.phy_data.header.type_id = data(55,48).to_uint();
    }

    void set_data_ac(Catapult::RMI_Msg &msg, sc_biguint<SIZE_DATA> &data){
        data = 0;
        data(31,0) = msg.phy_data.payload.to_uint();
        data(39,32) = msg.phy_data.header.msg_type.to_uint();
        data(47,40) = msg.phy_data.header.instance_id.to_uint();
        data(55,48) = msg.phy_data.header.type_id.to_uint();
    }

public:
    SC_HAS_PROCESS(adapter_rtsnoc_tlm_to_achannel_functional);
    adapter_rtsnoc_tlm_to_achannel_functional(sc_module_name nm) :sc_module(nm),
        rx_channel(this),
        tx_channel(this),
        adapted(rx_channel,tx_channel,_iid)

    {
        std::cout << "## INFO: Elaborating UNTIMED node " << this->name() << "<"
                << "ADDR_X_CALL= " << X << ","
                << "ADDR_Y_CALL= " << Y << ","
                << "ADDR_LOCAL= " << LOCAL_ADDR << ","
                << "SIZE_DATA= " << SIZE_DATA << ","
                << "SIZE_X= " << SIZE_X << ","
                << "SIZE_Y= " << SIZE_Y << ","
                << "BUS_SIZE= " << BUS_SIZE << "\n"
                << "\t\tnode_size " << sizeof(adapted) << "bytes"
                << std::endl;

        SC_THREAD(handle_rx);
        SC_THREAD(handle_tx);
        SC_THREAD(handle_agent);
    }

    void handle_agent(){
        if(rst.read()) wait(rst.negedge_event());

        while(true){
            adapted.top_level();
        }
    }

    void handle_rx(){

        if(rst.read()) wait(rst.negedge_event());

        while(true){

            while(dout.num_available() == 0) wait(dout.data_written_event());

            sc_biguint<SIZE_DATA>   rx_data;
            sc_biguint<3>           rx_local_dst;
            sc_biguint<SIZE_Y>      rx_Y_dst;
            sc_biguint<SIZE_X>      rx_X_dst;
            sc_biguint<3>           rx_local_orig;
            sc_biguint<SIZE_Y>      rx_Y_orig;
            sc_biguint<SIZE_X>      rx_X_orig;
            (rx_X_orig,
                    rx_Y_orig,
                    rx_local_orig,
                    rx_X_dst,
                    rx_Y_dst,
                    rx_local_dst,
                    rx_data) = dout.read();

            Catapult::RMI_Msg tx_data;
            set_msg_ac(tx_data,rx_data);
            tx_data.phy_addr.X = rx_X_orig.to_uint();
            tx_data.phy_addr.Y = rx_Y_orig.to_uint();
            tx_data.phy_addr.local = rx_local_orig.to_uint();

            rx_channel.write(tx_data);

        }
    }

    void handle_tx(){

        if(rst.read()) wait(rst.negedge_event());

        while(true){

        Catapult::RMI_Msg tx_msg;
        tx_channel.read(tx_msg);


        sc_biguint<SIZE_DATA>   tx_data;
        set_data_ac(tx_msg, tx_data);
        sc_biguint<3>           tx_local_orig = LOCAL_ADDR;
        sc_biguint<SIZE_Y>      tx_Y_orig = Y;
        sc_biguint<SIZE_X>      tx_X_orig = X;
        sc_biguint<3>           tx_local_dst = tx_msg.phy_addr.local.to_uint();
        sc_biguint<SIZE_Y>      tx_Y_dst = tx_msg.phy_addr.X.to_uint();
        sc_biguint<SIZE_X>      tx_X_dst = tx_msg.phy_addr.Y.to_uint();

        sc_biguint<BUS_SIZE> data = (tx_X_orig,
                                         tx_Y_orig,
                                         tx_local_orig,
                                         tx_X_dst,
                                         tx_Y_dst,
                                         tx_local_dst,
                                         tx_data);


         din.write(data);
        }
    }


};


#define DECLARE_NODE_TIMED(name,ns)                                           \
namespace Timed { \
template<                                                                               \
    unsigned int X,                                                             \
    unsigned int Y,                                                             \
    unsigned int LOCAL_ADDR,                                                        \
    unsigned int SIZE_DATA,                                                             \
    unsigned int SIZE_X,                                                                \
    unsigned int SIZE_Y,                                                                \
    unsigned int RMI_MSG_SIZE,                                                           \
    unsigned int IID_SIZE,                                                              \
    unsigned int IID_N_IDS                                                              \
>                                                                                       \
class HW_NODE(name): public adapter_rtsnoc_tlm_to_rtl<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,IID_N_IDS, \
                                      adapter_rtsnoc_to_achannel<ns::HW_NODE(name),X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS> > {                  \
public:                                                                                 \
    typedef adapter_rtsnoc_to_achannel<ns::HW_NODE(name),X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS> Adapted;                          \
    typedef adapter_rtsnoc_tlm_to_rtl<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y, IID_N_IDS, Adapted> Base;                          \
\
    HW_NODE(name)(sc_module_name nm) :Base(nm){}                                            \
}; \
}

#define DECLARE_NODE_UNTIMED(name)\
namespace Untimed { \
template<\
    unsigned int X,\
    unsigned int Y,\
    unsigned int LOCAL_ADDR,\
    unsigned int SIZE_DATA,\
    unsigned int SIZE_X,\
    unsigned int SIZE_Y,\
    unsigned int RMI_MSG_SIZE,\
    unsigned int IID_SIZE,\
    unsigned int IID_N_IDS\
>\
class HW_NODE(name): public adapter_rtsnoc_tlm_to_achannel_functional<\
                                        X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,IID_N_IDS,\
                                        Implementation::name> {\
public:\
    typedef  adapter_rtsnoc_tlm_to_achannel_functional<\
                                        X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,IID_N_IDS,\
                                        Implementation::name> Base;\
\
    HW_NODE(name)(sc_module_name nm) :Base(nm){}\
}; \
}

#define DECLARE_NODE(name)\
template<\
    unsigned int X,\
    unsigned int Y,\
    unsigned int LOCAL_ADDR,\
    unsigned int SIZE_DATA,\
    unsigned int SIZE_X,\
    unsigned int SIZE_Y,\
    unsigned int RMI_MSG_SIZE,\
    unsigned int IID_SIZE,\
    unsigned int IID_N_IDS\
>\
class HW_NODE(name): public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::HW_NODE(name)<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS>, \
                                Untimed::HW_NODE(name)<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS> >::Result { \
public:\
    typedef  typename Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::HW_NODE(name)<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS>, \
                                Untimed::HW_NODE(name)<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE,IID_SIZE,IID_N_IDS> >::Result Base;\
\
    HW_NODE(name)(sc_module_name nm) :Base(nm){}\
}


//

#endif /* NOC_TO_CATAPULT_H_ */
