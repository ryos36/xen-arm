include $(CURDIR)/../Config.mk

OBJS 	:= $(patsubst %.c,%.o,$(wildcard *.c))
OBJS 	+= $(patsubst %.S,%.o,$(wildcard *.S))
										   
default: $(OBJS)
	$(LD) $(LDFLAGS) -r -o built_in.o $^

clean:

%.o: %.S $(HDRS) Makefile
	$(AS) (ASFLAGS) -c $< -o $@

%.o: %.c $(HDRS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HDRS) Makefile
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@

