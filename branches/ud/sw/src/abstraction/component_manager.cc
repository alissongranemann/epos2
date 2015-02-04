// EPOS Component Manager Abstraction Implementation

#include <utility/malloc.h>
#include <component_manager.h>

__BEGIN_SYS

void Component_Manager::call(Buffer * buf, unsigned int op_id,
        unsigned int n_args, unsigned int n_ret, unsigned int * data) {
    db<Component_Manager>(TRC) << "Component_Manager::call(buf=" << buf
        << ",op_id=" << op_id << ",n_args=" << n_args << ",n_ret=" << n_ret
        << ",data=" << data << "(";

    for (unsigned int i = 0; i < n_args; ++i)
        db<Component_Manager>(TRC) << reinterpret_cast<void *>(data[i]) << ",";

    db<Component_Manager>(TRC) << "))" << endl;

    Component_Controller::send_call(buf->id, op_id);
    Component_Controller::send_call_data(buf->id, n_args, data);
    Component_Controller::receive_return_data(buf->id, n_ret, data);
}

void Component_Manager::recfg(Buffer * buf, Type_Id type_id) {
    db<Component_Manager>(TRC) << "Component_Manager::recfg(buf="
        << buf << "," << "type_id=" << type_id << ")" << endl;

    unsigned int local = ((buf->node)->addr)->local;

    (buf->node)->decouple();
    _pcap->transfer(_bitstreams[local][type_id].addr, _bitstreams[local][type_id].n_bytes/4);
    (buf->node)->couple();
}

void Component_Manager::int_handler(unsigned int interrupt) {
    //Component_Controller::disable_agent_receive_int();
    db<Component_Manager>(TRC) << "Component_Manager::int_handler(): begin"
        << endl;

    Component_Controller::agent_call_info call_info;

    while(Component_Controller::agent_has_call(call_info)) {
        SW_Dispatcher dispatcher = (SW_Dispatcher)call_info.dispatcher_address;
        dispatcher(call_info);
    }
    //Component_Controller::enable_agent_receive_int(&int_handler);
    db<Component_Manager>(TRC) << "Component_Manager::int_handler(): end"
        << endl;
}

__END_SYS
