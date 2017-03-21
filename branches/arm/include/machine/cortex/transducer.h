// EPOS ARM Cortex Smart Transducer Declarations

#ifndef __cortex_transducer_h
#define __cortex_transducer_h

#include <smart_data.h>

#include <keyboard.h>
#include <smart_plug.h>
#include <hydro_board.h>
#include <gpio.h>
#include <tstp.h>
#include <tsc.h>
#include <rfid_reader.h>
#include <persistent_storage.h>
#include <condition.h>
#include <spi.h>

__BEGIN_SYS

typedef TSTP::Region Region;
typedef TSTP::Coordinates Coordinates;

enum CUSTOM_UNITS
{
    DOOR = TSTP::Unit::DIR | 1,
};

class Keyboard_Sensor: public Keyboard
{
public:
    static const unsigned int UNIT = TSTP::Unit::Acceleration;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef Keyboard::Observer Observer;
    typedef Keyboard::Observed Observed;

public:
    Keyboard_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Keyboard_Sensor> * data) {
        if(ready_to_get())
            data->_value = get();
        else
            data->_value = -1;
    }

    static void actuate(unsigned int dev, Smart_Data<Keyboard_Sensor> * data, const Smart_Data<Keyboard_Sensor>::Value & command) {}
};

#ifdef __mmod_emote3__

class Water_Level_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Level_Sensor> * data) {
        data->_value = level(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Level_Sensor> * data, const Smart_Data<Water_Level_Sensor>::Value & command) {}
};

class Water_Turbidity_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Amount_of_Substance; // TODO
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data) {
        data->_value = turbidity(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data, const Smart_Data<Water_Turbidity_Sensor>::Value & command) {}
};

class Water_Flow_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = (TSTP::Unit::SI) | (TSTP::Unit::DIR) | ((4 + 3) * TSTP::Unit::M) | ((4 - 1) * TSTP::Unit::S); // m^3/s
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data) {
        data->_value = water_flow();
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data, const Smart_Data<Water_Flow_Sensor>::Value & command) {}
};

class Pluviometer: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::DIV | TSTP::Unit::Length; // TODO: we want mm, or mm/m^2
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void sense(unsigned int dev, Smart_Data<Pluviometer> * data) {
        data->_value = rain();
    }

    static void actuate(unsigned int dev, Smart_Data<Pluviometer> * data, const Smart_Data<Pluviometer>::Value & command) {}
};

