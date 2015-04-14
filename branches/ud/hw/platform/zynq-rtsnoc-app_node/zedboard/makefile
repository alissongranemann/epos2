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
STATIC_SRCS	+= $(addprefix ../../../mach/common/rtsnoc_router/, $(SRCS))

include ../../../mach/common/comp_manager/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/common/comp_manager/, $(SRCS))

include ../../../mach/common/amba_wishbone/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/common/amba_wishbone/, $(SRCS))

include ../../../components/catapult/adder/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../components/catapult/adder/, $(SRCS))

HLS_COMPONENTS	:= adder	

# Project properties
DEVICE		:= xc7z020
PACKAGE		:= clg484
SPEED		:= -1
TARGET_LANGUAGE	:= VHDL

include ../../../tools/vivado/Makefile.common
