INSTALL		= install
INSTALL_PROG	= $(INSTALL) -m0755
INSTALL_DIR	= $(INSTALL) -d -m0755

XEN_ROOT=../..
include $(XEN_ROOT)/tools/Rules.mk

CFLAGS   += -Werror

INCLUDES += -I $(XEN_XC)
INCLUDES += -I $(XEN_LIBXC)
CFLAGS   += $(INCLUDES)

HDRS     = $(wildcard *.h)

INSTALL_SBIN = xm xend

all: build
build: $(TARGETS)

install: build
	[ -d $(DESTDIR)/usr/bin ] || $(INSTALL_DIR) $(DESTDIR)/usr/bin
	[ -d $(DESTDIR)/usr/sbin ] || $(INSTALL_DIR) $(DESTDIR)/usr/sbin
	$(INSTALL_PROG) $(INSTALL_SBIN) $(DESTDIR)/usr/sbin

clean:
	$(RM) *.o $(TARGETS) *~

