include $(CURDIR)/../../Config.mk

OBJS	:= march-$(XENO_TARGET_SUBARCH)/start.o
OBJS	+= $(patsubst %.c,%.o,$(wildcard march-$(XENO_TARGET_SUBARCH)/*.c))
OBJS 	+= $(patsubst %.c,%.o,$(wildcard common/*.c))
OBJS 	+= $(patsubst %.S,%.o,$(wildcard common/*.S))
OBJS 	+= $(patsubst %.c,%.o,$(wildcard xen/*.c))					   
OBJS 	+= $(patsubst %.c,%.o,$(wildcard lib/*.c))

default: $(XENO_TARGET_ARCH)

$(XENO_TARGET_ARCH): $(OBJS)
	$(LD) $(LDFLAGS) -r -o $@.o $^

clean:


%.o: %.c $(HDRS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HDRS) Makefile
	$(CC) $(CFLAGS) $(AFLAGS) -D__ASSEMBLY__ -c $< -o $@


