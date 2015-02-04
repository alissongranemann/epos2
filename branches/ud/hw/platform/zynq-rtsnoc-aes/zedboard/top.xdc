# u_recfg_node_0
create_pblock u_recfg_node_0
set_property HD.RECONFIGURABLE true [get_cells u_recfg_node_0]
add_cells_to_pblock [get_pblocks u_recfg_node_0]  [get_cells -quiet [list u_recfg_node_0]]
resize_pblock [get_pblocks u_recfg_node_0] -add {SLICE_X50Y0:SLICE_X85Y49}
resize_pblock [get_pblocks u_recfg_node_0] -add {RAMB18_X3Y0:RAMB18_X3Y19}
resize_pblock [get_pblocks u_recfg_node_0] -add {RAMB36_X3Y0:RAMB36_X3Y9}
set_property RESET_AFTER_RECONFIG 1 [get_pblocks u_recfg_node_0]

# LEDs
set_property PACKAGE_PIN T22 [get_ports led[0]]
set_property IOSTANDARD LVCMOS33 [get_ports led[0]]
set_property PACKAGE_PIN T21 [get_ports led[1]]
set_property IOSTANDARD LVCMOS33 [get_ports led[1]]
set_property PACKAGE_PIN U22 [get_ports led[2]]
set_property IOSTANDARD LVCMOS33 [get_ports led[2]]
set_property PACKAGE_PIN U21 [get_ports led[3]]
set_property IOSTANDARD LVCMOS33 [get_ports led[3]]
set_property PACKAGE_PIN V22 [get_ports led[4]]
set_property IOSTANDARD LVCMOS33 [get_ports led[4]]
set_property PACKAGE_PIN W22 [get_ports led[5]]
set_property IOSTANDARD LVCMOS33 [get_ports led[5]]
set_property PACKAGE_PIN U19 [get_ports led[6]]
set_property IOSTANDARD LVCMOS33 [get_ports led[6]]
set_property PACKAGE_PIN U14 [get_ports led[7]]
set_property IOSTANDARD LVCMOS33 [get_ports led[7]]
