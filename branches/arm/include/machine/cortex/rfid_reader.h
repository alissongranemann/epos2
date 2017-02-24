// EPOS ARM Cortex RFID Reader Mediator Declarations

#ifndef __cortex_rfid_reader_h
#define __cortex_rfid_reader_h

#include <rfid_reader.h>
#include <machine.h>
#include <picc.h>

__BEGIN_SYS

// MFRC522 MIFARE Reader chip from NXP
class MFRC522: private Machine_Model
{
public:
    typedef PICC::UID<10> UID; // TODO
    typedef PICC::Key<6> Key; // TODO

    typedef char Block[16];

    void reset();
    bool card_present();
    void read_card(UID * uid);
    bool select(const UID & uid);
    void halt_card();
    unsigned int put(unsigned int block, Block data);
    unsigned int read(unsigned int block, Block data);
    bool authenticate(unsigned int block, const Key & key);

private:
    //static volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(FLASH_CTRL_BASE)[o / sizeof(Reg32)]; } // TODO
};

typedef MFRC522 RFID_Reader_Engine;


// MIFARE RFID Reader/Writer
class RFID_Reader: public RFID_Reader_Common, public PICC, private RFID_Reader_Engine
{
    typedef RFID_Reader_Engine Engine;

public:
    typedef Engine::UID UID; // TODO
    typedef Engine::Key Key; // TODO

    class Card
    {
        friend class RFID_Reader;

    public:
        Card() : _unit(units++) {}

        unsigned int unit() const { return _unit; }
        const UID & uid() const { return _uid; }

    private:
        ~Card(); // Call RFID_Reader.free(Card); instead
        UID _uid;
        unsigned int _unit;
        static unsigned int units;
    };

    Card * get() {
        Card * c = new (SYSTEM) Card;
        while(!ready_to_get());
        read_card(&(c->_uid));
        return c;
    }

    void free(Card * c) {
        if(select(c))
            halt_card();
        delete c;
    }

    unsigned int put(const Card * c, unsigned int block, Block data) {
        if(select(c))
            return Engine::put(block, data);
        return 0;
    }

    unsigned int read(const Card * c, unsigned int block, Block data) {
        if(select(c))
            return Engine::read(block, data);
        return 0;
    }

    bool authenticate(const Card * c, unsigned int block, const Key & key) {
        if(select(c))
            return Engine::authenticate(block, key);
        return 0;
    }

    void reset() { Engine::reset(); }

    bool ready_to_get() { return card_present(); }
    bool ready_to_put() { return card_present(); }

private:
    bool select(const Card * c) {
        bool ret = true;
        if(_selected_unit != c->unit()) {
            while(!ready_to_put());
            ret = Engine::select(c->uid());
            _selected_unit = ret ? c->unit() : -1;
        }
        return ret;
    }

    unsigned int _selected_unit;
};

__END_SYS

#endif
