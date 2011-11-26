XEN_ROOT = ../
include $(XEN_ROOT)/tools/Rules.mk

#SUBDIRS :=
#SUBDIRS += libxc
#SUBDIRS += xenstore
#SUBDIRS += misc
#SUBDIRS += examples
#SUBDIRS += xcutils
#SUBDIRS += console

.PHONY: all install clean check check_clean

all: check
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done

install: check
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done

clean: check_clean
	@set -e; for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done

check:

check_clean:

