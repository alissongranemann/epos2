// EPOS Add Abstraction Declarations

#ifndef __dummy_caller_unified_h
#define __dummy_caller_unified_h

#include "component.h"

#include "dummy_callee.h"

#include "aes.h"
#include "adpcm_decoder.h"
#include "dtmf_algorithm.h"

namespace Implementation {

class Dummy_Caller : public Component{

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
        OP_RSP_CONTROLL_TONE,
        OP_RSP_TEM_TUDO,
        OP_RSP_TEM_TUDO_FAZ_TUDO,
    };

private:
    typedef System::Dummy_Callee Callee;
    Callee callee;


    enum{
        __dtmf_FRAMESIZE = 32*22,
        __dtmf_TONES = DTMF_Algorithm::TONES
    };
    typedef DTMF_Algorithm::sample_t sample_t;
    AES __aes;
    ADPCM_Decoder __adpcm;
    unsigned short __dtmf_buffer_idx;
    sample_t __dtmf_samples[__dtmf_FRAMESIZE];
    bool __dtmf_tone_responses[__dtmf_TONES];

public:

    Dummy_Caller(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<Dummy_Caller>::n_ids])
        :Component(rx_ch, tx_ch, iid[0]),
         callee(rx_ch, tx_ch, &iid[1]),
         __dtmf_buffer_idx(0){

        for (int i = 0; i < __dtmf_FRAMESIZE; ++i) __dtmf_samples[i] = 0;
        for (int i = 0; i < __dtmf_TONES; ++i) __dtmf_tone_responses[i] = false;
    }

     void func_0_0(){
        callee.func_0_0();
    }

    unsigned int func_1_0(){
        return callee.func_1_0();
    }


    void func_0_1(unsigned int i1){
        callee.func_0_1(i1);
    }
    unsigned int func_0_1_acc(){
        return callee.func_0_1_acc();
    }

    unsigned int func_1_1(unsigned int i1){
        return callee.func_1_1(i1);
    }
    unsigned int func_1_1_acc(){
        return callee.func_1_1_acc();
    }

    unsigned int func_1_pkt_1(safe_pkt_t pkt, unsigned int i1){
        return callee.func_1_pkt_1(pkt,i1);
    }
    unsigned int func_1_pkt_1_acc(){
        return callee.func_1_pkt_1_acc();
    }

    unsigned int func_1_2(unsigned int i1, unsigned int i2){
        return callee.func_1_2(i1,i2);
    }
    unsigned int func_1_2_acc(){
        return callee.func_1_2_acc();
    }

    unsigned int func_1_4(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4){
        return callee.func_1_4(i1,i2,i3,i4);
    }
    unsigned int func_1_4_acc(){
        return callee.func_1_4_acc();
    }

    unsigned int func_1_8(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4,
                          unsigned int i5, unsigned int i6, unsigned int i7, unsigned int i8){
        return callee.func_1_8(i1,i2,i3,i4,i5,i6,i7,i8);
    }
    unsigned int func_1_8_acc(){
        return callee.func_1_8_acc();
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

    int rsp_tem_tudo(safe_pkt_t pkt,decoded_pkt_t dec_pkt,unsigned char tone,unsigned int select){
       switch (select) {
        case OP_RSP_AES_DECIPHER:    return callee.rsp_aes_decipher(pkt);
        case OP_RSP_ADPCM_DECODE:    return callee.rsp_adpcm_decode(pkt);
        case OP_RSP_DTMF_ADD_SAMPLE: return callee.rsp_dtmf_add_sample(dec_pkt);
        case OP_RSP_CONTROLL_TONE:   return callee.rsp_controller_tone_detected(tone);
        case 0xFF: return (int)select; //only measures switch overhead
        default: return 0;
       }
    }

    /////////////////////////////////////////////////////////////////////////

    int rsp_tem_tudo_faz_tudo(safe_pkt_t pkt,decoded_pkt_t dec_pkt,unsigned char tone_arg,unsigned int select){
       switch (select) {
        case OP_RSP_AES_DECIPHER:{
            __aes.add_key(AES::DEFAULT_CIPHER_KEY);
            __aes.cipher_block(pkt.data, pkt.data);
            return (int)(pkt.data[0]+pkt.data[15]);
        }
        case OP_RSP_ADPCM_DECODE:{
            #pragma hls_unroll yes
            adpcm_unfold: for (int i = 0; i < 16; ++i) {
                unsigned int s1 = pkt.data[i] & 0xF;
                unsigned int s2 = pkt.data[i] >> 4;

                dec_pkt.data[i*2] = __adpcm.decode(s1);
                dec_pkt.data[(i*2)+1] = __adpcm.decode(s2);
            }
            return (int)(dec_pkt.data[0]+dec_pkt.data[31]);
        }
        case OP_RSP_DTMF_ADD_SAMPLE:{
             for (int i = 0; i < 32; ++__dtmf_buffer_idx, ++i) {
                 __dtmf_samples[__dtmf_buffer_idx] = dec_pkt.data[i];
             }
             if(__dtmf_buffer_idx == __dtmf_FRAMESIZE){

                 GOERTZEL: for(int i = 0; i < __dtmf_TONES; ++i){
                     __dtmf_tone_responses[i] = DTMF_Algorithm::goertzel(i,__dtmf_samples);
                 }

                 unsigned char detected_signal = DTMF_Algorithm::analyze_responses(__dtmf_tone_responses);

                 // convert from [0, 15] to [1,2,3,..,A,B,..,F] using a lookup table
                 // 'I' for invalid signal
                 unsigned char tone = 'I';
                 if(detected_signal != DTMF_Algorithm::INVAL) {
                     tone = DTMF_Algorithm::button_names[detected_signal];
                 }

                 __dtmf_buffer_idx = 0;

                 return tone;
             }
             else{
                 return 0;
             }
        }
        default: return 0;
       }
    }
};

