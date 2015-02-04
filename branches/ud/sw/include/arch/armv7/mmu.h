// EPOS-- ARMV7 MMU Mediator Declarations

#ifndef __armv7_mmu_h
#define __armv7_mmu_h

#include <system/memory_map.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/debug.h>
#include <cpu.h>
#include <mmu.h>

__BEGIN_SYS
/* ARMv7 only has addressable memory in the first 1GB of the address space,
 * the remaining is allocated for memory mapped registers.
 * It is also a good idea to not map anything in the first 1MB.
 */
class ARMV7_MMU: public MMU_Common<12, 8, 12>
{
private:
    typedef Grouping_List<Frame> List;
    static const unsigned int PHY_MEM = Memory_Map<Machine>::PHY_MEM;
	static const unsigned int MMU_TABLE_ADDR = Traits<ARMV7_MMU>::MMU_TABLE_ADDR;

public:
	enum{
		//TTP L1 flags
		L1_ENTRY_SHIFT = 10,
		DOMAIN_SHIFT = 5,
		NS =		1<<3,
		L2_ENTRY =	1,
		//TTP L2 flags
		L2_ENTRY_SHIFT = 12,
		nG = 		1<<11,
		S = 		1<<10,
		APx = 		1<<9,
		TEX_SHIFT =	6,
		AP1 = 		3<<5,
		AP0 = 		3<<4,
		C = 		1<<3,//Write-through no Allocate.
		B = 		1<<2,//Write Back. If C==B==0, non-cacheable
		WRITE_BACK_NO_ALLOCATE = C|B,
		SMALL_PAGE =1<<1,
		XN = 		1,
	};
    // Page Flags
    class ARMV7_Flags
    {
    public:
        enum {
            PRE  = 0x001, // Presence (0=not-present, 1=present)
            RW   = 0x002, // Write (0=read-only, 1=read-write)
            USR  = 0x004, // Access Control (0=supervisor, 1=user)
            PWT  = 0x008, // Cache Mode (0=write-back, 1=write-through)
            PCD  = 0x010, // Cache Disable (0=cacheable, 1=non-cacheable)
            ACC  = 0x020, // Accessed (0=not-accessed, 1=accessed
            DRT  = 0x040, // Dirty (only for PTEs, 0=clean, 1=dirty)
            PS   = 0x080, // Page Size (for PDEs, 0=4KBytes, 1=4MBytes)
            GLB  = 0X100, // Global Page (0=local, 1=global)
            EX   = 0x200, // User Def. (0=non-executable, 1=executable)
            CT   = 0x400, // User Def. (0=non-contiguous, 1=contiguous) *
            IO   = 0x800, // User Def. (0=memory, 1=I/O) *
            APP  = (PRE | RW  | ACC | USR),
            SYS  = (PRE | RW  | ACC),
            PCI  = (SYS | PCD | IO),
            APIC = (SYS | PCD),
            VGA = (SYS | PCD),
            DMA  = (SYS | PCD | CT), // *


			//TTP L1 flags
			L1_ENTRY_SHIFT = 10,
			DOMAIN_SHIFT = 5,
			NS =		1<<3,
			L2_ENTRY =	1,
			//TTP L2 flags
			L2_ENTRY_SHIFT = 12,
			nG = 		1<<11,
			S = 		1<<10,
			APx = 		1<<9,
			TEX_SHIFT =	6,
			AP1 = 		3<<5,
			AP0 = 		3<<4,
			C = 		1<<3,//Write-through no Allocate.
			B = 		1<<2,//Write Back. If C==B==0, non-cacheable
			WRITE_BACK_NO_ALLOCATE = C|B,
			SMALL_PAGE =1<<1,
			XN = 		1,
        };

    public:
        ARMV7_Flags() {}
        ARMV7_Flags(const ARMV7_Flags & f) : _flags(f._flags) {}
        ARMV7_Flags(unsigned int f) : _flags(f) {}
        ARMV7_Flags(Flags f) : _flags(PRE | ACC |
        			     ((f & Flags::RW)  ? RW  : 0) |
        			     ((f & Flags::USR) ? USR : 0) |
        			     ((f & Flags::CWT) ? PWT : 0) |
        			     ((f & Flags::CD)  ? PCD : 0) |
        			     ((f & Flags::CT)  ? CT  : 0) |
        			     ((f & Flags::IO)  ? PCI : 0) ) {}

        operator unsigned int() const { return _flags; }

        friend OStream & operator << (OStream & db, ARMV7_Flags f) { db << (void *)f._flags; return db; }