class Current_Sensor: public Smart_Plug
{
public:
    static const unsigned int UNIT = TSTP::Unit::Current;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Smart_Plug::Observer Observer;
    typedef Smart_Plug::Observed Observed;

public:
    static void sense(unsigned int dev, Smart_Data<Current_Sensor> * data) {
        data->_value = current(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Current_Sensor> * data, const Smart_Data<Current_Sensor>::Value & command) {
        Smart_Plug::actuate(dev, command);
    }
};

class ADC_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Luminous_Intensity;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    ADC_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<ADC_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.read();
    }

    static void actuate(unsigned int dev, Smart_Data<ADC_Sensor> * data, const Smart_Data<ADC_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class Temperature_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Temperature;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    Temperature_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Temperature_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.read();
    }

    static void actuate(unsigned int dev, Smart_Data<Temperature_Sensor> * data, const Smart_Data<Temperature_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class GPIO_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::DIGITAL + 1;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

public:
    GPIO_Sensor() {}

    GPIO_Sensor(unsigned int dev, char port, unsigned int pin) {
        _dev[dev] = new GPIO(port, pin, GPIO::IN, GPIO::Pull::UP, &int_handler, GPIO::Edge::BOTH);
    }

    static void sense(unsigned int dev, Smart_Data<GPIO_Sensor> * data) {
        data->_value = _dev[dev]->get();
    }

    static void actuate(unsigned int dev, Smart_Data<GPIO_Sensor> * data, const Smart_Data<GPIO_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static void int_handler(const unsigned int & id) { notify(); }

    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
    static GPIO * _dev[32];
};

class Door_Sensor: public RFID_Reader
{
    static const unsigned int MAX_DEVICES = 8;
    static const unsigned int DOOR_OPEN_TIME = 30 * 1000000;

public:
    static const unsigned int UNIT = CUSTOM_UNITS::DOOR;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef RFID_Reader::Observer Observer;
    typedef RFID_Reader::Observed Observed;

public:
    class Data : public RFID_Reader::UID
    {
    public:
        enum Code {
            DENIED     = 0,
            OPEN_NOW   = 1 << 0,
            AUTHORIZED = 1 << 1,
        };

        Data() : _code(0) {}
        Data(unsigned int v) : RFID_Reader::UID(v >> 8), _code(v) {}
        Data(const RFID_Reader::UID & u, unsigned char code = 0) : RFID_Reader::UID(u), _code(code) {}

        operator unsigned int() const {
            unsigned int i = uid();
            return (i << 8) | _code;
        }

        unsigned char code() const { return _code; }
        void code(unsigned char c) { _code = c; }

        bool authorized() const { return _code & AUTHORIZED; }
        void authorized(bool a) { if(a) _code |= AUTHORIZED; else _code &= ~AUTHORIZED; }
        bool open() const { return _code & OPEN_NOW; }
        void open(bool a) { if(a) _code |= OPEN_NOW; else _code &= ~OPEN_NOW; }

    private:
        unsigned char _code;
    }__attribute__((packed));

    Door_Sensor(unsigned int dev, GPIO * door_gpio, GPIO * button, SPI * reader_spi, GPIO * reader_gpio0, GPIO * reader_gpio1)
    : RFID_Reader(reader_spi, reader_gpio0, reader_gpio1), _button_pressed(false), _door_control_gpio(door_gpio), _button(button),
      _door_control_condition(), _door_control_thread(&door_control, &_door_control_condition, door_gpio) {
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
        _door_control_gpio->set();

        _button->direction(GPIO::IN);
        _button->handler(&button_handler, GPIO::FALLING);
    }

    ~Door_Sensor() {
        _button->int_disable();
        for(unsigned int i = 0; i < MAX_DEVICES; i++)
            if(_dev[i] == this)
                _dev[i] = 0;
    }

    static void button_handler(const IC::Interrupt_Id & id) {
        for(unsigned int i = 0; i < MAX_DEVICES; i++) // TODO
            if(_dev[i])
                _dev[i]->button();
    }

    static void sense(unsigned int dev, Smart_Data<Door_Sensor> * data) {
        assert(dev < MAX_DEVICES);
        if(_dev[dev])
            _dev[dev]->sense(data);
    }

    static void actuate(unsigned int dev, Smart_Data<Door_Sensor> * data, const Smart_Data<Door_Sensor>::Value & command) {
        assert(dev < MAX_DEVICES);
        if(_dev[dev])
            _dev[dev]->actuate(data, command);
    }

private:
    static int door_control(Condition * condition, GPIO * gpio) {
        while(true) {
            condition->wait();
            gpio->set();
            Alarm::delay(DOOR_OPEN_TIME);
            gpio->clear();
        }
        return 0;
    }

    void button() {
        _button_pressed = true;
        _door_control_gpio->set();
        notify();
    }

    void sense(Smart_Data<Door_Sensor> * data) {
        if(_button_pressed) {
            _button_pressed = false;
            Data d(1, Data::Code::AUTHORIZED | Data::Code::OPEN_NOW);
            data->_value = d;
            _door_control_condition.signal();
        } else if(RFID_Reader::ready_to_get()) {
            RFID_Reader::UID id = RFID_Reader::get();
            Data d(id, code(id));
            data->_value = d;

            if(d.authorized())
                _door_control_condition.signal();
        } else
            data->_value = 0;
    }

    void actuate(Smart_Data<Door_Sensor> * data, const Smart_Data<Door_Sensor>::Value & command) {
        Data d = command;
        if(d.open())
            _door_control_condition.signal();

        unsigned int end = list_size();
        for(unsigned int i = 0; i < end; i++) {
            Data d2 = read(i);
            if(d.uid() == d2.uid()) {
                if(d.code() != d2.code())
                    update(d, i);
                return;
            }
        }

        if(d.authorized())
            push(d, end);
    }

    unsigned char code(const RFID_Reader::UID & u) {
        for(unsigned int i = 0; i < list_size(); i++) {
            Data d = read(i);
            if(d.uid() == u)
                return d.code();
        }

        return Data::Code::DENIED;
    }

    unsigned int list_size() {
        unsigned int ret;
        Persistent_Storage::read(0, &ret, sizeof(unsigned int));
        if(ret > FLASH_LIMIT) {
            ret = 0;
            Persistent_Storage::write(0, &ret, sizeof(unsigned int));
        }
        return ret;
    }

    Data read(unsigned int flash_block) {
        Data d;
        Persistent_Storage::read(sizeof(Persistent_Storage::Word) + flash_block * SIZE_ALIGNED, &d, sizeof(Data));
        return d;
    }

    void update(const Data & d, unsigned int flash_block) {
        Persistent_Storage::write(sizeof(Persistent_Storage::Word) + flash_block * SIZE_ALIGNED, &d, sizeof(Data));
    }

    unsigned int push(const Data & d, unsigned int flash_block) {
        bool pushed = true;
        unsigned int addr = sizeof(Persistent_Storage::Word) + flash_block * SIZE_ALIGNED;
        if(addr > FLASH_LIMIT) {
            flash_block = Random::random() % FLASH_LIMIT;
            addr = sizeof(Persistent_Storage::Word) + flash_block * SIZE_ALIGNED;
            pushed = false;
        }

        Persistent_Storage::write(addr, &d, sizeof(Data));

        if(pushed) {
            unsigned int b = flash_block + 1;
            Persistent_Storage::write(0, &b, sizeof(unsigned int));
        }

        return flash_block;
    }

private:
    static const unsigned int SIZE_ALIGNED = (((sizeof(Data) + sizeof(Persistent_Storage::Word) - 1) / sizeof(Persistent_Storage::Word)) * sizeof(Persistent_Storage::Word));
    static const unsigned int FLASH_LIMIT = Persistent_Storage::SIZE / SIZE_ALIGNED - 1;

private:
    bool _button_pressed;
    GPIO * _door_control_gpio;
    GPIO * _button;
    Condition _door_control_condition;
    Thread _door_control_thread;
    static Observed _observed;
    static Door_Sensor * _dev[MAX_DEVICES];
};

typedef Smart_Data<Current_Sensor> Current;
typedef Smart_Data<ADC_Sensor> Luminous_Intensity;
typedef Smart_Data<Temperature_Sensor> Temperature;
typedef Smart_Data<GPIO_Sensor> Presence;
typedef Smart_Data<Water_Flow_Sensor> Water_Flow;
typedef Smart_Data<Water_Level_Sensor> Water_Level;
typedef Smart_Data<Water_Turbidity_Sensor> Water_Turbidity;
typedef Smart_Data<Pluviometer> Rain;
typedef Smart_Data<Door_Sensor> Door;

#endif

typedef Smart_Data<Keyboard_Sensor> Acceleration;

__END_SYS

#endif
