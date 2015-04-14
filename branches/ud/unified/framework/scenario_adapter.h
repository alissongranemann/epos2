#ifndef __scenario_adapter_h
#define __scenario_adapter_h

#include <system/config.h>
#include <framework/scenario.h>

__BEGIN_SYS

// Default declaration, used when no enter/leave operations are needed
template<typename C>
class Scenario_Adapter: public C,
        public IF<Traits<C>::hardware,
            HW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max>,
            SW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max> >::Result
{
public:
    Scenario_Adapter(Channel_t &rx_ch, Channel_t &tx_ch): C(rx_ch, tx_ch) {}

private:
    typedef typename IF<Traits<C>::hardware,
        HW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max>,
        SW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max> >::Result
        Scenario;
};

// Macro for creating a specialization using the default declaration
#define ADAPTER_BEGIN(name)\
template<>\
class Scenario_Adapter<name >:\
    public name,\
    public IF<Traits<name>::hardware,\
        HW_Scenario<name, Traits<name>::Alloc_Obj_Type, Traits<name>::Alloc_Idx, Traits<name>::Alloc_Max>,\
        SW_Scenario<name, Traits<name>::Alloc_Obj_Type, Traits<name>::Alloc_Idx, Traits<name>::Alloc_Max> >::Result\
{\
private:\
    typedef typename IF<Traits<C>::hardware,\
        HW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max>,\
        SW_Scenario<C, typename Traits<C>::Alloc_Obj_Type, typename Traits<C>::Alloc_Idx, Traits<C>::Alloc_Max> >::Result\
        Scenario;\
\
public:\
    typedef Scenario::Link Link;\
    Scenario_Adapter(Channel_t &rx_ch, Channel_t &tx_ch): name(rx_ch, tx_ch) {}\

#define ADAPTER_END };

__END_SYS

#endif
