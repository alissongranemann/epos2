// EPOS Component Manager Abstraction Implementation

#include <component_manager.h>

__BEGIN_SYS

void Component_Manager::call(Id id, Method m, unsigned int n_args,
        unsigned int n_ret, unsigned int * data) {
    db<Component_Manager>(TRC) << "Component_Manager::call(id=" << id
        << ",m=" << m << ",n_args=" << n_args << ",n_ret=" << n_ret
        << ",data=" << data << "(";

    for (unsigned int i = 0; i < n_args; ++i)
        db<Component_Manager>(TRC) << reinterpret_cast<void *>(data[i]) << ",";

    db<Component_Manager>(TRC) << "))" << endl;

    // TODO: Fix the hardcored id
    Component_Controller::send_call(0, m);
    Component_Controller::send_call_data(0, n_args, data);
    Component_Controller::receive_return_data(0, n_ret, data);
}

void Component_Manager::int_handler(const unsigned int & interrupt) {
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
