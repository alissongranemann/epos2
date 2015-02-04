#ifndef __handle_sw_h
#define __handle_sw_h

#include <utility/spin.h>
#include <component_manager.h>
#include "../../../unified/framework/proxy.h"
#include "../../../unified/framework/scenario_adapter.h"

using EPOS::Component_Manager;
using EPOS::Spin;

namespace Implementation {

template<typename Component>
class Handle_Common<Component, Configurations::EPOS_SOC_Catapult, false>
{
public:
    Handle_Common(Channel_t &rx_ch, Channel_t &tx_ch) {
        // A static instance ID implies that it will be shared by all
        // different types but it's better than hard coded IDs
        // TODO: Possible race condition!
        static unsigned int inst_id = 0;

        _inst_id = inst_id++;

        // We keep a Scenario_Adapter and Proxy in order to manage the Component
        // in both hardware and software domains
        _adapter = new Scenario_Adapter<Component>(rx_ch, tx_ch, _inst_id);
        _proxy = new Proxy<Component>(rx_ch, tx_ch, _inst_id);

        // TODO: Which should be the initial domain of the component?
        _domain = Component_Manager::HARDWARE;
    }

    ~Handle_Common() {
        delete _adapter;
        delete _proxy;
    }

protected:
    void enter_recfg() {
        _spin.acquire();

        get_state();

        // TODO: Speed hack! hehehe :)
        delete _proxy;

        // TODO: Can we remove these damn channels?
        Channel_t rx_ch;
        Channel_t tx_ch;
        _proxy = new Proxy<Component>(rx_ch, tx_ch, _inst_id);

        set_state();

        _domain = Component_Manager::HARDWARE;
    }

    void leave_recfg() { _spin.release(); }

protected:
    Scenario_Adapter<Component> * _adapter;
    Proxy<Component> * _proxy;
    Component_Manager::Domain _domain;

private:
    bool get_state() {
        return true;
    }

    bool set_state() {
        return true;
    }

private:
    Spin _spin;
    unsigned int _inst_id;
};

};

#endif
