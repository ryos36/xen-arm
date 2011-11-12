INSTALL         = install
INSTALL_DIR     = $(INSTALL) -d -m0755
INSTALL_PROG    = $(INSTALL) -m0755
INSTALL_DATA    = $(INSTALL) -m0644

XEN_ROOT = ../../../..
include $(XEN_ROOT)/tools/Rules.mk
 
all: build

INCLUDEDIR = $(XEN_ROOT)/tools/addons/python-2.4.3/Include/

INCLUDEDIR += -I$(XEN_ROOT)/tools/libxc
INCLUDEDIR += -I$(XEN_ROOT)/tools/xenstore

LIBRARYDIR =  $(XEN_ROOT)/tools/libxc
LIBRARYDIR += -L$(XEN_ROOT)/tools/xenstore
LIBRARY = xenctrl -lxenguest -lxenstore

#CFLAGS = -fno-strict-aliasing -Wall -Werror  -I$(INCLUDEDIR) -L$(LIBRARYDIR) -l$(LIBRARY)
CFLAGS += -pthread -fno-strict-aliasing -Wall -Werror 

 
all: build
build:  xc/xc.c xs/xs.c
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c xc/xc.c -o  xc/xc.o
	$(CC) $(CFLAGS) -shared -L$(LIBRARYDIR) -l$(LIBRARY) xc/xc.o -o xc/xc.so
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c xs/xs.c -o xs/xs.o
	$(CC) -shared $(CFLAGS) -L$(LIBRARYDIR) -l$(LIBRARY) xs/xs.o -o xs/xs.so  

clean:
	rm -rf *.so *.o

install: xc.so xs.so
	[ -d $(DESTDIR)/usr/lib ] || \
		$(INSTALL_DIR) $(DESTDIR)/usr/lib
	$(INSTALL_PROG) $< $(DESTDIR)/usr/lib
