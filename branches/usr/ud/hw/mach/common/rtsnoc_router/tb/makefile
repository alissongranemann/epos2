all: rtsnoc_to_wishbone_tb.iverilog

run: rtsnoc_to_wishbone_tb.iverilog
	./rtsnoc_to_wishbone_tb.iverilog -lxt2

rtsnoc_to_wishbone_tb.iverilog: rtsnoc_to_wishbone_tb.v ../rtsnoc_to_wishbone_master.v ../wishbone_slave_to_rtsnoc.v
	iverilog -W all -o rtsnoc_to_wishbone_tb.iverilog rtsnoc_to_wishbone_tb.v ../rtsnoc_to_wishbone_master.v ../wishbone_slave_to_rtsnoc.v