PROXY_BEGIN(Dummy_Caller)
    void func_0_0(){
        Base::call<Dummy_Caller::OP_FUNC_0_0>();
    }
    unsigned int func_1_0(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_0,unsigned int>();
    }
    void func_0_1(unsigned int i1){
        Base::call<Dummy_Caller::OP_FUNC_0_1>(i1);
    }
    unsigned int func_0_1_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_0_1_ACC,unsigned int>();
    }

    unsigned int func_1_1(unsigned int i1){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_1,unsigned int>(i1);
    }
    unsigned int func_1_1_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_1_ACC,unsigned int>();
    }
    unsigned int func_1_pkt_1(safe_pkt_t pkt, unsigned int i1){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_PKT_1,unsigned int>(pkt,i1);
    }
    unsigned int func_1_pkt_1_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_PKT_1_ACC,unsigned int>();
    }
    unsigned int func_1_2(unsigned int i1, unsigned int i2){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_2,unsigned int>(i1,i2);
    }
    unsigned int func_1_2_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_2_ACC,unsigned int>();
    }
    unsigned int func_1_4(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_4,unsigned int>(i1,i2,i3,i4);
    }
    unsigned int func_1_4_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_4_ACC,unsigned int>();
    }
    unsigned int func_1_8(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4,
                          unsigned int i5, unsigned int i6, unsigned int i7, unsigned int i8){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_8,unsigned int>(i1,i2,i3,i4,i5,i6,i7,i8);
    }
    unsigned int func_1_8_acc(){
        return Base::call_r<Dummy_Caller::OP_FUNC_1_8_ACC,unsigned int>();
    }


    int rsp_aes_decipher(safe_pkt_t pkt){
        return Base::call_r<Dummy_Caller::OP_RSP_AES_DECIPHER,int>(pkt);
    }
    int rsp_adpcm_decode(safe_pkt_t pkt){
        return Base::call_r<Dummy_Caller::OP_RSP_ADPCM_DECODE,int>(pkt);
    }
    int rsp_dtmf_add_sample(decoded_pkt_t pkt){
        return Base::call_r<Dummy_Caller::OP_RSP_DTMF_ADD_SAMPLE,int>(pkt);
    }
    int rsp_controller_tone_detected(unsigned char tone){
       return Base::call_r<Dummy_Caller::OP_RSP_CONTROLL_TONE,int>(tone);
    }
    int rsp_tem_tudo(safe_pkt_t pkt,decoded_pkt_t dec_pkt,unsigned char tone,unsigned int select){
       return Base::call_r<Dummy_Caller::OP_RSP_TEM_TUDO,int>(pkt,dec_pkt,tone,select);
    }
    int rsp_tem_tudo_faz_tudo(safe_pkt_t pkt,decoded_pkt_t dec_pkt,unsigned char tone,unsigned int select){
       return Base::call_r<Dummy_Caller::OP_RSP_TEM_TUDO_FAZ_TUDO,int>(pkt,dec_pkt,tone,select);
    }
PROXY_END

AGENT_BEGIN(Dummy_Caller)
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
    D_CALL_R_4(rsp_tem_tudo, OP_RSP_TEM_TUDO,int,safe_pkt_t,decoded_pkt_t,unsigned char,unsigned int);
    D_CALL_R_4(rsp_tem_tudo_faz_tudo, OP_RSP_TEM_TUDO_FAZ_TUDO,int,safe_pkt_t,decoded_pkt_t,unsigned char,unsigned int);
AGENT_END

};

DECLARE_COMPONENT(Dummy_Caller);

#endif
