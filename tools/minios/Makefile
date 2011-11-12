include $(CURDIR)/Config.mk

SEDFLAGS        = s/XENO_START_ADDRESS/$(XENO_START)/

XENO_TARGET_IMAGE := mini-os

MINIOS_BASE	:= `pwd`

OBJS            := arch/$(XENO_TARGET_ARCH)/$(XENO_TARGET_ARCH).o
OBJS            += kernel/built_in.o
OBJS            += lib/built_in.o

default: $(XENO_TARGET_IMAGE)

$(XENO_TARGET_IMAGE): build
	$(LD) $(LDFLAGS) -N -T arch/$(XENO_TARGET_ARCH)/march-$(XENO_TARGET_SUBARCH)/$(XENO_TARGET_SUBARCH).lds $(OBJS) -o $@.elf
	$(OBJCOPY) -O binary -R .note -R .comment -S $(XENO_TARGET_IMAGE).elf $(XENO_TARGET_IMAGE).bin
	$(STRIP) -o mini-os.img mini-os.elf

build:
	[ -e include/asm] || ln -sf $(MINIOS_BASE)/include/asm-$(XENO_TARGET_ARCH) include/asm
	[ -e include/xen/interface] ] || ln -sf $(XEN_BASE_DIR)/xen/include/public include/xen/interface
	$(MAKE) -C kernel
	$(MAKE) -C lib
	$(MAKE) -C arch/$(XENO_TARGET_ARCH)

clean:  
	find . -type f -name '*.o' | xargs rm -f
	rm -f include/xen/interface
	rm -f include/asm
	rm -f *.o *~ core 
	rm -f *.out
	rm -f *.bin
	rm -f *.elf
	find . -type l | xargs rm -f

