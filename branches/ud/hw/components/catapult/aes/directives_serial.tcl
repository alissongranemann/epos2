//  Catapult University Version 2011a.41 (Production Release) Thu Apr  7 20:18:15 PDT 2011
//  
//  Copyright (c) Mentor Graphics Corporation, 1996-2011, All Rights Reserved.
//                       UNPUBLISHED, LICENSED SOFTWARE.
//            CONFIDENTIAL AND PROPRIETARY INFORMATION WHICH IS THE
//          PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS
//  
//  Running on Linux tiago@spectrum 3.2.0-29-generic #46-Ubuntu SMP Fri Jul 27 17:03:23 UTC 2012 x86_64
//  
//  Package information: SIFLIBS v17.0_1.1, HLS_PKGS v17.0_1.1, 
//                       DesignPad v2.78_0.0
//  
//  This version may only be used for academic purposes.  Some optimizations 
//  are disabled, so results obtained from this version may be sub-optimal.
//  
project new
solution file add ./top_level.cc -type C++
solution file add ../../../../../unified/components/src/aes.cc -type C++
directive set -REGISTER_IDLE_SIGNAL false
directive set -IDLE_SIGNAL {}
directive set -FSM_ENCODING none
directive set -REG_MAX_FANOUT 0
directive set -NO_X_ASSIGNMENTS true
directive set -SAFE_FSM false
directive set -RESET_CLEARS_ALL_REGS true
directive set -ASSIGN_OVERHEAD 0
directive set -DESIGN_GOAL area
directive set -OLD_SCHED false
directive set -PIPELINE_RAMP_UP true
directive set -COMPGRADE fast
directive set -SPECULATE true
directive set -MERGEABLE true
directive set -REGISTER_THRESHOLD 256
directive set -MEM_MAP_THRESHOLD 32
directive set -UNROLL no
directive set -CLOCK_OVERHEAD 20.000000
directive set -OPT_CONST_MULTS -1
directive set -PRESERVE_STRUCTS true
go analyze
directive set -TECHLIBS {{Xilinx_accel_VIRTEX-6-1.lib Xilinx_accel_VIRTEX-6-1} {mgc_Xilinx-VIRTEX-6-1_beh_psr.lib {{mgc_Xilinx-VIRTEX-6-1_beh_psr part 6VLX240TFF1156}}} {ram_Xilinx-VIRTEX-6-1_RAMDB.lib ram_Xilinx-VIRTEX-6-1_RAMDB} {ram_Xilinx-VIRTEX-6-1_PIPE.lib ram_Xilinx-VIRTEX-6-1_PIPE} {ram_Xilinx-VIRTEX-6-1_RAMSB.lib ram_Xilinx-VIRTEX-6-1_RAMSB} {rom_Xilinx-VIRTEX-6-1.lib rom_Xilinx-VIRTEX-6-1} {rom_Xilinx-VIRTEX-6-1_SYNC_regin.lib rom_Xilinx-VIRTEX-6-1_SYNC_regin} {rom_Xilinx-VIRTEX-6-1_SYNC_regout.lib rom_Xilinx-VIRTEX-6-1_SYNC_regout}}
directive set -CLOCK_NAME clk
directive set -TRANSACTION_DONE_SIGNAL false
directive set -DONE_FLAG {}
directive set -START_FLAG {}
directive set -CLOCKS {clk {-CLOCK_PERIOD 10.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 5.0 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_NAME {} -ENABLE_ACTIVE high}}
directive set -CLOCK_NAME clk
directive set -DESIGN_HIERARCHY AES_Node__FR39ac_channel__tm__21_Q2_8Catapult7RMI_MsgT1PUc
go compile
directive set /AES_Node/core -DESIGN_GOAL area
directive set /AES_Node -EFFORT_LEVEL high
directive set /AES_Node/rx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_in_wire_wait
directive set /AES_Node/tx_ch:rsc -MAP_TO_MODULE mgc_ioport.mgc_out_stdreg_wait
directive set /AES_Node/iid:rsc -MAP_TO_MODULE {[DirectInput]}
directive set /AES_Node/core/agent._expandKey:while -ITERATIONS 40
go architect
go extract
