# 
ifndef XENO_CONFIG_INCLUDED
XENO_DEBUG		= n

XEN_BASE_DIR		= `pwd`/../..

XENO_COMPILE_ARCH    	= $(shell uname -m | sed -e s/i.86/x86_32/)
XENO_TARGET_ARCH 	= arm

# Tools to run on system hosting the build
HOSTCC     		= gcc
HOSTCFLAGS 		= -Wall -Werror -Wstrict-prototypes -g -fomit-frame-pointer

#
# Cross Tool chain configuration
#
#CROSS_COMPILE		= arm_v4t_le-
CROSS_COMPILE		= /opt/arm-linux-4.1.1/bin/arm-linux-

#
# Target Configuration for Xeno-mini
#
#XENO_TARGET_SUBARCH ?= imx, omap
#XENO_TARGET_SUBARCH  	= omap
#XENO_TARGET_SUBARCH  	= imx
XENO_TARGET_SUBARCH	= fiore

AS         		= $(CROSS_COMPILE)as
LD         		= $(CROSS_COMPILE)ld
CC         		= $(CROSS_COMPILE)gcc
CPP        		= $(CROSS_COMPILE)gcc -E
AR         		= $(CROSS_COMPILE)ar
RANLIB     		= $(CROSS_COMPILE)ranlib
NM         		= $(CROSS_COMPILE)nm
STRIP      		= $(CROSS_COMPILE)strip
OBJCOPY    		= $(CROSS_COMPILE)objcopy
OBJDUMP    		= $(CROSS_COMPILE)objdump

CFLAGS 			+= -fno-builtin -Wall -Werror -Wredundant-decls -Wno-format -mapcs
CFLAGS 			+= -Wnested-externs -Wpointer-arith 
CFLAGS			+= -I$(CURDIR)/include -L$(CURDIR)/lib
CFLAGS			+= -D__XEN__ -D__KERNEL__ -DCONFIG_${DOMAIN_NUM}

ifeq ($(XENO_DEBUG),y)
	CFLAGS += -g
else
	CFLAGS += -g
endif

export XENO_COMPILE_ARCH XENO_TARGET_ARCH XENO_TARGET_SUBARCH XEN_BASE_DIR
export AS LD CC CPP AR RANLIB NM STRIP OBJCOPY OBJDUMP CFLAGS
export HOSTCC HOSTCFLAGS
export CFLAGS AFLAGS LDFLAGS

export XENO_START
export DOMAIN_NUM

XENO_CONFIG_INCLUDED := YES
export XENO_CONFIG_INCLUDED
endif
