/*
 * memmap.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *             SungKwan Heo <sk.heo@samsung.com>
 *             Jaemin Ryu <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ARM_MEMMAP_H__
#define __ARM_MEMMAP_H__

#include <asm/page.h>

#define MEMMAP_ENTRY_MAX	16

#ifndef  __ASSEMBLY__
struct map_desc {
	unsigned long virtual;
	unsigned long physical;
	unsigned long length;
	unsigned long type;
};

struct memmap_entry {
  unsigned int	addr;
  unsigned int	size;
  unsigned int	type;
};

struct system_memmap {
  int	nr_map;
  struct memmap_entry map[MEMMAP_ENTRY_MAX];
};

extern unsigned long 	init_memmap(void);
extern void		print_memmap(void);

extern struct system_memmap memmap;

#endif /* !__ASSEMBLY__ */

#ifdef __ASSEMBLY__
#define MAP_ENTRY(va, pa, size, attr)           \
	.word   size                            ;\
	.word   4 * ((va) >> 20)                ;\
	.word   (pa) | (attr)                   ;
#endif

/*
 *  Physical memory map 
 */

#define MEMMAP_GUEST0_PARTITION_BASE	(PHYS_OFFSET)
#define MEMMAP_GUEST0_PARTITION_SIZE	(CONFIG_MEMMAP_GUEST0_SIZE)
#define MEMMAP_GUEST0_PARTITION_LIMIT	(MEMMAP_GUEST0_PARTITION_BASE + MEMMAP_GUEST0_PARTITION_SIZE)

#define MEMMAP_GUEST1_PARTITION_BASE	(MEMMAP_GUEST0_PARTITION_LIMIT)
#define MEMMAP_GUEST1_PARTITION_SIZE	(CONFIG_MEMMAP_GUEST1_SIZE)
#define MEMMAP_GUEST1_PARTITION_LIMIT	(MEMMAP_GUEST1_PARTITION_BASE + MEMMAP_GUEST1_PARTITION_SIZE)

#define MEMMAP_GUEST2_PARTITION_BASE	(MEMMAP_GUEST1_PARTITION_LIMIT)
#define MEMMAP_GUEST2_PARTITION_SIZE	(CONFIG_MEMMAP_GUEST2_SIZE)
#define MEMMAP_GUEST2_PARTITION_LIMIT	(MEMMAP_GUEST2_PARTITION_BASE + MEMMAP_GUEST2_PARTITION_SIZE)

#define MEMMAP_GUEST3_PARTITION_BASE	(MEMMAP_GUEST2_PARTITION_LIMIT)
#define MEMMAP_GUEST3_PARTITION_SIZE	(CONFIG_MEMMAP_GUEST3_SIZE)
#define MEMMAP_GUEST3_PARTITION_LIMIT	(MEMMAP_GUEST3_PARTITION_BASE + MEMMAP_GUEST3_PARTITION_SIZE)

#define MEMMAP_HYPERVISOR_LIMIT		(0x40000000)
#define MEMMAP_HYPERVISOR_SIZE		(0x00B00000)
#define MEMMAP_HYPERVISOR_BASE		(MEMMAP_HYPERVISOR_LIMIT - MEMMAP_HYPERVISOR_SIZE)

#define MEMMAP_GUEST0_IMAGE_SIZE	CONFIG_MEMMAP_GUEST0_IMAGE_SIZE
#define MEMMAP_GUEST1_IMAGE_SIZE	CONFIG_MEMMAP_GUEST1_IMAGE_SIZE
#define MEMMAP_GUEST2_IMAGE_SIZE	CONFIG_MEMMAP_GUEST2_IMAGE_SIZE
#define MEMMAP_GUEST3_IMAGE_SIZE	CONFIG_MEMMAP_GUEST3_IMAGE_SIZE

#define MEMMAP_GUEST0_RAMDISK_SIZE	CONFIG_MEMMAP_GUEST0_RAMDISK_SIZE	
#define MEMMAP_GUEST1_RAMDISK_SIZE	CONFIG_MEMMAP_GUEST1_RAMDISK_SIZE
#define MEMMAP_GUEST2_RAMDISK_SIZE	CONFIG_MEMMAP_GUEST2_RAMDISK_SIZE
#define MEMMAP_GUEST3_RAMDISK_SIZE	CONFIG_MEMMAP_GUEST3_RAMDISK_SIZE

#define MEMMAP_GUEST0_IMAGE_BASE	(MEMMAP_GUEST0_PARTITION_LIMIT - MEMMAP_GUEST0_IMAGE_SIZE)
#define MEMMAP_GUEST0_RAMDISK_BASE	(MEMMAP_GUEST0_IMAGE_BASE - MEMMAP_GUEST0_RAMDISK_SIZE)
#define MEMMAP_GUEST1_IMAGE_BASE	(MEMMAP_GUEST1_PARTITION_LIMIT - MEMMAP_GUEST1_IMAGE_SIZE)
#define MEMMAP_GUEST1_RAMDISK_BASE	(MEMMAP_GUEST1_IMAGE_BASE - MEMMAP_GUEST1_RAMDISK_SIZE)
#define MEMMAP_GUEST2_IMAGE_BASE	(MEMMAP_GUEST2_PARTITION_LIMIT - MEMMAP_GUEST2_IMAGE_SIZE)
#define MEMMAP_GUEST2_RAMDISK_BASE	(MEMMAP_GUEST2_IMAGE_BASE - MEMMAP_GUEST2_RAMDISK_SIZE)
#define MEMMAP_GUEST3_IMAGE_BASE	(MEMMAP_GUEST3_PARTITION_LIMIT - MEMMAP_GUEST3_IMAGE_SIZE)
#define MEMMAP_GUEST3_RAMDISK_BASE	(MEMMAP_GUEST3_IMAGE_BASE - MEMMAP_GUEST3_RAMDISK_SIZE)
#endif /* !__ARM_MEMMAP_H__ */


