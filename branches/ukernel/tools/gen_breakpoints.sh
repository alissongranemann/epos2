#!/bin/bash
# gen_breakpoints - Creates a list of breakpoints for all functions in an ELF file.
# Input: non-stripped ELF file.

FILE="$1"

nm -C $FILE | egrep ' [TW] ' > ${FILE}_breakpoints.txt 
sed -i -e 's/^/# 0x/' -e 's/\# \(0x[0-9a-f]*\).*/\0\nbreak \*\1/' ${FILE}_breakpoints.txt

mv ${FILE}_breakpoints.txt .

