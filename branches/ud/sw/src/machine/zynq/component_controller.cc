// EPOS Cortex-A Component Controller Implementation

#include <machine/cortex_a/component_controller.h>

__BEGIN_SYS

bool Cortex_A_Component_Controller::alloc_proxy(Buffer * buf)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::alloc_proxy(buf=" << *buf << ")" << endl;

    // Issue an ALLOC_PROXY command and wait its processing
    while(!ctrl(IDLE));
    ctrl(CMD) = ALLOC_PROXY;
    while(!ctrl(IDLE));

    if((buf->_id = ctrl(RESULT)) != CMD_RESULT_ERR) {
        // Configure Buffer registers
        buf->reg(ADDR_X) = buf->_addr._x;
        buf->reg(ADDR_Y) = buf->_addr._y;
        buf->reg(ADDR_LOCAL) = buf->_addr._h;
        buf->reg(TYPE) = buf->_type;
        buf->reg(UNIT) = buf->_unit;
    }

    db<Cortex_A_Component_Controller>(TRC) << "id=" << buf->_id << endl;

    return buf->_id != CMD_RESULT_ERR;
}

// TODO: Test it!
bool Cortex_A_Component_Controller::alloc_agent(Buffer * buf)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::alloc_agent(buf=" << *buf << ")" << endl;

    // Issue an ALLOC_AGENT command and wait its processing
    while(!ctrl(IDLE));
    ctrl(CMD) = ALLOC_AGENT;
    while(!ctrl(IDLE));

    if((buf->_id = ctrl(RESULT)) != CMD_RESULT_ERR) {
        // Configure Buffer registers
        buf->reg(DISP_ADDR) = buf->_info.disp_addr;
        buf->reg(OBJ_ADDR) = buf->_info.obj_addr;
        buf->reg(TYPE) = buf->_type;
        buf->reg(UNIT) = buf->_unit;
    }

    db<Cortex_A_Component_Controller>(TRC) << "id=" << buf->_id << endl;

    return buf->_id != CMD_RESULT_ERR;
}

bool Cortex_A_Component_Controller::free_buf(Buffer * buf)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::free_buf(buf=" << *buf << ")" << endl;

    while(!ctrl(IDLE));
    // TODO: HACK! Fix comp_manager.v someday.
    ctrl(CMD) = (buf->_id << 2) | FREE_BUF;

    while(!ctrl(IDLE));

    return ctrl(RESULT) != CMD_RESULT_ERR;
}

// Send return data to component associated with buffer buf_id
void Cortex_A_Component_Controller::send_return_data(Buffer * buf,
        unsigned int n_ret, unsigned long * data)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::send_return_data(buf=" << *buf
        << ",n_ret=" << n_ret << ",data(";

    for(unsigned int i = 0; i < n_ret; i++)
        db<Cortex_A_Component_Controller>(TRC)
            << reinterpret_cast<void *>(data[i]) << ",";

    db<Cortex_A_Component_Controller>(TRC) << "))" << endl;

    while(buf->reg(TX));
    buf->reg(MSG_TYPE) = RTSNoC::RESP_DATA;

    for(unsigned int i = 0; i < n_ret; i++) {
        buf->reg(DATA) = data[i];
        buf->reg(TX) = true;
        while(buf->reg(TX));
    }
}

// Return the operation id of the received call
unsigned int Cortex_A_Component_Controller::receive_call(Buffer * buf)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::receive_call(buf=" << *buf << ")" << endl;

    unsigned int op_id = 0xFFFFFFFF;

    if(buf->reg(RX)) {
        unsigned int msg_type = buf->reg(MSG_TYPE);

        if(msg_type == RTSNoC::CALL)
            op_id = buf->reg(DATA);
        else
            db<Cortex_A_Component_Controller>(WRN)
                << "Received wrong msg_type: " << msg_type << endl;

        buf->reg(RX) = false;
    }

    db<Cortex_A_Component_Controller>(TRC) << "op_id=" << op_id << endl;

    return op_id;
}

