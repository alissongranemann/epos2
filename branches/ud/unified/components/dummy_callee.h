// EPOS Add Abstraction Declarations

#ifndef __dummy_callee_unified_h
#define __dummy_callee_unified_h

#include "component.h"
#ifndef HIGH_LEVEL_SYNTHESIS
#include <utility/ostream.h>
#include <timer.h>
#endif


namespace Implementation {

class Dummy_Callee : public Component{

public:
    enum {
        OP_FUNC_0_0 = 0,
        OP_FUNC_1_0,
        OP_FUNC_0_1,
        OP_FUNC_0_1_ACC,
        OP_FUNC_1_1,
        OP_FUNC_1_1_ACC,
        OP_FUNC_1_PKT_1,
        OP_FUNC_1_PKT_1_ACC,
        OP_FUNC_1_2,
        OP_FUNC_1_2_ACC,
        OP_FUNC_1_4,
        OP_FUNC_1_4_ACC,
        OP_FUNC_1_8,
        OP_FUNC_1_8_ACC,

        OP_RSP_AES_DECIPHER,
        OP_RSP_ADPCM_DECODE,
        OP_RSP_DTMF_ADD_SAMPLE,
        OP_RSP_CONTROLL_TONE
    };

private:
    unsigned int _func_dummy_acc;
    unsigned int _func_0_1_acc;
    unsigned int _func_1_1_acc;
    unsigned int _func_1_2_acc;
    unsigned int _func_1_4_acc;
    unsigned int _func_1_8_acc;
    unsigned int _func_1_pkt_1_acc;
#ifndef HIGH_LEVEL_SYNTHESIS
        volatile unsigned int * TIMER_REG;
        System::OStream cout;
#endif

    inline void _func_acc(unsigned int &acc, unsigned int &arg){
#ifndef HIGH_LEVEL_SYNTHESIS
        //timer_read = 8 cycles
        //add/sub = about 3 cycles
        unsigned int curr = *TIMER_REG;
        acc += (curr - arg) - 17;//subtract the overhead 8+3+3+3
#endif
    }

public:

    Dummy_Callee(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<Dummy_Callee>::n_ids])
        :Component(rx_ch, tx_ch, iid[0])
         , _func_dummy_acc(0)
         , _func_0_1_acc(0)
         , _func_1_1_acc(0)
         , _func_1_2_acc(0)
         , _func_1_4_acc(0)
         , _func_1_8_acc(0)
         , _func_1_pkt_1_acc(0)
#ifndef HIGH_LEVEL_SYNTHESIS
         , TIMER_REG(reinterpret_cast<volatile unsigned int *>(0x80000800))
#endif
         {}

    void func_0_0(){
        ++_func_dummy_acc;
    }

    unsigned int func_1_0(){
        return _func_dummy_acc;
    }


    void func_0_1(unsigned int i1){
        _func_acc(_func_0_1_acc,i1);
    }
    unsigned int func_0_1_acc(){
        return _func_0_1_acc;
    }

    unsigned int func_1_1(unsigned int i1){
        _func_acc(_func_1_1_acc,i1);
        return i1;
    }
    unsigned int func_1_1_acc(){
        return _func_1_1_acc;
    }

    unsigned int func_1_pkt_1(safe_pkt_t pkt, unsigned int i1){
        _func_acc(_func_1_pkt_1_acc,i1);
        return i1;
    }
    unsigned int func_1_pkt_1_acc(){
        return _func_1_pkt_1_acc;
    }


    unsigned int func_1_2(unsigned int i1, unsigned int i2){
        _func_acc(_func_1_2_acc,i2);
        return i2;
    }
    unsigned int func_1_2_acc(){
        return _func_1_2_acc;
    }

    unsigned int func_1_4(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4){
        _func_acc(_func_1_4_acc,i4);
        return i4;
    }
    unsigned int func_1_4_acc(){
        return _func_1_4_acc;
    }

    unsigned int func_1_8(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4,
                          unsigned int i5, unsigned int i6, unsigned int i7, unsigned int i8){
        _func_acc(_func_1_8_acc,i8);
        return i8;
    }
    unsigned int func_1_8_acc(){
        return _func_1_8_acc;
    }

    ////////////////////////////////////
    // RSP comp tests

    int rsp_aes_decipher(safe_pkt_t pkt){
        return (int)(pkt.data[0]+pkt.data[15]);
    }

    int rsp_adpcm_decode(safe_pkt_t pkt){
        return (int)(pkt.data[0]+pkt.data[15]);
    }

    int rsp_dtmf_add_sample(decoded_pkt_t pkt){
        return (int)(pkt.data[0]+pkt.data[31]);
    }

    int rsp_controller_tone_detected(unsigned char tone){
       return (int)tone;
    }

};

