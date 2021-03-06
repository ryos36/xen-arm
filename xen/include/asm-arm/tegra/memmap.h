#ifndef __ARCH_TEGRA_MEMMAP_H__
#define __ARCH_TEGRA_MEMMAP_H__

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

#endif /* __ARCH_TEGRA_MEMMAP_H__ */
