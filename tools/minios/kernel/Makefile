include $(CURDIR)/../Config.mk

OBJS 	:= $(patsubst %.c,%.o,$(wildcard *.c))
										   
default: $(OBJS)
	$(LD) $(LDFLAGS) -r -o built_in.o $^
clean:

%.o: %.c $(HDRS) Makefile
	@echo $(CFLAGS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HDRS) Makefile
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@


