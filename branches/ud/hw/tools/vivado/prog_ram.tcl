source $::env(TOOLS_DIR)/log_utils.tcl
source $::env(TOOLS_DIR)/ps7_init.tcl

# Read script arguments
set addr $::env(BIN_RECFG_ADDR)

# Init Zynq's Processing System
connect arm hw
ps7_init
ps7_post_config

foreach bit_recfg $argv {
    command "dow -data $bit_recfg $addr"
    set addr [expr $addr + [file size $bit_recfg]]
}
