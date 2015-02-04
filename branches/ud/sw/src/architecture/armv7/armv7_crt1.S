.file "armv7_crt1.S"

.section .text
.align 4
.global _start
_start:
    bl _init

.align  4
.global __epos_library_app_entry
__epos_library_app_entry:
    b  main     /* Do not link! LR was loaded by Context::load() */
    bl  _fini
    bl  _exit
