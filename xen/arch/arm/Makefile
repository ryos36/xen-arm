#
# xen/arch/arm/Makefile
#

ifndef TARGET_SUBARCH
$(error Unknown target platform)
endif

subdir-y += $(TARGET_SUBARCH) xen lib 

OBJCOPYFLAGS    :=-O binary -R .note -R .comment -S

$(TARGET): asm-offsets.h $(TARGET_SUBARCH)/start.o xen.lds $(ALL_OBJS)
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds $(TARGET_SUBARCH)/start.o $(ALL_OBJS) -o $@
	$(OBJCOPY) $(OBJCOPYFLAGS) $@ xen-bin

xen.lds: xen/xen.lds.S
	$(call quiet-cmd, $(CC) -E $(CFLAGS) -P $(AFLAGS) -o $@ $<, " GEN\t$@")

clean:: FORCE
	rm -f arch/arm/*.o arch/arm/$(TARGET_SUBARCH)/*.o lib/*.o xen/*.o xen.lds

asm-offsets.s: xen/asm-offsets.c
	$(call quite-cmd,$(CC) $(CFLAGS) -S -o $@ $<," GEN\t$@")

asm-offsets.h: asm-offsets.s
	@(set -e; \
	echo "/*"; \
	echo " * DO NOT MODIFY."; \
	echo " *"; \
	echo " * This file was auto-generated from $<"; \
	echo " *"; \
	echo " */"; \
	echo ""; \
	echo "#ifndef __ASM_OFFSETS_H__"; \
	echo "#define __ASM_OFFSETS_H__"; \
	echo ""; \
	sed -ne "/^->/{s:^->\([^ ]*\) [\$$#]*\([^ ]*\) \(.*\):#define \1 \2 /* \3 */:; s:->::; p;}"; \
	echo ""; \
	echo "#endif") <$< >$(BASEDIR)/include/asm-arm/$@

