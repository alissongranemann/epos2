// EPOS Add Abstraction Declarations

#ifndef __dummy_hw_area_test_unified_h
#define __dummy_hw_area_test_unified_h

#include "component.h"

//#define METHOD_TWO

namespace Implementation {

template <> struct Traits<Dummy_Component_For_Proxy>: public Traits<void>
{
    static const bool hardware = false;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 4;
    static const unsigned int n_ids = 1;
};

class Dummy_Component_For_Proxy: public Component
{
public:
    enum {
        OP_FUNC_0 = 0,
        OP_FUNC_1
    };

public:
    Dummy_Component_For_Proxy(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char inst_id):
        Component(rx_ch, tx_ch, inst_id) {}

    unsigned int func_0(unsigned int arg0) { return arg0; }
#ifdef METHOD_TWO
    unsigned int func_1(unsigned int arg0) { return arg0; }
#endif
};

PROXY_BEGIN(Dummy_Component_For_Proxy)
    unsigned int func_0(unsigned int);
#ifdef METHOD_TWO
    unsigned int func_1(unsigned int);
#endif
PROXY_END

unsigned int Implementation::Proxy<Implementation::Dummy_Component_For_Proxy>::func_0(unsigned int arg) {
    return Base::call_r<Dummy_Component_For_Proxy::OP_FUNC_0, unsigned int>(arg);
}

#ifdef METHOD_TWO
unsigned int Implementation::Proxy<Implementation::Dummy_Component_For_Proxy>::func_1(unsigned int arg0) {
    return Base::call_r<Dummy_Component_For_Proxy::OP_FUNC_1, unsigned int>(arg0);
}
#endif

};

DECLARE_COMPONENT(Dummy_Component_For_Proxy);

namespace Implementation {

template <> struct Traits<Dummy_HW_Area_Test>: public Traits<void>
{
    static const bool hardware = true;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 4;
    static const unsigned int n_ids = 1;
};

class Dummy_HW_Area_Test: public Component
{
public:
    enum {
        OP_FUNC_0   = 0xF0,
        OP_FUNC_1   = 0xF1
    };

public:
    EPOS::Dummy_Component_For_Proxy proxy;

    Dummy_HW_Area_Test(Channel_t &rx_ch, Channel_t &tx_ch,
            unsigned char inst_id): Component(rx_ch, tx_ch, inst_id),
            proxy(rx_ch, tx_ch, inst_id) {}

    //Dummy_HW_Area_Test(Channel_t &rx_ch, Channel_t &tx_ch,
            //unsigned char inst_id): Component(rx_ch, tx_ch, inst_id) {}

    unsigned int func_0(unsigned int arg0) { return proxy.func_0(arg0); }
    //unsigned int func_0(unsigned int arg0) { return arg0; }
#ifdef METHOD_TWO
    unsigned int func_1(unsigned int arg0) { return proxy.func_1(arg0); }
    //unsigned int func_1(unsigned int arg0) { return arg0; }
#endif
};

AGENT_BEGIN(Dummy_HW_Area_Test)
    //D_CALL_R_1(func_0, OP_FUNC_0, unsigned int, unsigned int)
//#ifdef METHOD_TWO
    //D_CALL_R_1(func_1, OP_FUNC_1, unsigned int, unsigned int)
//#endif
AGENT_END

};

DECLARE_COMPONENT(Dummy_HW_Area_Test);

#endif
