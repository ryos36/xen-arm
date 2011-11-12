XEN_ROOT = ../
include $(XEN_ROOT)/tools/Rules.mk

SUBDIRS :=
SUBDIRS += libxc
SUBDIRS += xenstore
SUBDIRS += misc
SUBDIRS += examples
SUBDIRS += xcutils
SUBDIRS += console

.PHONY: all install clean check check_clean ioemu eioemuinstall ioemuclean

all: check
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done
	$(MAKE) ioemu

install: check
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done
	$(MAKE) ioemuinstall
	$(INSTALL_DIR) -p $(DESTDIR)/var/xen/dump

clean: check_clean
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done
	$(MAKE) ioemuclean

check:
#	$(MAKE) -C check

check_clean:
#	$(MAKE) -C check clean

#ifndef XEN_NO_IOEMU
#ioemu ioemuinstall ioemuclean:
#	[ -f ioemu/config-host.h ] || \
#	(cd ioemu; ./configure --prefix=usr)
#	$(MAKE) -C ioemu $(patsubst ioemu%,%,$@)
#else
ioemu ioemuinstall ioemuclean:
#endif