// Return the received data stored in the buffer buf_id
unsigned int Cortex_A_Component_Controller::receive_call_data(Buffer * buf)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::receive_call_data(buf=" << *buf << ")"
        << endl;

    unsigned int call_data = 0;

    if(buf->reg(RX)) {
        unsigned int msg_type = buf->reg(MSG_TYPE);

        if(msg_type == RTSNoC::CALL_DATA)
            call_data = buf->reg(DATA);
        else
            db<Cortex_A_Component_Controller>(WRN)
                << "Received wrong msg_type: " << msg_type << endl;

        buf->reg(RX) = false;
    }

    db<Cortex_A_Component_Controller>(TRC) << "= "
        << reinterpret_cast<void *>(call_data) << endl;

    return call_data;
}

void Cortex_A_Component_Controller::send_call(Buffer * buf,
        unsigned int op_id)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::send_call(buf=" << *buf << ",op_id=" << op_id
        << ")" << endl;

    while(buf->reg(TX));

    buf->reg(MSG_TYPE) = RTSNoC::CALL;
    buf->reg(DATA) = op_id;
    buf->reg(TX) = true;
}

void Cortex_A_Component_Controller::send_call_data(Buffer * buf,
        unsigned int n_args, unsigned long * data)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::send_call_data(buf=" << *buf << ",n_args="
        << n_args << ",data(";

    for(unsigned int i = 0; i < n_args; i++)
        db<Cortex_A_Component_Controller>(TRC) << reinterpret_cast<void *>(data[i])
            << ",";

    db<Cortex_A_Component_Controller>(TRC) << "))" << endl;

    while(buf->reg(TX));
    buf->reg(MSG_TYPE) = RTSNoC::CALL_DATA;

    for(unsigned int i = 0; i < n_args; i++) {
        buf->reg(DATA) = data[i];
        buf->reg(TX) = true;
        while(buf->reg(TX));
    }
}

void Cortex_A_Component_Controller::receive_return_data(Buffer * buf,
        unsigned int n_ret, unsigned long * data)
{
    db<Cortex_A_Component_Controller>(TRC)
        << "Component_Controller::receive_return_data(buf=" << *buf << ",n_ret="
        << n_ret << ")" << endl;

    for(unsigned int i = 0; i < n_ret; i++) {
        while(!buf->reg(RX));

        unsigned int msg_type = buf->reg(MSG_TYPE);

        if(msg_type == RTSNoC::RESP_DATA)
            data[i] = buf->reg(DATA);
        else
            db<Cortex_A_Component_Controller>(WRN)
                << "Received wrong msg_type: " << msg_type << ")" << endl;

        buf->reg(RX) = false;
    }

    db<Cortex_A_Component_Controller>(TRC) << "data(";

    for (unsigned int i = 0; i < n_ret; i++)
        db<Cortex_A_Component_Controller>(TRC)
            << reinterpret_cast<void *>(data[i]) << ",";

    db<Cortex_A_Component_Controller>(TRC) << ")" << endl;
}

//bool Cortex_A_Component_Controller::agent_has_call(agent_call_info &info)
//{
    //db<Cortex_A_Component_Controller>(TRC)
        //<< "Component_Controller::agent_has_call()" << endl;

    //if(ctrl(AGENT_INT)) {
        //unsigned int buf_id = ctrl(AGENT_BUF);

        //info.buffer = buf_id;
        //info.dispatcher_address = buf_agent_disp_addr(buf_id);
        //info.object_address = buf_agent_disp_obj_addr(buf_id);

        //return true;
    //} else
        //return false;
//}

// TODO: Implement me
void Cortex_A_Component_Controller::enable_agent_receive_int(IC::Interrupt_Handler h)
{
}

// TODO: Implement me
void Cortex_A_Component_Controller::disable_agent_receive_int()
{
}

__END_SYS
