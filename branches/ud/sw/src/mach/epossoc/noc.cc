// EPOS EPOSSOC_NOC Implementation

#include <noc.h>

__BEGIN_SYS

EPOSSOC_NOC * EPOSSOC_NOC::_instantance = 0;

EPOSSOC_NOC::EPOSSOC_NOC():NOC_Common() {
    _info.local_addr = local_addr();
    _info.router_x_addr = router_x_addr();
    _info.router_y_addr = router_y_addr();
    _info.net_x_size = net_x_size();
    _info.net_y_size = net_y_size();
    _info.data_width = net_data_width();

    header_src_router_x_addr(_info.router_x_addr);
    header_src_router_y_addr(_info.router_y_addr);
    header_src_local_addr(_info.local_addr);

    if(_info.data_width != Traits<EPOSSOC_NOC>::NOC_DATA_BUS_SIZE)
        db<EPOSSOC_NOC>(WRN) << "EPOSSOC_NOC(): Data bus size = "
            << _info.data_width << " differs from Traits" << endl;
}

void EPOSSOC_NOC::send_header(Address const * address) {
    header_dst_local_addr(address->local);
    header_dst_router_x_addr(address->x);
    header_dst_router_y_addr(address->y);
}

void EPOSSOC_NOC::send(Packet const * _data) {
    while(wait());
    for (unsigned int i = 0; i < Traits<EPOSSOC_NOC>::PROC_NOC_BUS_RATE; ++i)
        data(i,_data->data[i]);
    wr();
}

void EPOSSOC_NOC::send(Address const * address, Packet const * _data) {
    send_header(address);
    send(_data);
}

void EPOSSOC_NOC::receive_header(Address * address) {
    address->local = header_src_local_addr();
    address->x = header_src_router_x_addr();
    address->y = header_src_router_y_addr();
}
void EPOSSOC_NOC::receive(Packet * _data) {
    while(!nd());
    for (unsigned int i = 0; i < Traits<EPOSSOC_NOC>::PROC_NOC_BUS_RATE; ++i)
        _data->data[i] = data(i);
    rd();
}

void EPOSSOC_NOC::receive(Address * address, Packet * _data) {
    receive_header(address);
    receive(_data);
}

void EPOSSOC_NOC::receive_int(IC::Interrupt_Handler h) {
    CPU::int_disable();
    disable_receive_int();
    IC::int_vector(IC::IRQ_NOC, h);
    enable_receive_int();
    CPU::int_enable();
}

__END_SYS
