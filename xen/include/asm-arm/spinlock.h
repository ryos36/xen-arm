#ifndef __ARM_SPINLOCK_H__
#define __ARM_SPINLOCK_H__

#include <xen/config.h>
#include <xen/lib.h>
#include <asm/atomic.h>

/*
 * Unlocked value : 0
 * Locked value   : 1
 */
#define _RAW_SPIN_LOCK_UNLOCKED	{ 0 }
#define _RAW_RW_LOCK_UNLOCKED	{ 0 }

typedef struct {
	volatile unsigned int lock;
}raw_spinlock_t;

typedef struct rwlock {
	volatile unsigned int lock;
}raw_rwlock_t;

#define _raw_spin_is_locked(x)	((x)->lock != 0)

static inline void _raw_spin_lock(raw_spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
"1:	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
#ifdef CONFIG_CPU_ISA_V6K
"	wfene\n"
#endif
"	strexeq	%0, %2, [%1]\n"
"	teqeq	%0, #0\n"
"	bne	1b"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (1)
	: "cc");

	mb();
}

static inline int _raw_spin_trylock(raw_spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
"	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
"	strexeq	%0, %2, [%1]"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (1)
	: "cc");

	if (tmp == 0) {
		mb();

		return 1;
	} else {
		return 0;
	}
}

static inline void _raw_spin_unlock(raw_spinlock_t *lock)
{
	mb();

	__asm__ __volatile__(
"	str	%1, [%0]\n"
#ifdef CONFIG_CPU_ISA_V6K
"	mcr	p15, 0, %1, c7, c10, 4\n" /* DSB */
"	sev"
#endif
	:
	: "r" (&lock->lock), "r" (0)
	: "cc");
}

/*
 * RWLOCKS
 *
 *
 * Write locks are easy - we just set bit 31.  When unlocking, we can
 * just write zero since the lock is exclusively held.
 */

static inline void _raw_write_lock(raw_rwlock_t *rw)
{
	unsigned long tmp;

	__asm__ __volatile__(
"1:	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
#ifdef CONFIG_CPU_ISA_V6K
"	wfene\n"
#endif
"	strexeq	%0, %2, [%1]\n"
"	teq	%0, #0\n"
"	bne	1b"
	: "=&r" (tmp)
	: "r" (&rw->lock), "r" (0x80000000)
	: "cc");

	mb();
}

static inline int _raw_write_trylock(raw_rwlock_t *rw)
{
	unsigned long tmp;

	__asm__ __volatile__(
"1:	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
"	strexeq	%0, %2, [%1]"
	: "=&r" (tmp)
	: "r" (&rw->lock), "r" (0x80000000)
	: "cc");

	if (tmp == 0) {
		mb();
		return 1;
	} else {
		return 0;
	}
}

static inline void _raw_write_unlock(raw_rwlock_t *rw)
{
	mb();

	__asm__ __volatile__(
	"str	%1, [%0]\n"
#ifdef CONFIG_CPU_ISA_V6K
"	mcr	p15, 0, %1, c7, c10, 4\n" /* DSB */
"	sev\n"
#endif
	:
	: "r" (&rw->lock), "r" (0)
	: "cc");
}

#define _raw_rw_is_locked(x)		((x)->lock != 0)

/* write_can_lock - would write_trylock() succeed? */
#define _raw_write_can_lock(x)		((x)->lock == 0)

/*
 * Read locks are a bit more hairy:
 *  - Exclusively load the lock value.
 *  - Increment it.
 *  - Store new lock value if positive, and we still own this location.
 *    If the value is negative, we've already failed.
 *  - If we failed to store the value, we want a negative result.
 *  - If we failed, try again.
 * Unlocking is similarly hairy.  We may have multiple read locks
 * currently active.  However, we know we won't have any write
 * locks.
 */
static inline void _raw_read_lock(raw_rwlock_t *rw)
{
	unsigned long tmp, tmp2;

	__asm__ __volatile__(
"1:	ldrex	%0, [%2]\n"
"	adds	%0, %0, #1\n"
"	strexpl	%1, %0, [%2]\n"
#ifdef CONFIG_CPU_ISA_V6K
"	wfemi\n"
#endif
"	rsbpls	%0, %1, #0\n"
"	bmi	1b"
	: "=&r" (tmp), "=&r" (tmp2)
	: "r" (&rw->lock)
	: "cc");

	mb();
}

static inline void _raw_read_unlock(raw_rwlock_t *rw)
{
	unsigned long tmp, tmp2;

	mb();

	__asm__ __volatile__(
"1:	ldrex	%0, [%2]\n"
"	sub	%0, %0, #1\n"
"	strex	%1, %0, [%2]\n"
"	teq	%1, #0\n"
"	bne	1b"
#ifdef CONFIG_CPU_ISA_V6K
"\n	cmp	%0, #0\n"
"	mcreq   p15, 0, %0, c7, c10, 4\n"
"	seveq"
#endif
	: "=&r" (tmp), "=&r" (tmp2)
	: "r" (&rw->lock)
	: "cc");
}

static inline int _raw_read_trylock(raw_rwlock_t *rw)
{
	unsigned long tmp, tmp2 = 1;

	__asm__ __volatile__(
"1:	ldrex	%0, [%2]\n"
"	adds	%0, %0, #1\n"
"	strexpl	%1, %0, [%2]\n"
	: "=&r" (tmp), "+r" (tmp2)
	: "r" (&rw->lock)
	: "cc");

	mb();
	return tmp2 == 0;
}

/* read_can_lock - would read_trylock() succeed? */
#define _raw_read_can_lock(x)		((x)->lock < 0x80000000)

#define _raw_spin_relax(lock)	cpu_relax()
#define _raw_read_relax(lock)	cpu_relax()
#define _raw_write_relax(lock)	cpu_relax()

#endif /* __ASM_SPINLOCK_H */
