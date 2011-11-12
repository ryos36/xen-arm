/******************************************************************************
 * timer.h
 * 
 * Copyright (c) 2002-2003 Rolf Neugebauer
 * Copyright (c) 2002-2005 K A Fraser
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <xen/spinlock.h>
#include <xen/time.h>
#include <xen/string.h>


struct timer {
    /* System time expiry value (nanoseconds since boot). */
    s_time_t		expires;
    /* CPU on which this timer will be installed and executed. */
    unsigned int	cpu;
    /* On expiry, '(*function)(data)' will be executed in softirq context. */
    void			(*function)(void *);
    void			*data;
    /* Has this timer been killed (cannot be activated)? */
    int				activated;
	struct timer	*next;
};

/*
 * All functions below can be called for any CPU from any CPU in any context.
 */

/*
 * Returns TRUE if the given timer is on a timer list.
 * The timer must *previously* have been initialised by init_timer(), or its
 * structure initialised to all-zeroes.
 */
static __inline__ int active_timer(struct timer *timer)
{
    return timer->activated;
}

/*
 * Initialise a timer structure with an initial callback CPU, callback
 * function and callback data pointer. This function may be called at any
 * time (and multiple times) on an inactive timer. It must *never* execute
 * concurrently with any other operation on the same timer.
 */
static __inline__ void init_timer(
    struct timer *timer,
    void           (*function)(void *),
    void            *data,
    unsigned int     cpu)
{
    memset(timer, 0, sizeof(*timer));
    timer->function	 = function;
    timer->data		 = data;
    timer->cpu		 = cpu;
}

/*
 * Set the expiry time and activate a timer. The timer must *previously* have
 * been initialised by init_timer() (so that callback details are known).
 */
extern void set_timer(struct timer *timer, s_time_t expires);

/*
 * Deactivate a timer This function has no effect if the timer is not currently
 * active.
 * The timer must *previously* have been initialised by init_timer(), or its
 * structure initialised to all zeroes.
 */
extern void stop_timer(struct timer *timer);

/*
 * Migrate a timer to a different CPU. The timer may be currently active.
 * The timer must *previously* have been initialised by init_timer(), or its
 * structure initialised to all zeroes.
 */
extern void migrate_timer(struct timer *timer, unsigned int new_cpu);

/*
 * Deactivate a timer and prevent it from being re-set (future calls to
 * set_timer will silently fail). When this function returns it is guaranteed
 * that the timer callback handler is not running on any CPU.
 * The timer must *previously* have been initialised by init_timer(), or its
 * structure initialised to all zeroes.
 */
extern void kill_timer(struct timer *timer);

/*
 * Bootstrap initialisation. Must be called before any other timer function.
 */
extern void timer_init(void);

#endif /* _TIMER_H_ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
