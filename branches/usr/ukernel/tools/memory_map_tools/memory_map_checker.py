#! /usr/bin/python3

from common import TLB
from common import TLB_QEMU_Monitor_Parser
from common import Memory_Map
from memory_map_v4 import Memory_Map_V4_Master_Task

import sys

from termcolor import colored

class Memory_Map_Checker:

    def __init__(self, memory_map):
        self.__memory_map = memory_map


    def __check(self, tlb, map_entry, msg):
        if msg:
            print(msg)

        if map_entry[Memory_Map.PHYSICAL] != Memory_Map.UNMAPPED:
            try:
                tlb_entry = tlb[map_entry[Memory_Map.LOGICAL]]
            except KeyError:
                print(colored('TLB dos not map address: ' + map_entry[Memory_Map.LOGICAL], 'red'))
                return 1

            if map_entry[Memory_Map.PHYSICAL] != tlb_entry[TLB.PHYSICAL]:
                print(colored('Mapping Error', 'red'))
                print(colored('TLB maps\t\t' + map_entry[Memory_Map.LOGICAL] + ' => ' + tlb_entry[TLB.PHYSICAL], 'red'))
                print(colored('Memory map expects\t' + map_entry[Memory_Map.LOGICAL] + ' => ' + map_entry[Memory_Map.PHYSICAL], 'red'))
                return 1
        else:
            print(colored('skipping: ' + msg, 'yellow'))

        return 0


    def __check_range(self, tlb, map_entries, msg):
        print(msg)

        errors = 0

        for entry in map_entries:
            errors += self.__check(tlb, entry, '')

        return errors


    def check(self, tlb_file_path):
        parser = TLB_QEMU_Monitor_Parser()
        tlb = parser.parse(tlb_file_path)

        # print(tlb)

        errors = 0

        errors += self.__check(tlb, self.__memory_map.IO, 'checking IO...')
        errors += self.__check(tlb, self.__memory_map.LOCAL_APIC, 'checking Local APIC...')
        errors += self.__check(tlb, self.__memory_map.VGA, 'checking VGA...')
        errors += self.__check(tlb, self.__memory_map.PCI, 'checking PCI...')
        errors += self.__check(tlb, self.__memory_map.SYS, 'checking SYS...')
        errors += self.__check(tlb, self.__memory_map.IDT, 'checking IDT...')
        errors += self.__check(tlb, self.__memory_map.GDT, 'checking GDT...')
        errors += self.__check(tlb, self.__memory_map.SYS_PT, 'checking SYS_PT...')
        errors += self.__check(tlb, self.__memory_map.SYS_PD, 'checking SYS_PD...')
        errors += self.__check(tlb, self.__memory_map.SYS_INFO, 'checking SYS_INFO...')
        errors += self.__check(tlb, self.__memory_map.TSS0, 'checking TSS0...')
        errors += self.__check(tlb, self.__memory_map.TSS1, 'checking TSS1...')
        errors += self.__check(tlb, self.__memory_map.TSS2, 'checking TSS2...')
        errors += self.__check(tlb, self.__memory_map.TSS3, 'checking TSS3...')
        errors += self.__check(tlb, self.__memory_map.TSS4, 'checking TSS4...')
        errors += self.__check(tlb, self.__memory_map.TSS5, 'checking TSS5...')
        errors += self.__check(tlb, self.__memory_map.TSS6, 'checking TSS6...')
        errors += self.__check(tlb, self.__memory_map.TSS7, 'checking TSS7...')
        errors += self.__check(tlb, self.__memory_map.SYS_CODE, 'checking SYS_CODE...')
        errors += self.__check(tlb, self.__memory_map.SYS_SHARED, 'checking SYS_SHARED...')
        errors += self.__check(tlb, self.__memory_map.SYS_DATA, 'checking SYS_DATA...')
        errors += self.__check(tlb, self.__memory_map.SYS_STACK, 'checking SYS_STACK...')
        errors += self.__check(tlb, self.__memory_map.SYS_HEAP, 'checking SYS_HEAP...')

        errors += self.__check_range(tlb, self.__memory_map.low_logical, 'checking addresses in the range [' + self.__memory_map.low_logical[0][Memory_Map.LOGICAL] + ', ' + self.__memory_map.low_logical[-1][Memory_Map.LOGICAL] + ']...')
        errors += self.__check_range(tlb, self.__memory_map.med_logical, 'checking addresses in the range [' + self.__memory_map.med_logical[0][Memory_Map.LOGICAL] + ', ' + self.__memory_map.med_logical[-1][Memory_Map.LOGICAL] + ']...')

        if errors:
            if errors == 1:
                print(colored('Found ' + str(errors) + ' error.', 'red'))
            else:
                print(colored('Found ' + str(errors) + ' errors.', 'red'))
        else:
            print(colored('Memory mapping OK. No errors found.', 'green'))

def main():
    argv = sys.argv
    if len(argv) < 2:
        print('Usage:')
        print('./memory_map TLB_FILE')
        return

    tlb_file_path = argv[1]
    memory_map = Memory_Map_V4_Master_Task()

    print('No memory map specified. Using version: ' + memory_map.VERSION)

    checker = Memory_Map_Checker(memory_map)
    checker.check(tlb_file_path)


if __name__ == '__main__':
    main()
