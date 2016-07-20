from common import Memory_Map

class Memory_Map_V4_Master_Task:

    'Expected memory map for 3 CPUs at the beginning of PC_Setup::call_next'

    VERSION = '4 Master Task'

    def __init__(self):
        self.IO             =   ('00000000f0000000', '00000000fee00000')
        self.LOCAL_APIC     =   self.IO
        self.VGA            =   ('00000000f0001000', '00000000000b8000')
        self.PCI            =   ('00000000f0011000', '00000000fe000000')
        self.SYS            =   ('00000000ff400000', '000000000ffff000')
        self.IDT            =   self.SYS
        self.GDT            =   ('00000000ff401000', '000000000fffe000')
        self.SYS_PT         =   ('00000000ff402000', '000000000fffd000')
        self.SYS_PD         =   ('00000000ff403000', '000000000fffc000')
        self.SYS_INFO       =   ('00000000ff404000', '000000000fffb000')
        self.TSS0           =   ('00000000ff405000', '000000000fffa000')
        self.TSS1           =   ('00000000ff406000', '000000000fff9000')
        self.TSS2           =   ('00000000ff407000', '000000000fff8000')
        self.TSS3           =   ('00000000ff408000', Memory_Map.UNMAPPED) # Taking into account SMP = 3
        self.TSS4           =   ('00000000ff409000', Memory_Map.UNMAPPED) # Taking into account SMP = 3
        self.TSS5           =   ('00000000ff40a000', Memory_Map.UNMAPPED) # Taking into account SMP = 3
        self.TSS6           =   ('00000000ff40b000', Memory_Map.UNMAPPED) # Taking into account SMP = 3
        self.TSS7           =   ('00000000ff40c000', Memory_Map.UNMAPPED) # Taking into account SMP = 3
        self.SYS_CODE       =   ('00000000ff700000', '000000000ff9b000')
        self.SYS_SHARED     =   ('00000000ff740000', '000000000ff9a000')
        self.SYS_DATA       =   ('00000000ff741000', '000000000ff99000')
        self.SYS_STACK      =   ('00000000ff7c1000', '000000000ff8d000')
        self.SYS_HEAP       =   ('00000000ff801000', Memory_Map.UNMAPPED) # Taking into account CPU at PC_Setup::call_next
        self.low_logical    = self.__low_logical()
        self.med_logical    = self.__med_logical()

    def __low_logical(self):
        'Pages starting at logical addresses in the range: [0000000000000000, 000000000ffff000] (first 256 MB) are equal to their physical counterparts.'
        addresses = []

        for i in range(65536):
            logical = hex(i * 4096)
            logical = logical.replace('0x', '')
            logical = ((16 - len(logical)) * '0') + logical

            physical = logical

            flags = 'NOT SUPPORTED YET'

            addresses.append((logical, physical, flags))

        return addresses

    def __med_logical(self):
        """Pages starting at logical addresses in the range:
        [0000000080000000, 000000008ffff000] (from 2 GB to 2 GB + 256 MB)
        are equal 80000000 OR PHY of their physical counterparts.
        E.g. logical 8ffe4000 maps onto 0ffe4000 physical."""
        addresses = []

        for i in range(65536):
            logical = hex(i * 4096)
            logical = logical.replace('0x', '')
            logical = ((16 - len(logical)) * '0') + logical

            physical = logical

            logical = logical[0:8] + '8' + logical[9:]

            # print(logical + ' => ' + physical)

            flags = 'NOT SUPPORTED YET'

            addresses.append((logical, physical, flags))

        return addresses

