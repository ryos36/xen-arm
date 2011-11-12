INSTALL         = install
INSTALL_DIR     = $(INSTALL) -d -m0755
INSTALL_PROG    = $(INSTALL) -m0755
INSTALL_DATA    = $(INSTALL) -m0644

XEN_ROOT    = ../..
include $(XEN_ROOT)/tools/Rules.mk

all: build

INCLUDEDIR = $(XEN_ROOT)/tools/addons/python-2.4.3
INCLUDEDIR += -I$(XEN_ROOT)/tools/addons/python-2.4.3/Include

INCLUDEDIR += -I$(XEN_ROOT)/tools/libxc
INCLUDEDIR += -I$(XEN_ROOT)/tools/xenstore

LIBRARYDIR =  $(XEN_ROOT)/tools/libxc
LIBRARYDIR += -L$(XEN_ROOT)/tools/xenstore
LIBRARY = xenctrl -lxenguest -lxenstore

XCEXTDIR = $(XEN_ROOT)/tools/python/xen/lowlevel/xc
XSEXTDIR = $(XEN_ROOT)/tools/python/xen/lowlevel/xs

#CFLAGS = -fno-strict-aliasing -Wall -Werror  -I$(INCLUDEDIR) -L$(LIBRARYDIR) -l$(LIBRARY)
CFLAGS += -pthread -fno-strict-aliasing -Wall -Werror

TARGETS = xen
TARGETS += xen/lowlevel
TARGETS += xen/sv
TARGETS += xen/util
TARGETS += xen/web
TARGETS += xen/xm 
TARGETS += xen/xend
TARGETS += xen/xend/server
TARGETS += xen/xend/xenstore

all: build
build:  $(XCEXTDIR)/xc.c $(XSEXTDIR)/xs.c
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $(XCEXTDIR)/xc.c -o  $(XCEXTDIR)/xc.o
	$(CC) $(CFLAGS) -shared -L$(LIBRARYDIR) -l$(LIBRARY) $(XCEXTDIR)/xc.o -o $(XCEXTDIR)/xc.so
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $(XSEXTDIR)/xs.c -o $(XSEXTDIR)/xs.o
	$(CC) -shared $(CFLAGS) -L$(LIBRARYDIR) -l$(LIBRARY) $(XSEXTDIR)/xs.o -o $(XSEXTDIR)/xs.so

clean:
	rm -rf *.so *.o

install: build 
	for i in $(TARGETS); \
	do \
	[ -d $(DESTDIR)/usr/lib/python/$$i ] || \
		$(INSTALL_DIR) $(DESTDIR)/usr/lib/python/$$i; \
		for j in `ls $$i/*.py`; \
		do \
			$(INSTALL_PROG) $$j $(DESTDIR)/usr/lib/python/$$i; \
		done \
	done
	$(INSTALL_PROG) $(XCEXTDIR)/xc.so $(DESTDIR)/usr/lib/python/xen/lowlevel
	$(INSTALL_PROG) $(XSEXTDIR)/xs.so $(DESTDIR)/usr/lib/python/xen/lowlevel
