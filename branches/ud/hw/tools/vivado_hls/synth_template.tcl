open_project prj
set_top <<name>>_top
add_files <<name>>.cc -cflags "-I../../../../hw/"
open_solution "solution1"
# TODO: Catapult technology mapping must be handled without user
# intervention in the Makefile
set_part {xc7z020clg484-1}
create_clock -period 10 -name default
set_directive_interface -mode ap_hs -register "<<name>>_top" rx_ch
set_directive_interface -mode ap_hs -register "<<name>>_top" tx_ch
set_directive_interface -mode ap_ctrl_none "<<name>>_top"
csynth_design
exit
