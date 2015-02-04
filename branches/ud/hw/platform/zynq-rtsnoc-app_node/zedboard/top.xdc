# u_recfg_node_0
create_pblock u_recfg_node_0
set_property HD.RECONFIGURABLE true [get_cells u_recfg_node_0]
add_cells_to_pblock [get_pblocks u_recfg_node_0]  [get_cells -quiet [list u_recfg_node_0]]
resize_pblock [get_pblocks u_recfg_node_0] -add {SLICE_X32Y0:SLICE_X35Y49}
resize_pblock [get_pblocks u_recfg_node_0] -add {DSP48_X2Y0:DSP48_X2Y19}
resize_pblock [get_pblocks u_recfg_node_0] -add {RAMB18_X2Y0:RAMB18_X2Y19}
resize_pblock [get_pblocks u_recfg_node_0] -add {RAMB36_X2Y0:RAMB36_X2Y9}
set_property RESET_AFTER_RECONFIG 1 [get_pblocks u_recfg_node_0]

# u_recfg_node_1
create_pblock u_recfg_node_1
set_property HD.RECONFIGURABLE true [get_cells u_recfg_node_1]
add_cells_to_pblock [get_pblocks u_recfg_node_1]  [get_cells -quiet [list u_recfg_node_1]]
resize_pblock [get_pblocks u_recfg_node_1] -add {SLICE_X32Y50:SLICE_X35Y99}
resize_pblock [get_pblocks u_recfg_node_1] -add {DSP48_X2Y20:DSP48_X2Y39}
resize_pblock [get_pblocks u_recfg_node_1] -add {RAMB18_X2Y20:RAMB18_X2Y39}
resize_pblock [get_pblocks u_recfg_node_1] -add {RAMB36_X2Y10:RAMB36_X2Y19}
set_property RESET_AFTER_RECONFIG 1 [get_pblocks u_recfg_node_1]

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
