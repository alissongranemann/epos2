// EPOS Zynq Component Controller Implementation

#include <mach/zynq/component_controller.h>

__BEGIN_SYS

// Allocate a proxy buffer in the Component_Controller. Returns the number of
// the allocated buffer.
unsigned int Zynq_Component_Controller::alloc_proxy(
        Zynq_Component_Controller::Address addr, Type_Id type_id,
        unsigned int inst_id) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::alloc_proxy(x=" << addr.x << ",y=" << addr.y
        << ",local=" << addr.local << ",type_id=" << type_id << ",inst_id="
        << inst_id << ")" << endl;

    while(!ctrl_cmd_idle());
    ctrl_cmd(CMD_ALLOC_PROXY);

    while(!ctrl_cmd_idle());
    unsigned int buf_id = ctrl_cmd_result();

    if(buf_id != CMD_RESULT_ERR) {
        // Set buffer registers
        buf_proxy_phy_x(buf_id, addr.x);
        buf_proxy_phy_y(buf_id, addr.y);
        buf_proxy_phy_local(buf_id, addr.local);
        buf_type_id(buf_id, type_id);
        buf_instance_id(buf_id, inst_id);
    }

    db<Zynq_Component_Controller>(TRC) << "buf_id=" << buf_id << endl;

    return buf_id;
}

// Allocate an agent buffer in the Component_Controller. Returns the number of
// the allocated buffer.
unsigned int Zynq_Component_Controller::alloc_agent(Type_Id type_id,
        unsigned int inst_id, unsigned int dispatcher_address,
        unsigned int dispatcher_object_address) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::alloc_agent(type_id="
        << type_id << ",inst_id=" << inst_id << ",addr="
        << reinterpret_cast<void *>(dispatcher_address) << ",obj_addr="
        << reinterpret_cast<void *>(dispatcher_object_address) << ")" << endl;

    while(!ctrl_cmd_idle());
    ctrl_cmd(CMD_ALLOC_AGENT);

    while(!ctrl_cmd_idle());
    unsigned int buf_id = ctrl_cmd_result();

    if(buf_id != CMD_RESULT_ERR) {
        // Set buffer registers
        buf_type_id(buf_id, type_id);
        buf_instance_id(buf_id, inst_id);
        buf_agent_disp_addr(buf_id, dispatcher_address);
        buf_agent_disp_obj_addr(buf_id, dispatcher_object_address);
    } else
        db<Zynq_Component_Controller>(WRN) << "Couldn't allocate buffer"
            << endl;

    db<Zynq_Component_Controller>(TRC) << "buf_id=" << buf_id << endl;

    return buf_id;
}

unsigned int Zynq_Component_Controller::free_buf(unsigned int buf_id) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::free_buf(buf_id=" << buf_id << ")" << endl;

    while(!ctrl_cmd_idle());
    // TODO: HACK! Fix comp_manager.v someday.
    ctrl_cmd((buf_id << 2) | CMD_FREE_BUF);

    while(!ctrl_cmd_idle());
    unsigned int result = ctrl_cmd_result();

    db<Zynq_Component_Controller>(TRC) << "result=" << result << endl;

    return buf_id;
}

// Send return data to component associated with buffer buf_id
void Zynq_Component_Controller::send_return_data(unsigned int buf_id,
        unsigned int n_ret, unsigned int * data) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::send_return_data(buf_id=" << buf_id
        << ",n_ret=" << n_ret << ",data(";

    for (unsigned int i = 0; i < n_ret; i++)
        db<Zynq_Component_Controller>(TRC)
            << reinterpret_cast<void *>(data[i]) << ",";

    db<Zynq_Component_Controller>(TRC) << "))" << endl;

    while(buf_tx(buf_id));
    buf_msg_type(buf_id, Implementation::MSG_TYPE_RESP_DATA);

    for (unsigned int i = 0; i < n_ret; i++) {
        buf_data_tx(buf_id, data[i]);
        buf_tx(buf_id,true);
        while(buf_tx(buf_id));
    }
}

