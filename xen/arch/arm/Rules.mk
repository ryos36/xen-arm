########################################
# arm-specific definitions

#
# If you change any of these configuration options then you must
# 'make clean' before rebuilding.
#

ifeq ($(TARGET_SUBARCH),)
$(error "XEN_TARGET_SUBARCH must be supplied.")
endif

include $(BASEDIR)/arch/$(TARGET_ARCH)/$(TARGET_SUBARCH)/Rules.mk

CFLAGS  += -mabi=aapcs-linux -mno-thumb-interwork
CFLAGS  += -nostdinc -fno-builtin -fno-common -fno-strict-aliasing -mno-thumb-interwork
CFLAGS  += -iwithprefix include -Wno-pointer-arith -pipe
CFLAGS  += -I$(BASEDIR)/include -I$(BASEDIR)/include/security -I$(BASEDIR)/include/security/crypto
CFLAGS  += $(CFLAGS-y)



