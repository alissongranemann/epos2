// EPOS ARM Cortex RFID Reader Mediator Implementation

#include <system/config.h>

#ifdef __RFID_READER_H

#include <machine/cortex/rfid_reader.h>

__BEGIN_SYS

// MFRC522 MIFARE Reader chip from NXP

// Class attributes

// Methods
void MFRC522::reset()
{
}

bool MFRC522::card_present()
{
    return false;
}

void MFRC522::read_card(UID * uid)
{
}

bool MFRC522::select(const UID & uid)
{
    return false;
}

void MFRC522::halt_card()
{
}

unsigned int MFRC522::put(unsigned int block, Block data)
{
    return 0;
}

unsigned int MFRC522::read(unsigned int block, Block data)
{
    return 0;
}

bool MFRC522::authenticate(unsigned int block, const Key & key)
{
    return false;
}

__END_SYS

#endif
