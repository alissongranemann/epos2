#include <system/boot_image.h>
#include <system/memory_map.h>

__BEGIN_SYS

PC_Boot_Image::PC_Boot_Image * PC_Boot_Image::_instance = 0;

PC_Boot_Image::PC_Boot_Image()
{
    System_Info<Machine> * si = reinterpret_cast<System_Info<Machine> *>(Memory_Map<Machine>::SYS_INFO);
    _app_extra = (si->lm.app_extra) | Memory_Map<PC>::PHY_MEM; /* NOTE: I am supossing that this is the address of the ELF file
                                      relative to the first extra application segment.
                                      See the end of PC_Setup::build_lm method at pc_setup.cc.  */

    _app_extra = _app_extra + 4; /* NOTE: I have no idea why, but the ELF is located a word after I thought it will be. */

    _app_extra_size = si->lm.app_extra_size;

    _app_extra_offset = 0;

    db<PC_Boot_Image>(TRC) << "PC:Boot_Image::PC_Boot_Image" << endl;
    db<PC_Boot_Image>(TRC) << "_app_extra: " << reinterpret_cast<void *>(_app_extra) << ", _app_extra_size: " << _app_extra_size << endl;
}


void dump_elf(ELF * elf)
{
    const unsigned int size = 512;
    db<PC_Boot_Image>(ERR) << "Dumping ELF (first " << size << " bytes)" << endl;
    int * buff = reinterpret_cast<int *>(elf);
    int aux;
    unsigned char byte;
    unsigned int printed = 0;

    for (unsigned int i = 0; i < size; i++) {
        aux = buff[i] & 0x000000ff;
        byte = aux;
        if (byte < 0xF)
            db<PC_Boot_Image>(ERR) << "0";
        db<PC_Boot_Image>(ERR) << hex << byte  << " ";
        printed ++;

        aux = buff[i] & 0x0000ff00;
        aux = aux >> 8;
        byte = aux;
        if (byte < 0xF)
            db<PC_Boot_Image>(ERR) << "0";
        db<PC_Boot_Image>(ERR) << hex << byte  << " ";
        printed ++;

        aux = buff[i] & 0x00ff0000;
        aux = aux >> 16;
        byte = aux;
        if (byte < 0xF)
            db<PC_Boot_Image>(ERR) << "0";
        db<PC_Boot_Image>(ERR) << hex << byte  << " ";
        printed ++;

        aux = buff[i] & 0xff000000;
        aux = aux >> 24;
        byte = aux;
        if (byte < 0xF)
            db<PC_Boot_Image>(ERR) << "0";
        db<PC_Boot_Image>(ERR) << hex << byte  << " ";
        printed ++;

        if (printed == 16) {
            printed = 0;
            db<PC_Boot_Image>(ERR) << endl;
        }
    }
}


ELF * PC_Boot_Image::next_extra_elf()
{
    db<PC_Boot_Image>(ERR) << "PC_Boot_Image::next_extra_elf" << endl;
    db<PC_Boot_Image>(ERR) << "_app_extra_offset (1): " << _app_extra_offset << endl;

    ELF * elf = 0;

    if (_app_extra + _app_extra_offset < _app_extra + _app_extra_size) {
        elf = reinterpret_cast<ELF *>(_app_extra + _app_extra_offset);
        
        if(!elf->valid()) {
            db<PC_Boot_Image>(ERR) << "ELF image at: " << reinterpret_cast<void *>(elf) << " is corrupted!" << endl;
            dump_elf(elf);
            return 0;
        }

        // _app_extra_offset += elf_size(elf);
        _app_extra_offset = 1223 + 5; // ISSUE #1: PC_Boot_Image::_app_extra_offset:> value is computed in a hardcoded way.

    } else {
        db<PC_Boot_Image>(ERR) << "Error: elf address trespass the boot image limits! Address: " << reinterpret_cast<void *>(_app_extra + _app_extra_offset) << " limit: " << reinterpret_cast<void *>(_app_extra + _app_extra_size) << endl;
    }
    db<PC_Boot_Image>(ERR) << "_app_extra_offset (2): " << _app_extra_offset << endl;

    ASM("next_extra_elf_return:");
    db<PC_Boot_Image>(ERR) << "...returning elf: " << reinterpret_cast<void *>(elf) << endl;
    // dump_elf(elf); // comment this later.

    return elf;
}

__END_SYS


