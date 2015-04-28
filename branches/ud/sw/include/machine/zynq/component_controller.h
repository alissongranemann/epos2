// EPOS Zynq Component Controller Mediator

#ifndef __zynq_component_controller_h
#define __zynq_component_controller_h

#include <machine.h>
#include <ic.h>
#include <component_controller.h>

#include "../../../../unified/rtsnoc.h"

__BEGIN_SYS

class Zynq_Component_Controller: public Component_Controller_Common
{
public:
    // RTSNoC address
    typedef RTSNoC::Address Address;

    // Error return result
    enum {
        CMD_RESULT_ERR  = 0xFFFFFFFF
    };

    struct agent_call_info {
        unsigned int buffer;
        unsigned int dispatcher_address;
        unsigned int object_address;
    };

    // Internal buffers used to implement RTSNoC's handshake
    class Buffer
    {
    public:
        Buffer(unsigned int type, unsigned int unit, Address addr) {
            proxy_id = alloc_proxy(addr, type, unit);

            if(proxy_id == CMD_RESULT_ERR) {
                db<Component_Controller>(ERR) << "Couldn't allocate proxy" << endl;
                Machine::panic();
            }
        }

        ~Buffer() { free_buf(proxy_id); }

    private:
        unsigned int proxy_id;
    };

public:
    static unsigned int alloc_proxy(Address addr, Type_Id type_id,
            unsigned int inst_id);
    static unsigned int alloc_agent(Type_Id type_id,
            unsigned int inst_id, unsigned int dispatcher_address,
            unsigned int dispatcher_object_address);
    static unsigned int free_buf(unsigned int buf_id);

    static unsigned int receive_call(unsigned int buf_id);
    static unsigned int receive_call_data(unsigned int buf_id);
    static void receive_return_data(unsigned int buf_id, unsigned int n_ret,
            unsigned long * data);

    static void send_call (unsigned int buf_id, unsigned int op_id);
    static void send_call_data(unsigned int buf_id, unsigned int n_args,
            unsigned long * data);
    static void send_return_data(unsigned int buf_id, unsigned int n_ret,
            unsigned long * data);

    static bool agent_has_call(agent_call_info &info);
    static void enable_agent_receive_int(IC::Interrupt_Handler h);
    static void disable_agent_receive_int();

private:
    // Base addresses for each register group
    enum {
        ADDR_BASE       = 0x43C00000,
        ADDR_BASE_CTRL  = ADDR_BASE,
        ADDR_BASE_BUF   = ADDR_BASE | 0x00000200
    };

    // Control registers address
    enum {
        CTRL_CMD                    = 0x00,
        CTRL_CMD_RESULT             = 0x04,
        CTRL_CMD_IDLE               = 0x08,
        CTRL_STATUS_AGENT_INT       = 0x0C,
        CTRL_STATUS_AGENT_BUF       = 0x10,
        CTRL_INFO_NOC_X             = 0x14,
        CTRL_INFO_NOC_Y             = 0x18,
        CTRL_INFO_NOC_LOCAL         = 0x1C,
        CTRL_INFO_BUF_SIZE          = 0x20
    };

    // Buffer registers address
    enum {
        BUF_PROXY_PHY_X             = 0x0 << 5,
        BUF_PROXY_PHY_Y             = 0x1 << 5,
        BUF_PROXY_PHY_LOCAL         = 0x2 << 5,
        BUF_MSG_TYPE                = 0x3 << 5,
        BUF_INSTANCE_ID             = 0x4 << 5,
        BUF_TYPE_ID                 = 0x5 << 5,
        BUF_DATA                    = 0x6 << 5,
        BUF_TX                      = 0x7 << 5,
        BUF_RX                      = 0x8 << 5,
        BUF_AGENT_DISP_ADDR         = 0x9 << 5,
        BUF_AGENT_DISP_OBJ_ADDR     = 0xA << 5
    };

    // Command opcodes
    enum {
        CMD_ALLOC_PROXY = 0,
        CMD_ALLOC_AGENT = 1,
        CMD_FREE_BUF    = 2
    };

private:
    static void ctrl_cmd(unsigned int val){ ctrl_reg(CTRL_CMD, val); }
    static unsigned int ctrl_cmd() { return ctrl_reg(CTRL_CMD); }