    private:
        unsigned int _flags;
    };

	static void build_PTs()
	{
		// simple 1:1 mapping
		PT_Entry l2_addr = MMU_TABLE_ADDR + 16*1024; //l1 has 16kb
		PT_Entry * entry = (PT_Entry*)MMU_TABLE_ADDR;
		//(*_master)[0] = entry;
		unsigned const int size_mb = PHY_MEM/(1024*1024);
		unsigned const int pt_size = PT_ENTRIES*sizeof(PT_Entry);

		ARMV7_Flags flags(0<<DOMAIN_SHIFT | NS | L2_ENTRY);
		for(unsigned int i = 0; i < size_mb; ++i)//run over the 512mb
		{
			build_l2_PT((PT_Entry*)(l2_addr + i*pt_size), i*1024*1024);
			entry[i] = (l2_addr + i*pt_size) | flags;
		}
		//Map the next 512mb in such way that phy2log works
		for(unsigned int i = 0; i < size_mb; ++i)
			entry[i+size_mb] = (l2_addr + i*pt_size) | flags;

		//From 1GB to 4GB, the address space is reserved for
		//memory mapped registers. Map 1:1 then.
		l2_addr = MMU_TABLE_ADDR + PD_ENTRIES*sizeof(PD_Entry) + size_mb*pt_size;
		for(unsigned int i = 2*size_mb; i < PD_ENTRIES; ++i)
		{
			build_l2_PT((PT_Entry*)l2_addr, i*1024*1024);
			entry[i] = l2_addr | flags;
			l2_addr += pt_size;
		}
		//kout << "Last L2 PT location: "<<l2_addr<<":"<<l2_addr+pt_size-1<<endl;
		//attach pages to directory TODO
	}

	static inline void build_l2_PT(PT_Entry* entry, Phy_Addr pa)
	{
		ARMV7_Flags flags(AP1 | AP0 | SMALL_PAGE);
		for(unsigned int i = 0; i < PT_ENTRIES; ++i)
			entry[i] = (pa + i*sizeof(Page)) | flags;
	}
	static void phy2log(Phy_Addr phy)
	{
		PT_Entry* l1 = (PT_Entry*)MMU_TABLE_ADDR;
		PT_Entry* l2 = (PT_Entry*) (l1[directory(phy)] & (~((1<<DIRECTORY_SHIFT)-1)));
		//kout << "l2: " << l2 << endl;
		//kout << "page: " << page(phy) << endl;
		kout << "phy " << phy << " = log " << l2[page(phy)] + offset(phy) << endl;
	}


public:
	
	static void disable_domains();
	static void disable_access_flags();
	static void disable();
	static void set_ttbr();
	static unsigned int check_ttbr();
	static void enable();
	static void enable_caches();


	static bool status()//true=MMU active
	{
		unsigned int flag;
		ASMV("mrc p15, 0, %0, c1, c0, 0 \n"
			 :"=r"(flag) : :);
		return flag & 0x1;
	}





    //Dummy DMA_Buffer (AVR CPU's does not make DMA)
    class DMA_Buffer {};

    // Page Flags
    //typedef MMU_Common<12, 8, 12>::Flags ARMV7_Flags;

    // Page_Table
	//template <unsigned int PT_ENTRIES = PT_ENTRIES>
    class Page_Table
	{
		public:
			Page_Table() {}

			PT_Entry & operator[](unsigned int i) { return _entry[i]; }

			void map(int from, int to, ARMV7_Flags flags) {
				Phy_Addr * addr = alloc(to - from);
				if(addr)
					remap(addr, from, to, flags);
				else 
					for( ; from < to; from++)
						_entry[from] = alloc() | flags;
			}

			void map_contiguous(int from, int to, ARMV7_Flags flags) {
				remap(alloc(to - from), from, to, flags);
			}

			void remap(Phy_Addr addr, int from, int to, ARMV7_Flags flags) {
				addr = align_page(addr);
				for( ; from < to; from++) {
					_entry[from] = addr | flags;
					addr+= sizeof(Page);
				}
			}

			void unmap(int from, int to) {
				for( ; from < to; from++) {
					free(_entry[from]);
					_entry[from] = 0;
				}
			}

			friend Debug & operator << (Debug & db, Page_Table & pt) {
				db << "{\n";
				int brk = 0;
				for(unsigned int i = 0; i < PT_ENTRIES; i++)
					if(pt[i]) {
						db << "[" << i << "]=" << pt[i] << "  ";
						if(!(++brk % 4))
							db << "\n";
					}
				db << "\n}";
				return db;
			}

