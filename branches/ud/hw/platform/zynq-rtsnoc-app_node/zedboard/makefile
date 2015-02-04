# Top level sources
STATIC_TOP	:= top
TOP_SRCS	:= top.vhd
STATIC_SRCS	:= $(TOP_SRCS)
STATIC_XDC 	:= top.xdc

# Machine sources
include ../../../mach/zynq/axi4lite/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/zynq/axi4lite/, $(SRCS))

include ../../../mach/common/rst_sync/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/common/rst_sync/, $(SRCS))

include ../../../mach/common/amba/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/common/amba/, $(SRCS))

include ../../../mach/common/rtsnoc_router/hw.srcs
STATIC_SRCS += $(addprefix ../../../mach/common/rtsnoc_router/, $(SRCS))

include ../../../mach/common/comp_manager/hw.srcs
STATIC_SRCS += $(addprefix ../../../mach/common/comp_manager/, $(SRCS))

include ../../../mach/common/amba_wishbone/hw.srcs
STATIC_SRCS += $(addprefix ../../../mach/common/amba_wishbone/, $(SRCS))

# Reconfigurable partitions
RECFG_PARTS	:= u_recfg_node_0 u_recfg_node_1

# add sources
HLS_COMPONENTS	+= add
include ../../../components/catapult/add/hw.srcs
add_SRCS	:= $(addprefix ../../../components/catapult/add/,$(SRCS))
add_GENERICS	:= "SIZE_X=1 SIZE_Y=1 SIZE_DATA=56 RMI_MSG_SIZE=80"

# mult sources
HLS_COMPONENTS	+= mult
include ../../../components/catapult/mult/hw.srcs
mult_SRCS	:= $(addprefix ../../../components/catapult/mult/,$(SRCS))
mult_GENERICS	:= "SIZE_X=1 SIZE_Y=1 SIZE_DATA=56 RMI_MSG_SIZE=80"

RECFG_COMPS	= $(HLS_COMPONENTS)
BIN_RECFG_ADDR	:= 0x01000000

# Project properties
DEVICE		:= xc7z020
PACKAGE		:= clg484
SPEED		:= -1
TARGET_LANGUAGE	:= VHDL

include ../../../tools/vivado/Makefile.common
