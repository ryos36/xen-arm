#ifndef __ARM_CONFIG_H__
#define __ARM_CONFIG_H__

#include <asm/arch/config.h>

#define supervisor_mode_kernel	(0)

#ifdef CONFIG_USE_HIGH_VECTORS
# define VECTORS_BASE		0xFFFF0000
#else
# define VECTORS_BASE		0x00000000
#endif

#define ARCH_HAS_IRQ_CONTROL	1

#ifndef STACK_ORDER               
# define STACK_ORDER		0
#endif

#define STACK_SIZE		(PAGE_SIZE << STACK_ORDER)
#define FORCE_CRASH()		while(1);

#ifndef NDEBUG
# define MEMORY_GUARD
#endif

#ifndef CONFIG_NR_CPUS
#define CONFIG_NR_CPUS	1
#endif

#define NR_CPUS			CONFIG_NR_CPUS

#define COMMAND_LINE_SIZE	512

#define OPT_CONSOLE_STR		"com1"

#define IOREMAP_MBYTES		4
#define DIRECTMAP_MBYTES	12
#define MAPCACHE_MBYTES		4

#define IOREMAP_VIRT_END	0UL
#define IOREMAP_VIRT_START	(IOREMAP_VIRT_END - (IOREMAP_MBYTES << 20))
#define DIRECTMAP_VIRT_END	(IOREMAP_VIRT_START)
#define DIRECTMAP_VIRT_START	(DIRECTMAP_VIRT_END - (DIRECTMAP_MBYTES << 20))
#define MAPCACHE_VIRT_END	(DIRECTMAP_VIRT_START)
#define MAPCACHE_VIRT_START	(MAPCACHE_VIRT_END - (MAPCACHE_MBYTES << 20))

#define SHARED_INFO_LIMIT	(MAPCACHE_VIRT_START)
#define SHARED_INFO_BASE	(SHARED_INFO_LIMIT -(1 << 20))

#define MAX_DMADOM_PFN 		0xFFFFFUL /* 32 addressable bits */


#ifndef HYPERVISOR_VIRT_START
# define HYPERVISOR_VIRT_START	mk_unsigned_long(0xFC000000)
#endif

#ifndef __ASSEMBLY__
extern unsigned long _end;		/* standard ELF symbol */
extern unsigned long xenheap_phys_start;
extern unsigned long xenheap_phys_end;	/* user-configurable */

extern void not_yet(void);
#endif

#define ELFSIZE	32

#endif  



