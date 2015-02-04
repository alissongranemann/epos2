// EPOS EPOSSOC Device Common Package Declarations

#ifndef __epossoc_device_h
#define __epossoc_device_h

#include <system/config.h>
#include <utility/list.h>

__BEGIN_SYS

class EPOSSOC_Device
{
private:
    typedef Simple_List<EPOSSOC_Device> List;

public:

    EPOSSOC_Device(const Type_Id & type, unsigned int unit, void * dev,
              unsigned int interrupt = ~0U)
        : _type(type), _unit(unit), _object(dev), _interrupt(interrupt),
          _busy(false), _link(this) { 
        _devices.insert(&_link);
    }

    ~EPOSSOC_Device() { _devices.remove(&_link); }

    void * object() { return _object; }

    static void * seize(const Type_Id & type, unsigned int unit) {
        EPOSSOC_Device * dev = get(type, unit);
        if(!dev) {
            db<EPOSSOC>(WRN) << "EPOSSOC_Device::seize: device not found\n";
            return 0;
        }
        if(dev->_busy) {
            db<EPOSSOC>(WRN) << "EPOSSOC_Device::seize: device busy\n";
            return 0;
        }
        dev->_busy = true;

        db<EPOSSOC>(TRC) << "EPOSSOC_Device::seize(type=" << type << ",unit=" << unit
                    << ") => " << dev << "\n";

        return dev->_object;
    }

    static void release(const Type_Id & type, unsigned int unit) {
        EPOSSOC_Device * dev = get(type, unit);
        if(!dev)
            db<EPOSSOC>(WRN) << "EPOSSOC_Device::release: device not found\n";
        dev->_busy = false; 
    }

    static EPOSSOC_Device * get(const Type_Id & type, unsigned int unit) {
        List::Element * e = _devices.head();
        for(; e && ((e->object()->_type != type) ||
                (e->object()->_unit != unit)); e = e->next());
        return e->object();

    }

    static EPOSSOC_Device * get(unsigned int interrupt) {
        List::Element * e = _devices.head();
        for(; e && (e->object()->_interrupt != interrupt); e = e->next());
        return e->object();
    }

    static void install_handler(unsigned int interrupt);

private:
    Type_Id _type;
    unsigned int _unit;
    void * _object;
    unsigned int _interrupt;
    bool _busy;
    List::Element _link;

    static List _devices;
};

__END_SYS

#endif
