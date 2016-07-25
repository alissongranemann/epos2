// EPOS Cortex NIC Mediator Initialization

#include <machine/cortex/nic.h>

__BEGIN_SYS

//template<int unit>
//inline static void call_init()
//{
//    typedef typename Traits<PC_Ethernet>::NICS::template Get<unit>::Result NIC;
//    static const unsigned int OFFSET = Traits<PC_Ethernet>::NICS::template Find<NIC>::Result;
//
//    if(Traits<NIC>::enabled)
//        NIC::init(unit - OFFSET);
//
//    call_init<unit + 1>();
//};
//
//template<>
//inline void call_init<Traits<PC_Ethernet>::NICS::Length>()
//{
//};
//
//void PC_Ethernet::init()
//{
//    call_init<0>();
//}
//
//__END_SYS

template<typename Type, int unit>
inline static void call_init()
{
    typedef typename Traits<Type>::NICS::template Get<unit>::Result NIC;
    static const unsigned int OFFSET = Traits<Type>::NICS::template Find<NIC>::Result;

    if(Traits<NIC>::enabled)
        NIC::init(unit - OFFSET);

    call_init<Type, unit + 1>();
};

#ifdef __mmod_zynq__

template<>
inline void call_init<Cortex_Ethernet, Traits<Cortex_Ethernet>::NICS::Length>() {}

void Cortex_Ethernet::init()
{
    call_init<Cortex_Ethernet, 0>();
}

#else

template<>
inline void call_init<Cortex_IEEE802_15_4, Traits<Cortex_IEEE802_15_4>::NICS::Length>() {}

void Cortex_IEEE802_15_4::init()
{
    call_init<Cortex_IEEE802_15_4, 0>();
}

#endif

__END_SYS
