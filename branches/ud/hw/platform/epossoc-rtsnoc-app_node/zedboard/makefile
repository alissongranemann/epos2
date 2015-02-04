# Top level sources
STATIC_TOP	:= platform
TOP_SRCS	:= platform.vhd ps.tcl ../rsp.vhd
STATIC_SRCS 	:= $(TOP_SRCS)
STATIC_XDC 	:= platform.xdc

# Machine sources
include ../../../mach/epossoc/rtsnoc/hw.srcs
STATIC_SRCS	+= $(addprefix ../../../mach/epossoc/rtsnoc/, $(MACH_EPOSSOC_RTSNOC_SRCS))

# Reconfigurable partitions
RECFG_PARTS	:= u_rsp/u_recfg_node

# add sources
HLS_COMPONENTS	+= add

include ../../../components/catapult/add/hw.srcs
add_SRCS	:= $(addprefix ../../../components/catapult/add/,$(SRCS))
add_GENERICS	:= "X=0 Y=0 LOCAL_ADDR=7 SIZE_X=1 SIZE_Y=1 SIZE_DATA=56 \
	RMI_MSG_SIZE=80 IID_SIZE=8"

# adpcm_codec sources
HLS_COMPONENTS	+= adpcm_codec

include ../../../components/catapult/adpcm_codec/hw.srcs
adpcm_codec_SRCS	:= $(addprefix ../../../components/catapult/adpcm_codec/,$(SRCS))
adpcm_codec_GENERICS	:= "X=0 Y=0 LOCAL_ADDR=7 SIZE_X=1 SIZE_Y=1 SIZE_DATA=56 \
	RMI_MSG_SIZE=80 IID_SIZE=8"

RECFG_COMPONENTS	= $(HLS_COMPONENTS)

# Project properties
DEVICE		:= xc7z020
PACKAGE		:= clg484
SPEED		:= -1
TARGET_LANGUAGE	:= VHDL

include ../../../tools/vivado/Makefile.common
