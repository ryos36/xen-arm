#ifndef __ZYNQ_CONFIG_H__
#define __ZYNQ_CONFIG_H__

#include <asm/arch/zynq.h>

#define HZ	100
#define CLOCK_TICK_RATE		1000000

#define XEN_START_ADDR          0xFF010000

#define PHYS_OFFSET   		(0x00000000UL)

#define ZYNQ_DRAM_PHY_ADDR	PHYS_OFFSET
#define ZYNQ_DRAM_SIZE  	0x10000000 //256M

#endif /* __ZYNQ_CONFIG_H__ */