    static unsigned int ctrl_cmd_result() { return ctrl_reg(CTRL_CMD_RESULT); }
    static bool ctrl_cmd_idle() { return ctrl_reg(CTRL_CMD_IDLE) == 1; }

    static bool ctrl_status_agent_int() { return ctrl_reg(CTRL_STATUS_AGENT_INT) == 1; }
    static unsigned int ctrl_status_agent_buf() { return ctrl_reg(CTRL_STATUS_AGENT_BUF); }

    static unsigned int ctrl_info_noc_x() { return ctrl_reg(CTRL_INFO_NOC_X); }
    static unsigned int ctrl_info_noc_y() { return ctrl_reg(CTRL_INFO_NOC_Y); }
    static unsigned int ctrl_info_noc_local() { return ctrl_reg(CTRL_INFO_NOC_LOCAL); }
    static unsigned int ctrl_info_buf_size() { return ctrl_reg(CTRL_INFO_BUF_SIZE); }

    static void buf_proxy_phy_x(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_PROXY_PHY_X, val); }
    static unsigned int buf_proxy_phy_x(unsigned int buf_id) { return buf_reg(buf_id, BUF_PROXY_PHY_X); }

    static void buf_proxy_phy_y(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_PROXY_PHY_Y, val); }
    static unsigned int buf_proxy_phy_y(unsigned int buf_id) { return buf_reg(buf_id, BUF_PROXY_PHY_Y); }

    static void buf_proxy_phy_local(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_PROXY_PHY_LOCAL, val); }
    static unsigned int buf_proxy_phy_local(unsigned int buf_id) { return buf_reg(buf_id, BUF_PROXY_PHY_LOCAL); }

    static void buf_msg_type(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_MSG_TYPE, val); }
    static unsigned int buf_msg_type(unsigned int buf_id) { return buf_reg(buf_id, BUF_MSG_TYPE); }

    static void buf_instance_id(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_INSTANCE_ID, val); }
    static unsigned int buf_instance_id(unsigned int buf_id) { return buf_reg(buf_id, BUF_INSTANCE_ID); }

    static void buf_type_id(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_TYPE_ID, val); }
    static unsigned int buf_type_id(unsigned int buf_id) { return buf_reg(buf_id, BUF_TYPE_ID); }

    static void buf_data_tx(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_DATA, val); }
    static unsigned int buf_data_rx(unsigned int buf_id) { return buf_reg(buf_id, BUF_DATA); }

    static void buf_tx(unsigned int buf_id, bool val){ buf_reg(buf_id, BUF_TX, val); }
    static bool buf_tx(unsigned int buf_id) { return buf_reg(buf_id, BUF_TX); }

    static void buf_rx(unsigned int buf_id, bool val){ buf_reg(buf_id, BUF_RX, val); }
    static bool buf_rx(unsigned int buf_id) { return buf_reg(buf_id, BUF_RX); }

    static void buf_agent_disp_addr(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_AGENT_DISP_ADDR, val); }
    static unsigned int buf_agent_disp_addr(unsigned int buf_id) { return buf_reg(buf_id, BUF_AGENT_DISP_ADDR); }

    static void buf_agent_disp_obj_addr(unsigned int buf_id, unsigned int val){ buf_reg(buf_id, BUF_AGENT_DISP_OBJ_ADDR, val); }
    static unsigned int buf_agent_disp_obj_addr(unsigned int buf_id) { return buf_reg(buf_id, BUF_AGENT_DISP_OBJ_ADDR); }

    static unsigned int ctrl_reg(unsigned int reg) {
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(ADDR_BASE_CTRL | reg);
        return *aux;
    }

    static void ctrl_reg(unsigned int reg, unsigned int data) {
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(ADDR_BASE_CTRL | reg);
        *aux = data;
    }

    static unsigned int buf_reg(unsigned int buf_id, unsigned int reg) {
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(ADDR_BASE_BUF | reg | (buf_id<<2));
        return *aux;
    }

    static void buf_reg(unsigned int buf_id, unsigned int reg, unsigned int data) {
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(ADDR_BASE_BUF | reg | (buf_id<<2));
        *aux = data;
    }
};

__END_SYS

#endif
