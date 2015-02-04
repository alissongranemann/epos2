#ifndef __mips32_traits_h
#define __mips32_traits_h

__BEGIN_SYS

// MIPS32
template <> struct Traits<MIPS32>: public Traits<void>
{
    enum {LITTLE, BIG};
    static const unsigned int ENDIANESS         = BIG;
    static const unsigned int WORD_SIZE         = 32;
    static const unsigned int CLOCK             = 50000000;
    static const bool unaligned_memory_access   = false;
};

template <> struct Traits<MIPS32_TSC>: public Traits<void>
{
    // This should map the address of the TSC counter on the respective machine.
    // Default values are 0x20000060 (Plasma), 0x80000800 (EPOSSoC).
	static const unsigned int ADDRESS = 0x80000800;
};

template <> struct Traits<MIPS32_MMU>: public Traits<void>
{
};

__END_SYS

#endif