PROXY_BEGIN(Dummy_Callee)
    void func_0_0(){
        Base::call<Dummy_Callee::OP_FUNC_0_0>();
    }
    unsigned int func_1_0(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_0,unsigned int>();
    }
    void func_0_1(unsigned int i1){
        Base::call<Dummy_Callee::OP_FUNC_0_1>(i1);
    }
    unsigned int func_0_1_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_0_1_ACC,unsigned int>();
    }

    unsigned int func_1_1(unsigned int i1){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_1,unsigned int>(i1);
    }
    unsigned int func_1_1_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_1_ACC,unsigned int>();
    }
    unsigned int func_1_pkt_1(safe_pkt_t pkt, unsigned int i1){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_PKT_1,unsigned int>(pkt,i1);
    }
    unsigned int func_1_pkt_1_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_PKT_1_ACC,unsigned int>();
    }
    unsigned int func_1_2(unsigned int i1, unsigned int i2){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_2,unsigned int>(i1,i2);
    }
    unsigned int func_1_2_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_2_ACC,unsigned int>();
    }
    unsigned int func_1_4(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_4,unsigned int>(i1,i2,i3,i4);
    }
    unsigned int func_1_4_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_4_ACC,unsigned int>();
    }
    unsigned int func_1_8(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4,
                          unsigned int i5, unsigned int i6, unsigned int i7, unsigned int i8){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_8,unsigned int>(i1,i2,i3,i4,i5,i6,i7,i8);
    }
    unsigned int func_1_8_acc(){
        return Base::call_r<Dummy_Callee::OP_FUNC_1_8_ACC,unsigned int>();
    }


    int rsp_aes_decipher(safe_pkt_t pkt){
        return Base::call_r<Dummy_Callee::OP_RSP_AES_DECIPHER,int>(pkt);
    }
    int rsp_adpcm_decode(safe_pkt_t pkt){
        return Base::call_r<Dummy_Callee::OP_RSP_ADPCM_DECODE,int>(pkt);
    }
    int rsp_dtmf_add_sample(decoded_pkt_t pkt){
        return Base::call_r<Dummy_Callee::OP_RSP_DTMF_ADD_SAMPLE,int>(pkt);
    }
    int rsp_controller_tone_detected(unsigned char tone){
       return Base::call_r<Dummy_Callee::OP_RSP_CONTROLL_TONE,int>(tone);
    }
PROXY_END

AGENT_BEGIN(Dummy_Callee)
    D_CALL_0(func_0_0, OP_FUNC_0_0);

    D_CALL_R_0(func_1_0, OP_FUNC_1_0,unsigned int);

    D_CALL_1(func_0_1, OP_FUNC_0_1,unsigned int);
    D_CALL_R_0(func_0_1_acc, OP_FUNC_0_1_ACC,unsigned int);

    D_CALL_R_1(func_1_1, OP_FUNC_1_1, unsigned int,unsigned int);
    D_CALL_R_0(func_1_1_acc, OP_FUNC_1_1_ACC, unsigned int);

    D_CALL_R_2(func_1_pkt_1, OP_FUNC_1_PKT_1, unsigned int,safe_pkt_t,unsigned int);
    D_CALL_R_0(func_1_pkt_1_acc, OP_FUNC_1_PKT_1_ACC, unsigned int);

    D_CALL_R_2(func_1_2, OP_FUNC_1_2, unsigned int,unsigned int,unsigned int);
    D_CALL_R_0(func_1_2_acc, OP_FUNC_1_2_ACC, unsigned int);

    D_CALL_R_4(func_1_4, OP_FUNC_1_4, unsigned int,unsigned int,unsigned int,unsigned int,unsigned int);
    D_CALL_R_0(func_1_4_acc, OP_FUNC_1_4_ACC, unsigned int);

    D_CALL_R_8(func_1_8, OP_FUNC_1_8, unsigned int,unsigned int,unsigned int,unsigned int,unsigned int,
                                                   unsigned int,unsigned int,unsigned int,unsigned int);
    D_CALL_R_0(func_1_8_acc, OP_FUNC_1_8_ACC, unsigned int);

    D_CALL_R_1(rsp_aes_decipher, OP_RSP_AES_DECIPHER,int,safe_pkt_t);
    D_CALL_R_1(rsp_adpcm_decode, OP_RSP_ADPCM_DECODE,int,safe_pkt_t);
    D_CALL_R_1(rsp_dtmf_add_sample, OP_RSP_DTMF_ADD_SAMPLE,int,decoded_pkt_t);
    D_CALL_R_1(rsp_controller_tone_detected, OP_RSP_CONTROLL_TONE,int,unsigned char);
AGENT_END

};

DECLARE_COMPONENT(Dummy_Callee);

#endif
