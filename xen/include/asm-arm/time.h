#ifndef __ASM_TIME_H__
#define __ASM_TIME_H__

#include <xen/types.h>
#include <xen/cache.h>
#include <xen/softirq.h>
#include <asm/page.h>
#include <asm/current.h>
#include <asm/arch/config.h>

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE	100
#endif

//#define HZ	CLOCK_TICK_RATE

#ifndef __ASSEMBLY__
#define watchdog_disable() ((void)0)
#define watchdog_enable()  ((void)0)

#define send_timer_event(v)	send_guest_virq(v, VIRQ_TIMER)

#define time_after(a, b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))

#define time_before(a, b)	time_after(b, a)

#define time_after_eq(a,b)      	\
	(typecheck(unsigned long, a) && \
	typecheck(unsigned long, b) && 	\
	((long)(a) - (long)(b) >= 0))

#define time_before_eq(a,b)     time_after_eq(b,a)

extern u64 jiffies;

/*
 *  Return nanoseconds from time of boot
 */
static inline s64 get_s_time(void)
{
	return (s64)(jiffies * (1000000000 / HZ));
}

#endif
#endif
