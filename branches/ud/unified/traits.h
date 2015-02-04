// Unified Traits

#ifndef __traits_unified_h
#define __traits_unified_h

#include "framework/types.h"
#include "framework/meta.h"

namespace Implementation {

template <class Imp>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = false;
    static const bool power_management = false;
};

template<>

struct Traits<Configurations::EPOS_SOC_Catapult>
{
    const static unsigned int serdes_pkt_size = 4;
};

template<>
struct Traits<Sys>
{
#ifdef HIGH_LEVEL_SYNTHESIS
    const static bool hardware_domain = true;
#else
    const static bool hardware_domain = false;
#endif
    const static bool timed_simulation = true;

    typedef Configurations::EPOS_SOC_Catapult Platform;

    const static unsigned int serdes_pkt_size = Traits<Platform>::serdes_pkt_size;
};

template <> struct Traits<Dummy_Callee>: public Traits<void>
{
    static const bool hardware = false;
    static const bool singleton = false;// Gambi na resource table
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 64;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<Dummy_Caller>: public Traits<void>
{
    static const bool hardware = false;
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 128;
    static const unsigned int n_ids = 2;
};

template <> struct Traits<Add>: public Traits<void>
{
    static const bool hardware = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<EC>: public Traits<void>
{
    static const bool hardware = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<MAC>: public Traits<void>
{
    static const bool hardware = true;
    static const bool reconfigurable = true;
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<AES>: public Traits<void>
{
    static const bool hardware = true;
    static const bool reconfigurable = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 16;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<Mult>: public Traits<void>
{
    static const bool hardware = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = Traits<Add>::n_ids + 1;
};

template <> struct Traits<DTMF_Detector>: public Traits<void>
{
    static const bool hardware = true;
    static const bool reconfigurable = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<ADPCM_Codec>: public Traits<void>
{
    static const bool hardware = true;
    static const bool reconfigurable = true;

    static const int recfg_cyles = 11908;
    static const int cipher_sw_cyles = 61854;
    static const int cipher_hw_cyles = 1178;

    static const bool singleton = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 4;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<Sched<void,void> >: public Traits<void>
{
    static const bool hardware = false;

    static const bool singleton = false;

    static const unsigned int n_ids = 1;

    static const unsigned int queue_size = 8;//Static-alloc-only
};

template <> struct Traits<RSP_Controller>: public Traits<void>
{
    static const bool hardware = false; //always false
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 4;
    static const unsigned int n_ids = 1;
};

template <> struct Traits<RSP_DTMF>: public Traits<void>
{
    static const bool hardware = false;
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 64;
    static const unsigned int n_ids = Traits<RSP_Controller>::n_ids + 1;
};

template <> struct Traits<RSP_ADPCM>: public Traits<void>
{
    static const bool hardware = false;
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 16;
    static const unsigned int n_ids = Traits<RSP_DTMF>::n_ids + 1;
};

template <> struct Traits<RSP_AES>: public Traits<void>
{
    static const bool hardware = false;
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 16;
    static const unsigned int n_ids = Traits<RSP_ADPCM>::n_ids + 1;
};

template <> struct Traits<RSP_ETH>: public Traits<void>
{
    static const bool hardware = false; // always true
    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 4;
    static const unsigned int n_ids = Traits<RSP_AES>::n_ids + 1;
};

// Where should we put these?
template<class Implementation, class Proxy, bool hardware>
struct MAP {
    typedef typename IF<hardware==Traits<Sys>::hardware_domain, Implementation, Proxy>::Result
        Result;
};

// Helper
#define DECLARE_COMPONENT(name)\
namespace EPOS {\
typedef Implementation::MAP<Implementation::Scenario_Adapter<Implementation::name>,\
                            Implementation::Proxy<Implementation::name>,\
                            Implementation::Traits<Implementation::name>::hardware>::Result\
        name;\
};

#define DECLARE_RECFG_COMPONENT(name)\
namespace EPOS {\
typedef Implementation::MAP<Implementation::Scenario_Adapter<Implementation::name>,\
                            Implementation::Handle<Implementation::name>,\
                            Implementation::Traits<Implementation::name>::hardware>::Result\
        name;\
};

#define DECLARE_MODEL(name)\
class name: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::name, \
                                Untimed::name>::Result { \
public:\
    typedef Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::name, \
                                Untimed::name>::Result Base;\
\
    name(sc_module_name nm) :Base(nm){}\
}

#define DECLARE_SLAVE_DEVICE_MODEL(name)\
class name: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::name, \
                                Untimed::name>::Result { \
public:\
    typedef Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation, \
                                Timed::name, \
                                Untimed::name>::Result Base;\
\
    name(sc_module_name nm, unsigned int start_address, unsigned int end_address)\
        :Base(nm,start_address,end_address){}\
}

};

#endif
