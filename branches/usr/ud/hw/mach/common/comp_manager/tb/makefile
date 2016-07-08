# Path to design flow scripts
BASE_DIR	:= $(abspath ../../../..)
TOOLS_DIR	:= $(BASE_DIR)/hw/tools/vivado

# Output files directories
BUILD_DIR	:= $(abspath build)
LOG_DIR		:= $(BUILD_DIR)/log
BIT_DIR		:= $(BUILD_DIR)/bit
CHECKPOINT_DIR	:= $(BUILD_DIR)/checkpoint
SYNTH_DIR	:= $(CHECKPOINT_DIR)/synth
ROUTE_DIR	:= $(CHECKPOINT_DIR)/route

# Log directories
COMMAND_LOG	:= $(LOG_DIR)/command.log
CRITICAL_LOG	:= $(LOG_DIR)/critical.log

# Vivado command line options
VIVADO_OPTS	:= -nolog -nojournal -notrace -mode batch

# XMD command line options
XMD_OPTS	:= -tcl

# promgen command line options
PROMGEN_OPTS	:= -b -w -p bin -data_width 32 -u 0

# Targets
ROUTE_STATIC		:= $(ROUTE_DIR)/static/static.dcp
ROUTE_RECFG		:= $(foreach PART,$(notdir $(RECFG_PARTS)),$(ROUTE_DIR)/$(PART))
ROUTE_RECFG		:= $(foreach COMP,$(RECFG_COMPS),$(addsuffix /$(COMP).dcp,$(ROUTE_RECFG)))
SYNTH_STATIC		:= $(SYNTH_DIR)/static.dcp
SYNTH_RECFG_COMP	:= $(foreach COMP,$(RECFG_COMPS),$(addsuffix /$(COMP).dcp,$(SYNTH_DIR)))
BIT_STATIC		:= $(BIT_DIR)/static/static.bit
BIN_RECFG		:= $(foreach PART,$(notdir $(RECFG_PARTS)),$(BIT_DIR)/$(PART))
BIN_RECFG		:= $(foreach COMP,$(RECFG_COMPS),$(addsuffix /$(COMP).bin,$(BIN_RECFG)))

# Helper variables
RECFG_COMP_SRCS		= $$($$(notdir $$(basename $$@))_SRCS)
RECFG_COMP_TOP		= $(notdir $(basename $@))
RECFG_COMP_GENERICS	= $($*_GENERICS)
PART_TOP		= $(notdir $(patsubst %/,%,$(dir $@)))

all:	sim

parse:	$(VHDL_SRCS) $(VERILOG_SRCS)
	xvhdl $(VHDL_SRCS)
	xvlog $(VERILOG_SRCS)

elab:	parse


prog:	bit
	@vivado $(VIVADO_OPTS) -source $(TOOLS_DIR)/prog.tcl -tclargs $(BIT_STATIC)
	@xmd $(XMD_OPTS) $(TOOLS_DIR)/prog_ram.tcl "$(BIN_RECFG)"

$(BIT_STATIC):	$(ROUTE_STATIC)
	@vivado $(VIVADO_OPTS) -source $(TOOLS_DIR)/gen_bit_static.tcl -tclargs "$?"

clean:
	$(RM) -r $(BUILD_DIR) .Xil .data .srcs *.log *.mif *.os

.PHONY:	all bit route synth hls prog hls_clean veryclean clean

.EXPORT_ALL_VARIABLES:
