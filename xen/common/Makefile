obj-y += bitmap.o
obj-y += dom0_ops.o
obj-y += domain.o
obj-y += elf.o
obj-y += event_channel.o
obj-y += grant_table.o
obj-y += kernel.o
obj-y += lib.o
obj-y += memory.o
obj-y += multicall.o
obj-y += page_alloc.o
obj-y += rangeset.o
#obj-y += sched_bvt.o
obj-y += sched_sedf.o
obj-y += sched_credit.o
obj-y += schedule.o
obj-y += softirq.o
obj-y += string.o
#obj-y += symbols.o
#obj-y += trace.o
obj-y += timer.o
obj-y += vsprintf.o
obj-y += xmalloc.o
obj-y += spinlock.o
#obj-y += vdma.o

obj-$(perfc)       += perfc.o
obj-$(crash_debug) += gdbstub.o

# Object file contains changeset and compiler information.
kernel.o: $(BASEDIR)/include/xen/compile.h
