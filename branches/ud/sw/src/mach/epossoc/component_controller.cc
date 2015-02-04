// EPOS EPOSSOC Component Controller Implementation

#include <mach/epossoc/component_controller.h>

__BEGIN_SYS

// Allocate a proxy buffer in the Component_Controller. Returns the number of
// the allocated buffer.
unsigned int EPOSSOC_Component_Controller::alloc_proxy(
        EPOSSOC_Component_Controller::Address addr, Type_Id type_id,
        unsigned int inst_id) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::alloc_proxy(" << (void *)addr.x << ", "
        << (void *)addr.y << ", " << (void *)addr.local << ", " << (void *)type_id
        << ", " << (void *)inst_id << ")" << endl;

    while(!ctrl_cmd_idle());
    ctrl_cmd(CMD_ALLOC_PROXY);

    while(!ctrl_cmd_idle());
    unsigned int buf_num = ctrl_cmd_result();

    if(buf_num != CMD_RESULT_ERR) {
        // Set buffer registers
        buf_proxy_phy_x(buf_num, addr.x);
        buf_proxy_phy_y(buf_num, addr.y);
        buf_proxy_phy_local(buf_num, addr.local);
        buf_type_id(buf_num, type_id);
        buf_instance_id(buf_num, inst_id);
    } else
        db<EPOSSOC_Component_Controller>(WRN) << "Couldn't allocate buffer" << endl;

    db<EPOSSOC_Component_Controller>(TRC) << "= " << (void *)buf_num << endl;

    return buf_num;
}

// Allocate an agent buffer in the Component_Controller. Returns the number of
// the allocated buffer.
unsigned int EPOSSOC_Component_Controller::alloc_agent(Type_Id type_id,
        unsigned int inst_id, unsigned int dispatcher_address,
        unsigned int dispatcher_object_address) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::alloc_agent(" << (void *)type_id << ", "
        << (void *)inst_id << ", " << (void *)dispatcher_address << ", "
        << (void *)dispatcher_object_address << ")" << endl;

    while(!ctrl_cmd_idle());
    ctrl_cmd(CMD_ALLOC_AGENT);

    while(!ctrl_cmd_idle());
    unsigned int buf_num = ctrl_cmd_result();

    if(buf_num != CMD_RESULT_ERR) {
        // Set buffer registers
        buf_type_id(buf_num, type_id);
        buf_instance_id(buf_num, inst_id);
        buf_agent_disp_addr(buf_num, dispatcher_address);
        buf_agent_disp_obj_addr(buf_num, dispatcher_object_address);
    } else
        db<EPOSSOC_Component_Controller>(WRN) << "Couldn't allocate buffer" << endl;

    db<EPOSSOC_Component_Controller>(TRC) << "= " << (void *)buf_num << endl;

    return buf_num;
}

// Send return data to component associated with buffer buf_num
void EPOSSOC_Component_Controller::send_return_data(unsigned int buf_num,
        unsigned int n_ret, unsigned int * data) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::send_return_data(" << (void *)buf_num << ", "
        << (void *)n_ret << ", " << "data(";

    for (unsigned int i = 0; i < n_ret; ++i)
        db<EPOSSOC_Component_Controller>(TRC) << (void *)data[i] << ", ";

    db<EPOSSOC_Component_Controller>(TRC) << "))" << endl;

    while(buf_tx(buf_num));
    buf_msg_type(buf_num, Implementation::MSG_TYPE_RESP_DATA);

    for (unsigned int i = 0; i < n_ret; ++i) {
        buf_data_tx(buf_num, data[i]);
        buf_tx(buf_num,true);
        while(buf_tx(buf_num));
    }
}

// Return the operation id of the received call
unsigned int EPOSSOC_Component_Controller::receive_call(unsigned int buf_num) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::receive_call(" << (void *)buf_num << ")"
        << endl;

    unsigned int op_id = 0xFFFFFFFF;

    if(buf_rx(buf_num)) {
        unsigned int msg_type = buf_msg_type(buf_num);

        if(msg_type == Implementation::MSG_TYPE_CALL)
            op_id = buf_data_rx(buf_num);
        else
            db<EPOSSOC_Component_Controller>(WRN)
                << "Received msg of type " << (void *)msg_type
                << " is not a call" << endl;

        buf_rx(buf_num, false);
    } else
        db<EPOSSOC_Component_Controller>(WRN)
            << "No received data on buffer " << (void *)buf_num << endl;

    db<EPOSSOC_Component_Controller>(TRC) << "= " << (void *)op_id << endl;

    return op_id;
}

