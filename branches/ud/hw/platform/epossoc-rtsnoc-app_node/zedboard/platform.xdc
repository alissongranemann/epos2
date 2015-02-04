# Reconfigurable partition
create_pblock pblock_1
set_property HD.RECONFIGURABLE true [get_cells u_rsp/u_recfg_node]
add_cells_to_pblock [get_pblocks pblock_1]  [get_cells -quiet [list u_rsp/u_recfg_node]]
resize_pblock [get_pblocks pblock_1] -add {SLICE_X32Y0:SLICE_X35Y49}
resize_pblock [get_pblocks pblock_1] -add {DSP48_X2Y0:DSP48_X2Y19}
resize_pblock [get_pblocks pblock_1] -add {RAMB18_X2Y0:RAMB18_X2Y19}
resize_pblock [get_pblocks pblock_1] -add {RAMB36_X2Y0:RAMB36_X2Y9}
set_property RESET_AFTER_RECONFIG 1 [get_pblocks pblock_1]

# LEDs
set_property PACKAGE_PIN T22 [get_ports gpio_leds[0]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[0]]
set_property PACKAGE_PIN T21 [get_ports gpio_leds[1]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[1]]
set_property PACKAGE_PIN U22 [get_ports gpio_leds[2]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[2]]
set_property PACKAGE_PIN U21 [get_ports gpio_leds[3]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[3]]
set_property PACKAGE_PIN V22 [get_ports gpio_leds[4]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[4]]
set_property PACKAGE_PIN W22 [get_ports gpio_leds[5]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[5]]
set_property PACKAGE_PIN U19 [get_ports gpio_leds[6]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[6]]
set_property PACKAGE_PIN U14 [get_ports gpio_leds[7]]
set_property IOSTANDARD LVCMOS33 [get_ports gpio_leds[7]]

set_property PACKAGE_PIN V18 [get_ports leds[0]]
set_property IOSTANDARD LVCMOS33 [get_ports leds[0]]
set_property PACKAGE_PIN V19 [get_ports leds[1]]
set_property IOSTANDARD LVCMOS33 [get_ports leds[1]]
set_property PACKAGE_PIN U20 [get_ports leds[2]]
set_property IOSTANDARD LVCMOS33 [get_ports leds[2]]
set_property PACKAGE_PIN V20 [get_ports leds[3]]
set_property IOSTANDARD LVCMOS33 [get_ports leds[3]]

# Switches
set_property PACKAGE_PIN F22 [get_ports switches[0]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[0]]
set_property PACKAGE_PIN G22 [get_ports switches[1]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[1]]
set_property PACKAGE_PIN H22 [get_ports switches[2]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[2]]
set_property PACKAGE_PIN F21 [get_ports switches[3]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[3]]
set_property PACKAGE_PIN H19 [get_ports switches[4]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[4]]
set_property PACKAGE_PIN H18 [get_ports switches[5]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[5]]
set_property PACKAGE_PIN H17 [get_ports switches[6]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[6]]
set_property PACKAGE_PIN M15 [get_ports switches[7]]
set_property IOSTANDARD LVCMOS18 [get_ports switches[7]]

# Buttons
set_property PACKAGE_PIN N15 [get_ports buttons[0]]
set_property IOSTANDARD LVCMOS18 [get_ports buttons[0]]
set_property PACKAGE_PIN R16 [get_ports buttons[1]]
set_property IOSTANDARD LVCMOS18 [get_ports buttons[1]]
set_property PACKAGE_PIN T18 [get_ports buttons[2]]
set_property IOSTANDARD LVCMOS18 [get_ports buttons[2]]
set_property PACKAGE_PIN R18 [get_ports buttons[3]]
set_property IOSTANDARD LVCMOS18 [get_ports buttons[3]]

# Reset
set_property PACKAGE_PIN P16 [get_ports rst]
set_property IOSTANDARD LVCMOS18 [get_ports rst]

# UART
set_property PACKAGE_PIN W8 [get_ports uart_tx]
set_property IOSTANDARD LVCMOS33 [get_ports uart_tx]
set_property PACKAGE_PIN V10 [get_ports uart_rx]
set_property IOSTANDARD LVCMOS33 [get_ports uart_rx]
