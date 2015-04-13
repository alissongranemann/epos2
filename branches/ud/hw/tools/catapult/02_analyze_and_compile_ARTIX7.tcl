directive set -PRESERVE_STRUCTS true
directive set -OPT_CONST_MULTS -1
directive set -CLOCK_OVERHEAD 20.000000
directive set -UNROLL no
directive set -MEM_MAP_THRESHOLD 32
directive set -REGISTER_THRESHOLD 256
directive set -MERGEABLE true
directive set -SPECULATE true
directive set -COMPGRADE fast
directive set -PIPELINE_RAMP_UP true
directive set -OLD_SCHED false
directive set -DESIGN_GOAL area
directive set -ASSIGN_OVERHEAD 0
directive set -RESET_CLEARS_ALL_REGS true
directive set -SAFE_FSM false
directive set -NO_X_ASSIGNMENTS true
directive set -REG_MAX_FANOUT 0
directive set -FSM_ENCODING none
directive set -IDLE_SIGNAL {}
directive set -REGISTER_IDLE_SIGNAL false
go analyze
directive set -CLOCK_NAME clk
directive set -START_FLAG {}
directive set -DONE_FLAG {}
directive set -TRANSACTION_DONE_SIGNAL false
directive set -CLOCK_NAME clk
directive set -CLOCKS {clk {-CLOCK_PERIOD 10.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 5.0 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_NAME {} -ENABLE_ACTIVE high}}
directive set -TECHLIBS {{mgc_Xilinx-ARTIX-7-1_beh_psr.lib {{mgc_Xilinx-ARTIX-7-1_beh_psr part 7A100TFGG484}}} {ram_Xilinx-ARTIX-7-1_RAMDB.lib ram_Xilinx-ARTIX-7-1_RAMDB} {ram_Xilinx-ARTIX-7-1_PIPE.lib ram_Xilinx-ARTIX-7-1_PIPE} {ram_Xilinx-ARTIX-7-1_RAMSB.lib ram_Xilinx-ARTIX-7-1_RAMSB} {rom_Xilinx-ARTIX-7-1.lib rom_Xilinx-ARTIX-7-1} {rom_Xilinx-ARTIX-7-1_SYNC_regin.lib rom_Xilinx-ARTIX-7-1_SYNC_regin} {rom_Xilinx-ARTIX-7-1_SYNC_regout.lib rom_Xilinx-ARTIX-7-1_SYNC_regout}}
go compile

