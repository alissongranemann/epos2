// EPOS PC Multiprocessor (MP) Declarations

#ifndef __pc_mp_h
#define __pc_mp_h

#include <cpu.h>

__BEGIN_SYS

/*! Some structures speficied by Intel MultiProcessor Specification v1.4 of May, 1997. */
struct MP_Floating_Point
{
private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg32 Reg32;

public:
    friend Debug & operator<<(Debug & db, const MP_Floating_Point & mpfp)
    {
        db << "{";
        db << "signature: " << reinterpret_cast<const char *>(mpfp.signature) << ", ";
        db << "mp_configuration_table: " << reinterpret_cast<const char *>(mpfp.mp_configuration_table) << ", ";
        db << "length: " << mpfp.length << ", ";
        db << "version: " << mpfp.version << ", ";
        db << "checksum: " << reinterpret_cast<void *>(mpfp.checksum) << ", ";
        db << "features_1: " << reinterpret_cast<void *>(mpfp.features_1) << ", ";
        db << "features_2: " << reinterpret_cast<void *>(mpfp.features_2) << ", ";
        db << "features_3: " << reinterpret_cast<void *>(mpfp.features_3_to_5[0]) << ", ";
        db << "features_4: " << reinterpret_cast<void *>(mpfp.features_3_to_5[1]) << ", ";
        db << "features_5: " << reinterpret_cast<void *>(mpfp.features_3_to_5[2]);
        db << "}";
        return db;
    }

public:
    // The Signature, must contain _MP_, and is present on a 16 byte boundary.
    Reg8  signature[4];

    // The address of the MP Configuration Table.
    Reg32 mp_configuration_table;

    // The length of the floating point structure table, in 16 byte units. This field *should* contain 0x01, meaning 16-bytes.
    Reg8  length;

    // The version number of the MP Specification. A value of 1 indicates 1.1, 4 indicates 1.4, and so on.
    Reg8  version;

    // The checksum of the Floating Point Structure.
    Reg8  checksum;

    // Few feature bytes.
    Reg8 features_1;
    Reg8 features_2;
    Reg8 features_3_to_5[3];
};

struct MP_Configuration_Table
{
};

class MP
{
private:
    typedef CPU::Reg32 Reg32;

public:
    /*! It supposed to look at three places:
     *  (1) The first kilobyte of the Extended BIOS Data Area (EBDA).
     *  (2) The last kilobyte of base memory (639-640k).
     *  (3) The BIOS ROM address space (0xF0000-0xFFFFF).
     *
     *  Currently it looks only at (3).
     * */
    static bool find_mp_floating_point(Reg32 & mpfp_address)
    {
        // (1) The first kilobyte of the Extended BIOS Data Area (EBDA).
        db<void>(WRN) << "1: " << endl;

        // (2) The last kilobyte of base memory (639-640k).
        db<void>(WRN) << "2: " << endl;

        // (3) The BIOS ROM address space (0xF0000-0xFFFFF).
        db<void>(WRN) << "3: " << endl;
        for (unsigned long where = 0xF0000; where < 0x100000; where += 16)
        {
            char * mpfp = reinterpret_cast<char *>(where);
            if (strncmp( mpfp, "_MP_", 4) == 0)
            {
                db<void>(WRN) << "MP Floating Pointer Structure found at: " << reinterpret_cast<void *>(where) << endl;
                mpfp_address = where;

                MP_Floating_Point * mpfp = reinterpret_cast<MP_Floating_Point *>(mpfp_address);
                db<void>(WRN) << "MPFP: " << *mpfp << endl;

                return true;
            }
        }

        db<void>(WRN) << "MP Floating Pointer structure NOT found :(" << endl;
        return false;
    }

    static void print_local_apic_address()
    {
        /// TODO
        Reg32 mpfp_address = 0x0;
        find_mp_floating_point(mpfp_address);

        MP_Floating_Point * mpfp = reinterpret_cast<MP_Floating_Point *>(mpfp_address);
        db<void>(WRN) << "MPFP: " << *mpfp << endl;
        // db<void>(WRN) << "MPFP features_1: " << reinterpret_cast<void *>(mpfp->features_1) << endl;

        // Reg32 addr;
        // db<void>(WRN) << "Local APIC phy addr: " << reinterpret_cast<void *>(addr) << endl;
    }
};

__END_SYS

#endif
