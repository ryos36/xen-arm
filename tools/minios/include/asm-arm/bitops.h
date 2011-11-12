#ifndef __ASM_BITOPS_H__
#define __ASM_BITOPS_H__

#include <os/irq.h>
#include <os/lib.h>

#define __xg(x)			((struct __xchg_dummy *)(x))
#define xchg(ptr,v)		((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

void __bad_xchg(volatile void *ptr, int size)
{
	printk("xchg: bad data size: pc 0x%p, ptr 0x%p, size %d\n",
		__builtin_return_address(0), ptr, size);
}

static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	unsigned long ret;

	switch (size) {
		case 1:
			asm volatile("@	__xchg1\n"
						 "	swpb	%0, %1, [%2]"
						 : "=&r" (ret)
						 : "r" (x), "r" (ptr)
						 : "memory", "cc");
			break;
		case 4:
			asm volatile("@	__xchg4\n"
						 "	swp	%0, %1, [%2]"
						 : "=&r" (ret)
						 : "r" (x), "r" (ptr)
						 : "memory", "cc");
			break;
		default:
			__bad_xchg(ptr, size), ret = 0;
			break;
	}

	return ret;
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It can be reorderdered on other architectures other than x86.
 * It also implies a memory barrier.
 */
static inline int test_and_clear_bit(int nr, volatile unsigned long *p)
{
	unsigned long oldval, mask = 1UL << (nr & 31);

	p += nr >> 5;

	oldval = *p;
	*p = oldval & ~mask;
	return oldval & mask;

}

static inline int test_and_change_bit(int nr, volatile unsigned long *p)
{
	unsigned long oldval, mask = 1UL << (nr & 31);

	p += nr >> 5;

	oldval = *p;
	*p = oldval ^ mask;
	return oldval & mask;
}


static inline int constant_test_bit(int nr, const volatile unsigned long *addr)
{
	return ((1UL << (nr & 31)) & (addr[nr >> 5])) != 0;
}

static inline int variable_test_bit(int nr, const volatile unsigned long * addr)
{
	return 0;
}

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 *
 * Note: there are no guarantees that this function will not be reordered
 * on non x86 architectures, so if you are writting portable code,
 * make sure not to rely on its reordering guarantees.
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
 
static inline void set_bit(unsigned int bit, volatile unsigned long *p)
{
	unsigned long flags;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	local_irq_save(flags);
	*p |= mask;
	local_irq_restore(flags);
}

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static inline void clear_bit(unsigned int bit, volatile unsigned long *p)
{
	unsigned long flags;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	local_irq_save(flags);
	*p &= ~mask;
	local_irq_restore(flags);
}

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
inline unsigned long __ffs(unsigned long word)
{
	int k;

	k = 31;
	if (word & 0x0000ffff) { k -= 16; word <<= 16; }
	if (word & 0x00ff0000) { k -= 8;  word <<= 8;  }
	if (word & 0x0f000000) { k -= 4;  word <<= 4;  }
	if (word & 0x30000000) { k -= 2;  word <<= 2;  }
	if (word & 0x40000000) { k -= 1; }
        return k;
}


static __inline__ void synch_set_bit(int nr, volatile void * addr)
{
}

static __inline__ void synch_clear_bit(int nr, volatile void * addr)
{
}

static __inline__ int synch_test_and_set_bit(int nr, volatile void * addr)
{
	return 0;
}

static __inline__ int synch_test_and_clear_bit(int nr, volatile void * addr)
{
	return 0;
}

static __inline__ int synch_const_test_bit(int nr, const volatile void * addr)
{
	return 0;
}

static __inline__ int synch_var_test_bit(int nr, volatile void * addr)
{
	return 0;
}

#define synch_test_bit(nr,addr) (__builtin_constant_p(nr) ? synch_const_test_bit((nr),(addr)) : synch_var_test_bit((nr),(addr)))

#endif /* _BITOPS_H_ */
