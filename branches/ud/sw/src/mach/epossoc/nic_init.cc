// EPOS EPOSSOC NULL NIC Mediator Initialization

#include <machine.h>
#include <nic.h>

__BEGIN_SYS

/*
template <int unit>
inline static void call_init()
{
    Traits<EPOSSOC_NIC>::NICS::template Get<unit>::Result::init(unit);
    call_init<unit + 1>();
};

template <>
inline static void call_init<Traits<EPOSSOC_NIC>::NICS::Length>() {};
*/

void EPOSSOC_NIC::init()
{
    //return call_init<0>();
    db<EPOSSOC_NIC>(TRC) << "EPOSSOC_NIC::init()\n";

}

__END_SYS
