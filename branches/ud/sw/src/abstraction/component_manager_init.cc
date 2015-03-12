// EPOS Component Manager Abstraction Initialization

#include <system.h>
#include <component_manager.h>

__BEGIN_SYS

Component_Manager::Buffer * Component_Manager::_nodes[];

template <unsigned int UNIT>
void Component_Manager::init_buffer()
{
    typedef typename Traits<Component_Manager>::Node<UNIT> NODE;

    Id id(NODE::TYPE_ID, NODE::UNIT_ID);
    Address addr(NODE::X, NODE::Y, NODE::LOCAL);

    // TODO: Must check if it's a proxy or agent
    _nodes[UNIT] = new (SYSTEM) Buffer(id, addr);

    init_buffer<UNIT + 1>();
};

template <>
void Component_Manager::init_buffer<Traits<Component_Manager>::UNITS>()
{
};

void Component_Manager::init() {
    db<Init, Component_Manager>(TRC) << "Component_Manager::init()" << endl;

    init_buffer<0>();

    // TODO: Is this really needed?
    Component_Controller::enable_agent_receive_int(&int_handler);
}

__END_SYS