// Return the received data stored in the buffer buf_num
unsigned int EPOSSOC_Component_Controller::receive_call_data(unsigned int buf_num) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::receive_call_data(" << (void *)buf_num << ")"
        << endl;

    unsigned int call_data = 0;

    if(buf_rx(buf_num)) {
        unsigned int msg_type = buf_msg_type(buf_num);

        if(msg_type == Implementation::MSG_TYPE_CALL_DATA)
            call_data = buf_data_rx(buf_num);
        else
            db<EPOSSOC_Component_Controller>(WRN)
                << "Received msg of type " << (void *)msg_type
                << " is not call data" << endl;

        buf_rx(buf_num, false);
    } else
        db<EPOSSOC_Component_Controller>(WRN)
            << "No received data on buffer " << (void *)buf_num << endl;

    db<EPOSSOC_Component_Controller>(TRC) << "= " << (void *)call_data << endl;

    return call_data;
}

void EPOSSOC_Component_Controller::send_call (unsigned int buf_num,
        unsigned int op_id) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::send_call(" << (void *)buf_num << ", "
        << (void *)op_id << ")" << endl;

    while(buf_tx(buf_num));

    buf_msg_type(buf_num, Implementation::MSG_TYPE_CALL);
    buf_data_tx(buf_num, op_id);

    buf_tx(buf_num, true);
}

void EPOSSOC_Component_Controller::send_call_data(unsigned int buf_num,
        unsigned int n_args, unsigned int * data) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::send_call_data(" << (void *)buf_num
        << ", " << (void *)n_args << ", " << "data(";

    for (unsigned int i = 0; i < n_args; ++i)
        db<EPOSSOC_Component_Controller>(TRC) << (void *)data[i] << ", ";

    db<EPOSSOC_Component_Controller>(TRC) << "))" << endl;

    while(buf_tx(buf_num));
    buf_msg_type(buf_num, Implementation::MSG_TYPE_CALL_DATA);

    for (unsigned int i = 0; i < n_args; ++i) {
        buf_data_tx(buf_num, data[i]);
        buf_tx(buf_num, true);
        while(buf_tx(buf_num));
    }
}

void EPOSSOC_Component_Controller::receive_return_data(unsigned int buf_num,
        unsigned int n_ret, unsigned int * data) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::receive_return_data(" << (void *) buf_num
        << ", " << (void *)n_ret << ")" << endl;

    for (unsigned int i = 0; i < n_ret; ++i) {
        while(!buf_rx(buf_num));

        unsigned int msg_type = buf_msg_type(buf_num);

        if(msg_type == Implementation::MSG_TYPE_RESP_DATA)
            data[i] = buf_data_rx(buf_num);
        else
            db<EPOSSOC_Component_Controller>(WRN)
                << "Received msg of type " << (void *)msg_type
                << " is not return data" << endl;

        buf_rx(buf_num, false);
    }

    db<EPOSSOC_Component_Controller>(TRC) << "=data(";

    for (unsigned int i = 0; i < n_ret; ++i)
        db<EPOSSOC_Component_Controller>(TRC) << (void *)data[i] << ", ";

    db<EPOSSOC_Component_Controller>(TRC) << ")" << endl;
}

bool EPOSSOC_Component_Controller::agent_has_call(agent_call_info &info) {
    db<EPOSSOC_Component_Controller>(TRC)
        << "Component_Controller::agent_has_call()" << endl;

    if(ctrl_status_agent_int()) {
        unsigned int buf_num = ctrl_status_agent_buf();

        info.buffer = buf_num;
        info.dispatcher_address = buf_agent_disp_addr(buf_num);
        info.object_address = buf_agent_disp_obj_addr(buf_num);

        return true;
    } else
        return false;
}

void EPOSSOC_Component_Controller::enable_agent_receive_int(IC::Interrupt_Handler h) {
    IC::disable(IC::IRQ_COMP_CONTRL);
    IC::int_vector(IC::IRQ_COMP_CONTRL, h);
    IC::enable(IC::IRQ_COMP_CONTRL);
}

void EPOSSOC_Component_Controller::disable_agent_receive_int() {
    IC::disable(IC::IRQ_COMP_CONTRL);
}

__END_SYS
