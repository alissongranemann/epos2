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
RECFG_PARTS	:= u_recfg_node_0

# adpcm_codec sources
HLS_COMPONENTS		+= adpcm_codec
include ../../../components/catapult/adpcm_codec/hw.srcs
adpcm_codec_SRCS	:= $(addprefix ../../../components/catapult/adpcm_codec/,$(SRCS))
adpcm_codec_GENERICS	:= "SIZE_X=1 SIZE_Y=1 SIZE_DATA=56 RMI_MSG_SIZE=80"

RECFG_COMPS	= $(HLS_COMPONENTS)
BIN_RECFG_ADDR	:= 0x01000000

# Project properties
DEVICE		:= xc7z020
PACKAGE		:= clg484
SPEED		:= -1
TARGET_LANGUAGE	:= VHDL

include ../../../tools/vivado/Makefile.common
