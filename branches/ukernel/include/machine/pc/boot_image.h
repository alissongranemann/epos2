// EPOS PC Boot Image

#ifndef __pc_boot_image_h
#define __pc_boot_image_h

#include <system/boot_image.h>
#include <utility/elf.h>

__BEGIN_SYS


class PC_Boot_Image : public Boot_Image_Common /// <<singleton>>
{
private: // Copied from System_Info
    typedef unsigned int LAddr;
    typedef unsigned int PAddr;
    typedef unsigned int Size;

public:
    static PC_Boot_Image * self()
    {
        if (! _instance)
            _instance = new (SYSTEM) PC_Boot_Image();

        return _instance;
    }

private:
    PC_Boot_Image();

public:
    /// extra_elf
    /*! Every time it is invoked, this method
     * returns an ELF image belowing to the application extra segment.
     * When there are no more ELF images to return, this method returns 0 (null).
     */
    ELF * next_extra_elf();


private:
    Size elf_size(ELF * elf) /* NOTE: I suppose that an ELF file size is given 
                                by the sum of the size of its segments. */
    {
        Size elf_size = 0;
        
        for(int i = 0; i < elf->segments(); i++)
            elf_size += elf->segment_size(i);
 
        return elf_size;
    }

private:
    static PC_Boot_Image * _instance;

private:
    PAddr _app_extra;
    PAddr _app_extra_offset;
    Size  _app_extra_size;
};


__END_SYS

#endif
