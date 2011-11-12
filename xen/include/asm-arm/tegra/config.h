#ifndef __TEGRA_CONFIG_H__
#define __TEGRA_CONFIG_H__

#include <asm/arch/tegra.h>

#define HZ	100
#define CLOCK_TICK_RATE		1000000

#define PHYS_OFFSET   		(0x00000000UL)

#define MEMMAP_DRAM_ADDR	PHYS_OFFSET
#define MEMMAP_DRAM_SIZE	0x40000000 //256MB
#endif
