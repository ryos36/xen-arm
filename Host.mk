# -*- mode: Makefile; -*-

# A debug build of Xen and tools?
DEBUG	?= y

# Target architecture
# Supported architectures are:
#	arm
ARCH	?= arm

# Target platform
# Supported target platforms are:
#	tegra250
MACHINE	?= tegra250

# If ACM_SECURITY = y, then the access control module is compiled
# into Xen and the policy type can be set by the boot policy file
#        y - Build the Xen ACM framework
#        n - Do not build the Xen ACM framework
ACM_SECURITY	?= n

# If ACM_SECURITY = y and no boot policy file is installed,
# then the ACM defaults to the security policy set by
#ACM_DEFAULT_SECURITY_POLICY
# Supported models are:
#	ACM_NULL_POLICY
#	ACM_CHINESE_WALL_POLICY
#	ACM_SIMPLE_TYPE_ENFORCEMENT_POLICY
#	ACM_CHINESE_WALL_AND_SIMPLE_TYPE_ENFORCEMENT_POLICY
ACM_DEFAULT_SECURITY_POLICY ?= ACM_NULL_POLICY

# Optional components
XENSTAT_XENTOP ?= y

VTPM_TOOLS ?= n

XEN_COMPILE_ARCH	?= $(shell uname -m | sed -e s/i.86/x86_32/)
XEN_TARGET_ARCH		?= arm
XEN_GUEST_KERNEL	?= linux-xen

LINUX_ROOT		?=$(XEN_ROOT)/../guest-kernels/$(XEN_GUEST_KERNEL)

# Tools to run on system hosting the build
HOSTCC     = gcc
HOSTCFLAGS = -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

#
# Cross Tool chain configuration
#
TOOLCHAIN_PREFIX = /opt/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-

#
# Toolchain configuration
#
AS		= $(TOOLCHAIN_PREFIX)as
LD		= $(TOOLCHAIN_PREFIX)ld
CC		= $(TOOLCHAIN_PREFIX)gcc
CPP		= $(TOOLCHAIN_PREFIX)gcc -E
AR		= $(TOOLCHAIN_PREFIX)ar
RANLIB		= $(TOOLCHAIN_PREFIX)ranlib
NM		= $(TOOLCHAIN_PREFIX)nm
STRIP		= $(TOOLCHAIN_PREFIX)strip
OBJCOPY		= $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP		= $(TOOLCHAIN_PREFIX)objdump

DISTDIR		?= $(XEN_ROOT)/dist
DESTDIR		?= $(DISTDIR)/install

INSTALL		= install
INSTALL_DIR	= $(INSTALL) -d -m0755
INSTALL_DATA	= $(INSTALL) -m0644
INSTALL_PROG	= $(INSTALL) -m0755

ifneq ($(debug),y)
# Optimisation flags are overridable
CFLAGS		?= -O2 -fomit-frame-pointer
CFLAGS		+= -DNDEBUG
else
CFLAGS		?= -g -O2 -fomit-frame-pointer
endif

ifneq ($(EXTRA_PREFIX),)
EXTRA_INCLUDES += $(EXTRA_PREFIX)/include
EXTRA_LIB += $(EXTRA_PREFIX)/$(LIBDIR)
endif

HOSTCFLAGS	+= $(call test-gcc-flag,$(HOSTCC),-Wdeclaration-after-statement)
CFLAGS		+= $(call test-gcc-flag,$(CC),-Wdeclaration-after-statement)

LDFLAGS		+= $(foreach i, $(EXTRA_LIB), -L$(i)) 
CFLAGS		+= $(foreach i, $(EXTRA_INCLUDES), -I$(i))

test-gcc-flag	= $(shell $(1) -v --help 2>&1 | grep -q " $(2) " && echo $(2))

# cc-option: Check if compiler supports first option, else fall back to second.
# Usage: cflags-y += $(call cc-option,$(CC),-march=winchip-c6,-march=i586)
cc-option	= $(shell if test -z "`$(1) $(2) -S -o /dev/null -xc \
		  /dev/null 2>&1`"; then echo "$(2)"; else echo "$(3)"; fi ;)

# cc-option-add: Add an option to compilation flags, but only if supported.
# Usage: $(call cc-option-add CFLAGS,CC,-march=winchip-c6)
cc-option-add	= $(eval $(call cc-option-add-closure,$(1),$(2),$(3)))
define cc-option-add-closure
    ifneq ($$(call cc-option,$$($(2)),$(3),n),n)
            $(1) += $(3)
    endif
endef

# cc-ver: Check compiler is at least specified version. Return boolean 'y'/'n'.
# Usage: ifeq ($(call cc-ver,$(CC),0x030400),y)
cc-ver = $(shell if [ $$((`$(1) -dumpversion | awk -F. \
           '{ printf "0x%02x%02x%02x", $$1, $$2, $$3}'`)) -ge $$(($(2))) ]; \
           then echo y; else echo n; fi ;)

# cc-ver-check: Check compiler is at least specified version, else fail.
# Usage: $(call cc-ver-check,CC,0x030400,"Require at least gcc-3.4")
cc-ver-check = $(eval $(call cc-ver-check-closure,$(1),$(2),$(3)))
define cc-ver-check-closure
    ifeq ($$(call cc-ver,$$($(1)),$(2)),n)
            override $(1) = echo "*** FATAL BUILD ERROR: "$(3) >&2; exit 1;
            cc-option := n
    endif
endef

-include $(XEN_ROOT)/.config
