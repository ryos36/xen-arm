#ifndef __ASM_ATOMIC_H__
#define __ASM_ATOMIC_H__

typedef struct {
	volatile int counter;
} atomic_t;


#define ATOMIC_INIT(i)		{ (i) }

#define atomic_read(v)		((v)->counter)

#if CONFIG_CPU_CORE_VERSION >= 6

static inline void atomic_set(atomic_t *v, int i)
{
	unsigned long tmp;

	__asm__ __volatile__("@ atomic_set\n"
"1:     ldrex   %0, [%1]\n"
"       strex   %0, %2, [%1]\n"
"       teq     %0, #0\n"
"       bne     1b"
	: "=&r" (tmp)
	: "r" (&v->counter), "r" (i)
	: "cc");
}

static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	__asm__ __volatile__("@ atomic_add_return\n"
"1:     ldrex   %0, [%2]\n"
"       add     %0, %0, %3\n"
"       strex   %1, %0, [%2]\n"
"       teq     %1, #0\n"
"       bne     1b"
	: "=&r" (result), "=&r" (tmp)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	return result;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	__asm__ __volatile__("@ atomic_sub_return\n"
"1:     ldrex   %0, [%2]\n"
"       sub     %0, %0, %3\n"
"       strex   %1, %0, [%2]\n"
"       teq     %1, #0\n"
"       bne     1b"
	: "=&r" (result), "=&r" (tmp)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	return result;
}


static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long tmp, tmp2;

	__asm__ __volatile__("@ atomic_clear_mask\n"
"1:     ldrex   %0, [%2]\n"
"       bic     %0, %0, %3\n"
"       strex   %1, %0, [%2]\n"
"       teq     %1, #0\n"
"       bne     1b"
	: "=&r" (tmp), "=&r" (tmp2)
	: "r" (addr), "Ir" (mask)
	: "cc");
}

static inline atomic_t atomic_cmpxchg(atomic_t *ptr, atomic_t old, atomic_t new)
{
	atomic_t oldval, res;

	do {
		__asm__ __volatile__("@ atomic_cmpxchg\n"
		"ldrex  %1, [%2]\n"
		"mov    %0, #0\n"
		"teq    %1, %3\n"
		"strexeq %0, %4, [%2]\n"
		: "=&r" (res.counter), "=&r" (oldval.counter)
		: "r" (&ptr->counter), "Ir" (old.counter), "r" (new.counter)
		: "cc");
	} while (res.counter);

	return oldval;
}

#else
static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val += i;
	local_irq_restore(flags);

	return val;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val -= i;
	local_irq_restore(flags);

	return val;
}

static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	local_irq_save(flags);
	*addr &= ~mask;
	local_irq_restore(flags);
}


static inline atomic_t atomic_cmpxchg(atomic_t *v, atomic_t old, atomic_t new)
{
        atomic_t ret; 
        unsigned long flags;

        local_irq_save(flags);
        ret.counter  = v->counter;
        if (likely(ret.counter == old.counter))
                v->counter = new.counter;
        local_irq_restore(flags);

        return ret;
}


#define atomic_set(v,i)		(((v)->counter) = (i))

#endif


#define _atomic_read(v)		atomic_read(&v)
#define _atomic_set(v,i)	atomic_set(&v,i)

#define atomic_add(i, v)	(void) atomic_add_return(i, v)
#define atomic_inc(v)		(void) atomic_add_return(1, v)
#define atomic_sub(i, v)	(void) atomic_sub_return(i, v)
#define atomic_dec(v)		(void) atomic_sub_return(1, v)

#define atomic_inc_and_test(v)	(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)	(atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)    (atomic_add_return(1, v))
#define atomic_dec_return(v)    (atomic_sub_return(1, v))

#define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)

static inline atomic_t atomic_compareandswap(atomic_t old, atomic_t new, atomic_t *v)
{
        atomic_t rc;
        rc = atomic_cmpxchg( (atomic_t *)v, old, new);
        return rc;
}
#endif
