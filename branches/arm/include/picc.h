// Identification cards - Contactless integrated circuit(s) cards - Proximity cards standard (ISO/IEC 14443-3)

#ifndef __picc_h
#define __picc_h

#include <cpu.h>

__BEGIN_SYS

// TODO: should this be called PICC or MIFARE?
class PICC
{
    typedef CPU::Reg8 Reg8;

public:
    // MIFARE constants
    enum MIFARE_Misc {
        MF_ACK       = 0xA,
        MF_KEY_SIZE  = 6,
    };

    // Commands sent to the MIFARE PICC (proximity integrated circuit card).
    // The commands used by the PCD (reader) to manage communication
    // with several PICCs (cards) (ISO 14443-3, Type A, section 6.4)
    enum PICC_Command {
        REQA      = 0x26,    // Request command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollisionor selection.
        WUPA      = 0x52,    // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY and prepare foranticollision or selection.
        CT        = 0x88,    // Cascade Tag. Used during anti collision.
        SEL_CL1   = 0x93,    // Anti collision/Select, Cascade Level 1
        SEL_CL2   = 0x95,    // Anti collision/Select, Cascade Level 2
        SEL_CL3   = 0x97,    // Anti collision/Select, Cascade Level 3
        HLTA      = 0x50,    // Halt command, Type A. Instructs an ACTIVE PICC to go to state HALT.
        // MIFARE Classic commands (http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9.1)
        // Use MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
        MF_AUTH_KEY_A  = 0x60,   // Perform authentication with Key A
        MF_AUTH_KEY_B  = 0x61,   // Perform authentication with Key B
        MF_READ        = 0x30,   // Reads one 16 byte block from the authenticated sector of the PICC.
        MF_WRITE       = 0xA0,   // Writes one 16 byte block to the authenticated sector of the PICC.
        MF_DECREMENT  = 0xC0,    // Decrements the contents of a block and stores the result in the internal data register.
        MF_INCREMENT  = 0xC1,    // Increments the contents of a block and stores the result in the internal data register.
        MF_RESTORE    = 0xC2,    // Reads the contents of a block into the internal data register.
        MF_TRANSFER  = 0xB0,     // Writes the contents of the internal data register to a block.
        // MIFARE Ultralight commands (http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 7.6)
        // The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
        PICC_CMD_UL_WRITE    = 0xA2    // Writes one 4 byte page to the PICC.
    };

    // PICC types
    enum PICC_Type {
        MIFARE_UL    = 0x00, // MIFARE Ultralight or Ultralight C
        NOT_COMPLETE = 0x04, // SAK of an incomplete UID
        MIFARE_1K    = 0x08, // MIFARE Classic - 1KB
        MIFARE_MINI  = 0x09, // MIFARE Classic - 320 bytes
        MIFARE_PLUS  = 0x11, // MIFARE Plus
        MIFARE_4K    = 0x18, // MIFARE Classic - 4KB
        ISO_14443_4  = 0x20, // PICC compliant with ISO/IEC 14443-4
        ISO_18092    = 0x40, // PICC compliant with ISO/IEC 18092
    };

    template<unsigned int UID_SIZE = 10>
    class UID
    {
    private:
        typename IF<(UID_SIZE == 4) || (UID_SIZE == 7) || (UID_SIZE == 10), bool, void>::Result UID_SIZE_CHECK; // UID_SIZE must be 4, 7, or 10

    public:
        UID() : _size(UID_SIZE) {}
        UID(const char * uid, Reg8 sak = 0) : _size(UID_SIZE), _sak(sak) {
            memcpy(_uid, uid, UID_SIZE);
        }

        PICC_Type type() const { return static_cast<PICC_Type>(_sak & 0x7F); }
        Reg8 size() const { return _size; }
        const Reg8 * uid() const { return _uid; }

        void sak(Reg8 s) { _sak = s; }
        void uid(const Reg8 * u) { memcpy(_uid, u, UID_SIZE); }

        friend Debug & operator<<(Debug & db, const UID & u) {
            db << "{s=" << u._size << ",u=";
            db << hex << u._uid[0];
            for(unsigned int i = 1; i < UID_SIZE; i++)
                db << "," << hex << u._uid[i];
            db << ",sa=" << u._sak;
            return db;
        }

        friend OStream & operator<<(OStream & os, const UID & u) {
            os << "{s=" << u._size << ",u=";
            os << hex << u._uid[0];
            for(unsigned int i = 1; i < UID_SIZE; i++)
                os << "," << hex << u._uid[i];
            os << ",sa=" << u._sak;
            return os;
        }

    private:
        Reg8 _size;
        Reg8 _uid[UID_SIZE];
        Reg8 _sak;
    }__attribute__((packed));

    template<unsigned int KEY_SIZE = 6>
    class Key {
    public:
        Key() : _size(KEY_SIZE) {}
        Key(const char * key) : _size(KEY_SIZE) {
            memcpy(_key, key, KEY_SIZE);
        }

        Reg8 size() const { return _size; }
        const Reg8 * key() const { return _key; }

        void key(const Reg8 * k) { memcpy(_key, k, KEY_SIZE); }

        friend Debug & operator<<(Debug & db, const Key & k) {
            db << "{s=" << k._size << ",k=";
            db << hex << k._key[0];
            for(unsigned int i = 1; i < KEY_SIZE; i++)
                db << "," << hex << k._key[i];
            return db;
        }

        friend OStream & operator<<(OStream & os, const Key & k) {
            os << "{s=" << k._size << ",k=";
            os << hex << k._key[0];
            for(unsigned int i = 1; i < KEY_SIZE; i++)
                os << "," << hex << k._key[i];
            return os;
        }

    private:
        unsigned int _size;
        Reg8 _key[KEY_SIZE];
    }__attribute__((packed));
};

__END_SYS

#endif
