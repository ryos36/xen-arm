#
# Grand Unified Makefile for Xen-ARM
#

# Export target architecture overrides to Xen and Linux sub-trees.
ifeq ($(XEN_TARGET_ARCH),arm)
ifndef XEN_TARGET_SUBARCH
$(error "Unknown platform")
endif
endif

ifneq ($(XEN_TARGET_ARCH),)
SUBARCH := $(subst x86_32,i386,$(XEN_TARGET_ARCH))
export XEN_TARGET_ARCH SUBARCH XEN_SYSTYPE
endif

# Default target must appear before any include lines
all: dist

XEN_ROOT=$(CURDIR)
export XEN_ROOT
include Config.mk

ifeq ($(XEN_TARGET_X86_PAE),y)
export pae=y
endif

.PHONY:	all dist install xen kernels tools world clean
.PHONY:	mkpatches mrproper kbuild kdelete kclean

# build and install everything into the standard system directories
install: install-xen install-kernels install-tools 

build: kernels
	$(MAKE) -C xen build
	$(MAKE) -C tools build

# The test target is for unit tests that can run without an installation.  Of
# course, many tests require a machine running Xen itself, and these are
# handled elsewhere.
test:
#	$(MAKE) -C tools/python test

# build and install everything into local dist directory
dist: DESTDIR=$(DISTDIR)/install
dist: dist-xen dist-kernels dist-tools 
	$(INSTALL_DIR) $(DISTDIR)/check
	$(INSTALL_DATA) ./COPYING $(DISTDIR)
	$(INSTALL_DATA) ./README $(DISTDIR)
	$(INSTALL_PROG) ./install.sh $(DISTDIR)
#	$(INSTALL_PROG) tools/check/chk tools/check/check_* $(DISTDIR)/check
dist-%: DESTDIR=$(DISTDIR)/install
dist-%: install-%
	@: # do nothing

# Legacy dist targets
xen: dist-xen
tools: dist-tools
kernels: dist-kernels

prep-kernels:
	for i in $(XKERNELS) ; do $(MAKE) $$i-prep || exit 1; done

install-xen:
	$(MAKE) -C xen install

install-tools:
	$(MAKE) -C tools install

install-kernels:
	for i in $(XKERNELS) ; do $(MAKE) $$i-install || exit 1; done

# Build all the various kernels and modules
kbuild: kernels

# Delete the kernel build trees entirely
kdelete:
	for i in $(XKERNELS) ; do $(MAKE) $$i-delete ; done

# Clean the kernel build trees
kclean:
	for i in $(XKERNELS) ; do $(MAKE) $$i-clean ; done

# Make patches from kernel sparse trees
mkpatches:
	for i in $(ALLSPARSETREES) ; do $(MAKE) $$i-xen.patch; done

# build xen, the tools, and a domain 0 plus unprivileged linux-xen images,
# and place them in the install directory. 'make install' should then
# copy them to the normal system directories
world: 
	$(MAKE) clean
	$(MAKE) kdelete
	$(MAKE) dist

# clean doesn't do a kclean
clean:: 
	$(MAKE) -C xen clean
	$(MAKE) -C tools clean

# clean, but blow away kernel build tree plus tarballs
distclean: clean
	rm -rf dist patches/tmp
	for i in $(ALLKERNELS) ; do $(MAKE) $$i-delete ; done
	for i in $(ALLSPARSETREES) ; do $(MAKE) $$i-mrproper ; done

# Linux name for GNU distclean
mrproper: distclean

install-logging: LOGGING=logging-0.4.9.2
install-logging:
	[ -f $(LOGGING).tar.gz ] || wget http://www.red-dove.com/$(LOGGING).tar.gz
	tar -zxf $(LOGGING).tar.gz
	cd $(LOGGING) && python setup.py install

# handy target to upgrade iptables (use rpm or apt-get in preference)
install-iptables:
	wget http://www.netfilter.org/files/iptables-1.2.11.tar.bz2
	tar -jxf iptables-1.2.11.tar.bz2
	$(MAKE) -C iptables-1.2.11 PREFIX= KERNEL_DIR=../linux-$(LINUX_VER)-xen0 install

menuconfig:
	$(MAKE) -C xen menuconfig

help:
	@echo 'Installation targets:'
	@echo '  install          - build and install everything'
	@echo '  install-xen      - build and install the Xen hypervisor'
	@echo '  install-tools    - build and install the control tools'
	@echo ''
	@echo 'Building targets:'
	@echo '  dist             - build and install everything into local dist directory'
	@echo '  world            - clean everything, delete guest kernel build'
	@echo '                     trees then make dist'
	@echo '  xen              - build and install Xen hypervisor'
	@echo '  tools            - build and install tools'
	@echo ''
	@echo 'Cleaning targets:'
	@echo '  clean            - clean the Xen, tools and docs (but not'
	@echo '                     guest kernel) trees'
	@echo ''

