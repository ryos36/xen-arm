#include <xen/config.h>
#include <xen/spinlock.h>
#include <xen/init.h>
#include <xen/types.h>
#include <xen/errno.h>
#include <xen/sched.h>
#include <xen/lib.h>
#include <xen/time.h>
#include <xen/softirq.h>
#include <xen/timer.h>
#include <asm/system.h>

#define TIMER_SLOP (50 * 1000)

struct timers {
    spinlock_t	 lock;
    struct timer *running;
    struct timer *list;
};

struct timers __cacheline_aligned timers[NR_CPUS];

static int add_entry(struct timer **timer, struct timer *t)
{
	struct timer *curr, **_pprev = timer;

	while ( ((curr = *_pprev) != NULL) && (curr->expires <= t->expires) )
		_pprev = &curr->next;

	t->next = curr;
	*_pprev = t;

	return (_pprev == timer);
}

static int remove_entry(struct timer **timer, struct timer *t)
{
	struct timer *curr, **_pprev = timer;

	while ( (curr = *_pprev) != t )
		_pprev = &curr->next;

	*_pprev = t->next;

	return (_pprev == timer);
}

int reprogram_timer(s_time_t timeout)
{
    s_time_t    expire;

    if ( timeout == 0 )
        return 1;

    expire = timeout - NOW();

    if ( expire <= 0 )
      return 0;	
	
    return 1;
}

static void timer_softirq_action(void)
{
	struct timer *t;
	struct timers *ts;
	int cpu = smp_processor_id();

	spin_lock_irq(&timers[cpu].lock);  

	ts = &timers[cpu];

	while(((t = ts->list) != NULL) && (t->expires < (NOW() + TIMER_SLOP)))
	{
		ts->list = t->next;

		ts->running = t;
	
		spin_unlock_irq(&ts->lock);
		t->activated = 0;
		t->function(t->data);
		spin_lock_irq(&ts->lock);

		ts->running = NULL;
	}

	spin_unlock_irq(&ts->lock);
}

static inline void timer_lock(struct timer *timer)
{
    unsigned int cpu;

    for ( ; ; )
    {
        cpu = timer->cpu;
        spin_lock(&timers[cpu].lock);
        if ( likely(timer->cpu == cpu) )
            break;
        spin_unlock(&timers[cpu].lock);
    }
}

static inline void timer_unlock(struct timer *timer)
{
    spin_unlock(&timers[timer->cpu].lock);
}

#define timer_lock_irq(t) \
    do { local_irq_disable(); timer_lock(t); } while ( 0 )
#define timer_lock_irqsave(t, flags) \
    do { local_irq_save(flags); timer_lock(t); } while ( 0 )
#define timer_unlock_irq(t) \
    do { timer_unlock(t); local_irq_enable(); } while ( 0 )
#define timer_unlock_irqrestore(t, flags) \
    do { timer_unlock(t); local_irq_restore(flags); } while ( 0 )

void set_timer(struct timer *timer, s_time_t expires)
{
    int cpu;
    unsigned long flags;

    timer_lock_irqsave(timer, flags);

    timer->expires = expires;
	
    cpu = timer->cpu;

    if(!active_timer(timer))
    {
        add_entry(&timers[cpu].list, timer);
        timer->activated = 1;
    }


    cpu_raise_softirq(cpu, TIMER_SOFTIRQ);

    timer_unlock_irqrestore(timer, flags);
}

void stop_timer(struct timer *timer)
{
    int cpu;
    unsigned long flags;

    timer_lock_irqsave(timer, flags);

    cpu = timer->cpu;

    if(active_timer(timer))
    {
        remove_entry(&timers[cpu].list, timer);
        timer->activated = 0;
    }

    cpu_raise_softirq(cpu, TIMER_SOFTIRQ);

    timer_unlock_irqrestore(timer, flags);

}

void kill_timer(struct timer *timer)
{
    int           cpu;
    unsigned long flags;

    BUG_ON(timers[smp_processor_id()].running == timer);

    stop_timer(timer);

    for_each_online_cpu ( cpu )
        while ( timers[cpu].running == timer )
            cpu_relax();
}

void __init timer_init(void)
{
    int i;

    open_softirq(TIMER_SOFTIRQ, timer_softirq_action);

    for ( i = 0; i < NR_CPUS; i++ )
    {
        spin_lock_init(&timers[i].lock);
        timers[i].list		= NULL;
	timers[i].running	= NULL;
    }
}
/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