// Return the operation id of the received call
unsigned int Zynq_Component_Controller::receive_call(unsigned int buf_id) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::receive_call(buf_id=" << buf_id << ")"
        << endl;

    unsigned int op_id = 0xFFFFFFFF;

    if(buf_rx(buf_id)) {
        unsigned int msg_type = buf_msg_type(buf_id);

        if(msg_type == Implementation::MSG_TYPE_CALL)
            op_id = buf_data_rx(buf_id);
        else
            db<Zynq_Component_Controller>(WRN)
                << "Received incorrect msg_type(" << msg_type << ")" << endl;

        buf_rx(buf_id, false);
    } else
        db<Zynq_Component_Controller>(WRN)
            << "No received data on buffer " << buf_id << endl;

    db<Zynq_Component_Controller>(TRC) << "op_id=" << op_id << endl;

    return op_id;
}

// Return the received data stored in the buffer buf_id
unsigned int Zynq_Component_Controller::receive_call_data(unsigned int buf_id) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::receive_call_data(buf_id=" << buf_id << ")"
        << endl;

    unsigned int call_data = 0;

    if(buf_rx(buf_id)) {
        unsigned int msg_type = buf_msg_type(buf_id);

        if(msg_type == Implementation::MSG_TYPE_CALL_DATA)
            call_data = buf_data_rx(buf_id);
        else
            db<Zynq_Component_Controller>(WRN)
                << "Received incorrect msg_type(" << msg_type << ")" << endl;

        buf_rx(buf_id, false);
    } else
        db<Zynq_Component_Controller>(WRN)
            << "No received data on buffer " << buf_id << endl;

    db<Zynq_Component_Controller>(TRC) << "= "
        << reinterpret_cast<void *>(call_data) << endl;

    return call_data;
}

void Zynq_Component_Controller::send_call (unsigned int buf_id,
        unsigned int op_id) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::send_call(buf_id=" << buf_id << ",op_id="
        << op_id << ")" << endl;

    while(buf_tx(buf_id));

    buf_msg_type(buf_id, Implementation::MSG_TYPE_CALL);
    buf_data_tx(buf_id, op_id);

    buf_tx(buf_id, true);
}

void Zynq_Component_Controller::send_call_data(unsigned int buf_id,
        unsigned int n_args, unsigned int * data) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::send_call_data(buf_id=" << buf_id
        << ",n_args=" << n_args << ",data(";

    for (unsigned int i = 0; i < n_args; i++)
        db<Zynq_Component_Controller>(TRC) << reinterpret_cast<void *>(data[i])
            << ",";

    db<Zynq_Component_Controller>(TRC) << "))" << endl;

    while(buf_tx(buf_id));
    buf_msg_type(buf_id, Implementation::MSG_TYPE_CALL_DATA);

    for (unsigned int i = 0; i < n_args; i++) {
        buf_data_tx(buf_id, data[i]);
        buf_tx(buf_id, true);
        while(buf_tx(buf_id));
    }
}

void Zynq_Component_Controller::receive_return_data(unsigned int buf_id,
        unsigned int n_ret, unsigned int * data) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::receive_return_data(buf_id=" << buf_id
        << ",n_ret=" << n_ret << ")" << endl;

    for (unsigned int i = 0; i < n_ret; i++) {
        while(!buf_rx(buf_id));

        unsigned int msg_type = buf_msg_type(buf_id);

        if(msg_type == Implementation::MSG_TYPE_RESP_DATA)
            data[i] = buf_data_rx(buf_id);
        else
            db<Zynq_Component_Controller>(WRN)
                << "Received incorrect msg_type(" << msg_type << ")" << endl;

        buf_rx(buf_id, false);
    }

    db<Zynq_Component_Controller>(TRC) << "data(";

    for (unsigned int i = 0; i < n_ret; i++)
        db<Zynq_Component_Controller>(TRC)
            << reinterpret_cast<void *>(data[i]) << ",";

    db<Zynq_Component_Controller>(TRC) << ")" << endl;
}

bool Zynq_Component_Controller::agent_has_call(agent_call_info &info) {
    db<Zynq_Component_Controller>(TRC)
        << "Component_Controller::agent_has_call()" << endl;

    if(ctrl_status_agent_int()) {
        unsigned int buf_id = ctrl_status_agent_buf();

        info.buffer = buf_id;
        info.dispatcher_address = buf_agent_disp_addr(buf_id);
        info.object_address = buf_agent_disp_obj_addr(buf_id);

        return true;
    } else
        return false;
}

// TODO: Implement me
void Zynq_Component_Controller::enable_agent_receive_int(IC::Interrupt_Handler h) {
}

// TODO: Implement me
void Zynq_Component_Controller::disable_agent_receive_int() {
}

__END_SYS
