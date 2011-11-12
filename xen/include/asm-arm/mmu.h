#ifndef __ARM_MMU_H__
#define __ARM_MMU_H__

#define MMU_NEED_FLUSH		1

#define TTB_C                   (1 << 0)
#define TTB_S                   (1 << 1)
#define TTB_IMP                 (1 << 2)
#define TTB_RGN_NC              (0 << 3)
#define TTB_RGN_WBWA            (1 << 3)
#define TTB_RGN_WT              (2 << 3)
#define TTB_RGN_WB              (3 << 3)

#define TTB_FLAGS               (TTB_RGN_WBWA | TTB_S)

#define TTB_MASK		(~0x3FFF)

#ifndef __ASSEMBLY__
static inline unsigned int get_dacr(void)
{
	unsigned int val;

	asm("mrc p15, 0, %0, c3, c0, 0" : "=r" (val) : : "cc");

	return val;
}


static inline void set_dacr(unsigned long val)
{
	asm("mrc p15, 0, %0, c3, c0, 0" : "=r" (val) : : "cc");
}


static inline unsigned int get_ttbr(void)
{
	unsigned int val;
	
	asm("mrc p15, 0, %0, c2, c0, 0" : "=r" (val) : : "cc");

	return val;
}

static inline void set_ttbr(unsigned int ttb)
{
	asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (ttb) : "cc");

	isb();
}

static inline void set_contextidr(unsigned long id)
{
	asm("mcr     p15, 0, %0, c13, c0, 1" : : "r" (id) : "cc");
}

static inline unsigned int get_contextidr(void)
{
	unsigned int val;
	
	asm("mrc p15, 0, %0, c13, c0, 1" : "=r" (val) : : "cc");

	return val;
}

#endif
#endif

