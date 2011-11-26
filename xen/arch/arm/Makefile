#
# xen/arch/arm/Makefile
#

ifndef TARGET_SUBARCH
$(error Unknown target platform)
endif

subdir-y += $(TARGET_SUBARCH) xen lib 

OBJCOPYFLAGS    :=-O binary -R .note -R .comment -S

$(TARGET): $(TARGET)-syms
	$(NM) -n $< | grep -v ' [aUw] ' > $(@D)/System.map
	$(OBJCOPY) -O binary -R .note -R .comment -S $< $@

$(TARGET)-syms: $(TARGET_SUBARCH)/start.o xen.lds $(ALL_OBJS)
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds $(TARGET_SUBARCH)/start.o $(ALL_OBJS) -o $@

xen.lds: xen/xen.lds.S
	$(call quiet-cmd, $(CC) -E $(CFLAGS) -P $(AFLAGS) -o $@ $<, " GEN\t$@")

clean:: FORCE
	rm -f xen-bin xen.lds
	rm -f arch/arm/*.o arch/arm/$(TARGET_SUBARCH)/*.o lib/*.o xen/*.o 
	rm -f $(BASEDIR)/include/asm-arm/arch
	rm -f $(BASEDIR)/include/asm


asm-offsets.s: xen/asm-offsets.c prepare_headers
	$(CC) $(CFLAGS) -S -o $@ $<

prepare_headers:
	[ -e $(BASEDIR)/include/asm/arch ] || \
        ln -sf $(BASEDIR)/include/asm/$(TARGET_SUBARCH) $(BASEDIR)/include/asm/arch