		private:
			PT_Entry _entry[PT_ENTRIES];
	};


	// Chunk (for Segment)
	class Chunk
	{
		public:
			Chunk() {}

			Chunk(unsigned int bytes, Flags flags)
				: _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(ARMV7_Flags(flags)), _pt(calloc(_pts)) {
					if(flags & ARMV7_Flags::CT)
						_pt->map_contiguous(_from, _to, _flags);
					else 
						_pt->map(_from, _to, _flags);
				}

			Chunk(Phy_Addr phy_addr, unsigned int bytes, Flags flags)
				: _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(ARMV7_Flags(flags)), _pt(calloc(_pts)) {
					_pt->remap(phy_addr, _from, _to, flags);
				}

			~Chunk() {
				if(!(_flags & ARMV7_Flags::IO)) {
					if(_flags & ARMV7_Flags::CT)
						free((*_pt)[_from], _to - _from);
					else
						for( ; _from < _to; _from++)
							free((*_pt)[_from]);
				}
				free(_pt, _pts);
			}

			unsigned int pts() const { return _pts; }
			ARMV7_Flags flags() const { return _flags; }
			Page_Table * pt() const { return _pt; }
			unsigned int size() const { return (_to - _from) * sizeof(Page); }

			Phy_Addr phy_address() const {
				return (_flags & ARMV7_Flags::CT) ?
					Phy_Addr(indexes((*_pt)[_from])) : Phy_Addr(false);
			}

			int resize(unsigned int amount) {
				if(_flags & ARMV7_Flags::CT)
					return 0;

				unsigned int pgs = pages(amount);

				unsigned int free_pgs = _pts * PT_ENTRIES - _to;
				if(free_pgs < pgs) { // resize _pt
					unsigned int pts = _pts + page_tables(pgs - free_pgs);
					Page_Table * pt = calloc(pts);
					memcpy(pt, _pt, _pts * sizeof(Page));
					free(_pt, _pts);
					_pt = pt;
					_pts = pts;
				}

				_pt->map(_to, _to + pgs, _flags);
				_to += pgs;

				return pgs * sizeof(Page);
			}

		private:
			unsigned int _from;
			unsigned int _to;
			unsigned int _pts;
			ARMV7_Flags _flags;
			Page_Table * _pt;
	};



    // Page Directory
    typedef Page_Table Page_Directory;

    // Directory (for Address_Space)
    class Directory 
    {
    public:
    Directory() {}
    Directory(Page_Directory * pd) {}
    ~Directory() {}
    
    Page_Table * pd() const { return 0; }

    void activate() { }

    Log_Addr attach(const Chunk & chunk) { return chunk.phy_address(); }
    Log_Addr attach(const Chunk & chunk, Log_Addr addr) {
        return (addr == chunk.phy_address())? addr : Log_Addr(false);
    }
    void detach(const Chunk & chunk) {}
    void detach(const Chunk & chunk, Log_Addr addr) {}

    Phy_Addr physical(Log_Addr addr) { return addr; }
    };

public:
    ARMV7_MMU() {}

    static void flush_tlb() {}
    static void flush_tlb(Log_Addr addr) {}

	/* In Tanenbaum's nomenclature, a page in virtual space, is
	 * simply called page. A page in physical space is called a frame.
	 * */
    static Phy_Addr alloc(unsigned int frames = 1)
	{
		Phy_Addr phy(false);
		if(frames) {
			List::Element * e = _free.search_decrementing(frames);
			if(e)
				phy = e->object() + e->size();
			else
				db<ARMV7_MMU>(WRN) << "ARMV7_MMU::alloc() failed!" << endl;
		}
		db<ARMV7_MMU>(TRC) << "ARMV7_MMU::alloc(frames=" << frames << ") => " << phy << endl;
		return phy;

	}
    static Phy_Addr calloc(unsigned int bytes = 1) {
    Phy_Addr phy = alloc(bytes);
    memset(phy, bytes, 0);
    return phy; 
    }
    static void free(Phy_Addr addr, int n = 1);

    static Page_Directory * volatile current() {
		return reinterpret_cast<Page_Directory * volatile>(CPU::pdp());
    }

    static Phy_Addr physical(Log_Addr addr) { return addr; }

    static void init();

    static unsigned int allocable() { return _free.head() ? _free.head()->size() : 0; }
	
private:
    static List _free;
	static Page_Directory * _master;
};

__END_SYS

#endif
