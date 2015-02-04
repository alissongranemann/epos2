#create work library
vlib work

#testbench files
sccom -I../../../../.. -g ../testbench.cc

#other soc files
vcom ../../../../../mach/common/coregen/ram_amba_128k.vhd
vcom ../../../../../mach/common/coregen/ram_amba_1024k.vhd

vlog ../../../../../mach/common/amba/amba_mux.v
vlog ../../../../../mach/common/amba/address_decoder.v
vlog ../../../../../mach/common/amba/priority_encoder.v
vlog ../../../../../mach/common/amba/axi4_reset_control.v
vcom ../../../../../mach/common/amba/axi4lite_dummy_master.vhd
vlog ../../../../../mach/common/amba/axi4lite_decoder.v

vlog ../../../../../mach/common/amba_wishbone/axi4lite_to_wishbone.v
vcom ../../../../../mach/common/amba_wishbone/wishbone_to_axi4lite.vhd

vlog ../../../../../mach/common/simple_uart/shortfifo.v
vlog ../../../../../mach/common/simple_uart/medfifo.v
vlog ../../../../../mach/common/simple_uart/simple_uart_rx.v
vlog ../../../../../mach/common/simple_uart/simple_uart_tx.v
vlog ../../../../../mach/common/simple_uart/simple_uart.v
vlog ../../../../../mach/common/simple_uart/simple_uart_axi4lite.v

vlog ../../../../../mach/common/gpio/very_simple_gpio.v
vlog ../../../../../mach/common/gpio/gpio_axi4lite.v

vlog ../../../../../mach/common/simple_pic/priority_enc.v
vlog ../../../../../mach/common/simple_pic/pic.v
vlog ../../../../../mach/common/simple_pic/pic_axi4lite.v

vlog ../../../../../mach/common/simple_timer/timer2.v
vlog ../../../../../mach/common/simple_timer/timer_axi4lite.v

vcom ../../../../../mach/common/rtsnoc_router/router/ARBITER_MACHINE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/COMPARE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/crossbar.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/FLOW_CONTROL.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/INPUT_INTERFACE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/OUTPUT_INTERFACE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/PIPELINE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/PRIORITY.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/QUEUE.vhd
vcom ../../../../../mach/common/rtsnoc_router/router/ROUTER.vhd
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_wishbone_proxy.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_axi4lite_proxy.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_axi4lite_reset.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_echo_sm.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_echo.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_to_wishbone_master.v
vlog ../../../../../mach/common/rtsnoc_router/wishbone_slave_to_rtsnoc.v
vlog ../../../../../mach/common/rtsnoc_router/axi4lite_slave_to_rtsnoc.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_to_axi4lite_master.v
vlog ../../../../../mach/common/rtsnoc_router/rtsnoc_to_achannel.v

vlog ../../../../../mach/common/catapult/rtl_mgc_ioport_v2001.v
vlog ../../../../../mach/common/catapult/rtl_mgc_ioport.v

#HLS
vlog ../../../../../framework/catapult/top_level/dummy_callee/hls/rtl.v
vlog ../../../../../framework/catapult/top_level/dummy_callee/rtl.v
vlog ../../../../../framework/catapult/top_level/dummy_caller/hls/rtl.v
vlog ../../../../../framework/catapult/top_level/dummy_caller/rtl.v

#Testbench node
vlog ../../parallel_bit_search.v
vlog ../../comp_manager.v
vlog ../../comp_manager_axi4lite.v
vlog ../../comp_manager_axi4lite_scmodel.v
vcom ../cpu_io_node_comp_manager_tb.vhd

#Top level
vcom ../unified_tests_comp_maneger_tb.vhd

#link systemc with modelsim
#sccom -L /usr/lib -l util -link
sccom -link 

#load/dump/run vhdl design
set StdArithNoWarnings 1
set NumericStdNoWarnings 1

#vsim -L unisim -L unisims_ver -L xilinxcorelib  work.virtual_platform
vsim -L unisim -L unisims_ver -L xilinxcorelib  work.Unified_Tests_Comp_Manager_tb

#vcd file trace_dec.vcd
vcd add -r -file trace_dec.vcd /*
#vcd add -r -file trace_dec.vcd /plasma_axi4lite_testbench/plasma/timer/*
#vcd add -r -file trace_dec.vcd /plasma_axi4lite_testbench/plasma/pic/*

set StdArithNoWarnings 1
set NumericStdNoWarnings 1

run -all
