/****************************************************************************
 * (C) 2002-2003 - Rolf Neugebauer - Intel Research Cambridge
 * (C) 2002-2003 University of Cambridge
 * (C) 2004      - Mark Williamson - Intel Research Cambridge
 ****************************************************************************
 *
 *        File: common/schedule.c
 *      Author: Rolf Neugebauer & Keir Fraser
 *              Updated for generic API by Mark Williamson
 * 
 * Description: Generic CPU scheduling code
 *              implements support functionality for the Xen scheduler API.
 *
 */

#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/sched.h>
#include <xen/domain.h>
#include <xen/delay.h>
#include <xen/event.h>
#include <xen/time.h>
#include <xen/timer.h>
#include <xen/perfc.h>
#include <xen/sched-if.h>
#include <xen/softirq.h>
#include <xen/trace.h>
#include <xen/mm.h>
#include <xen/preempt.h>
#include <xen/guest_access.h>
#include <asm/profile.h>
#include <public/sched.h>
#include <public/sched_ctl.h>
#include <security/acm/acm_hooks.h>

/* opt_sched: scheduler - default to SEDF */
//static char opt_sched[10] = "sedf";
static char opt_sched[10] = "credit";
//static char opt_sched[10] = "credit";
string_param("sched", opt_sched);

/* Various timer handlers. */
static void s_timer_fn(void *unused);
static void vcpu_singleshot_timer_fn(void *data);
static void vcpu_periodic_timer_fn(void *data);
static void poll_timer_fn(void *data);
static void vcpu_periodic_timer_work(struct vcpu *v);

void schedule(void);

/* This is global for now so that private implementations can reach it */
struct schedule_data schedule_data[NR_CPUS];

extern struct scheduler sched_sedf_def;
extern struct scheduler sched_credit_def;
static struct scheduler *schedulers[] = { 
    &sched_sedf_def,
    &sched_credit_def,
    NULL
};

static struct scheduler ops;

#define SCHED_OP(fn, ...)                                 \
         (( ops.fn != NULL ) ? ops.fn( __VA_ARGS__ )      \
          : (typeof(ops.fn(__VA_ARGS__)))0 )

/* Per-CPU periodic timer sends an event to the currently-executing domain. */
static struct timer t_timer[NR_CPUS]; 

static inline void vcpu_runstate_change(
    struct vcpu *v, int new_state, s_time_t new_entry_time)
{
    ASSERT(v->runstate.state != new_state);

    v->runstate.time[v->runstate.state] +=
        new_entry_time - v->runstate.state_entry_time;
    v->runstate.state_entry_time = new_entry_time;
    v->runstate.state = new_state;
}

void vcpu_runstate_get(struct vcpu *v, struct vcpu_runstate_info *runstate)
{
    if ( likely(v == current) )
    {
        /* Fast lock-free path. */
        memcpy(runstate, &v->runstate, sizeof(*runstate));
        ASSERT(runstate->state == RUNSTATE_running);
        runstate->time[RUNSTATE_running] += NOW() - runstate->state_entry_time;
    }
    else
    {
        vcpu_schedule_lock_irq(v);
        memcpy(runstate, &v->runstate, sizeof(*runstate));
        runstate->time[runstate->state] += NOW() - runstate->state_entry_time;
        vcpu_schedule_unlock_irq(v);
    }
}

long do_set_timer_op(s_time_t timeout)
{
    struct vcpu *v = current;

    if ( timeout == 0 ) {
        stop_timer(&v->singleshot_timer);
    } else {
        v->singleshot_timer.cpu = smp_processor_id();
        set_timer(&v->singleshot_timer, timeout);
    }

    return 0;
}

struct domain *alloc_domain(void)
{
    struct domain *d;

    if ( (d = xmalloc(struct domain)) != NULL )
        memset(d, 0, sizeof(*d));

    return d;
}

void free_domain(struct domain *d)
{
    struct vcpu *v;
    int i;
    
    for_each_vcpu ( d, v )
        sched_rem_domain(v); 

    SCHED_OP(free_task, d);

    for ( i = MAX_VIRT_CPUS-1; i >= 0; i-- )
        if ( (v = d->vcpu[i]) != NULL )
            free_vcpu_struct(v);

    xfree(d);
}

struct vcpu *alloc_vcpu(struct domain *d, unsigned int vcpu_id, unsigned int cpu_id)
{
    struct vcpu *v;

    BUG_ON(d->vcpu[vcpu_id] != NULL);

    if ( (v = alloc_vcpu_struct(d, vcpu_id)) == NULL )
        return NULL;

    v->domain = d;
    v->vcpu_id = vcpu_id;
    v->processor = cpu_id;

    atomic_set(&v->pausecnt, 0);
    v->vcpu_info = &d->shared_info->vcpu_info[vcpu_id];

    v->cpu_affinity = is_idle_domain(d) ?
        cpumask_of_cpu(cpu_id) : CPU_MASK_ALL;

    v->runstate.state = is_idle_vcpu(v) ? RUNSTATE_running : RUNSTATE_offline;
    v->runstate.state_entry_time = NOW(); 

    if ( (vcpu_id != 0) && !is_idle_domain(d) )
        set_bit(_VCPUF_down, &v->vcpu_flags);

    if ( SCHED_OP(alloc_task, v) < 0 )
    {
        free_vcpu_struct(v);
        return NULL;
    }

    d->vcpu[vcpu_id] = v;
    if ( vcpu_id != 0 )
        d->vcpu[v->vcpu_id-1]->next_in_list = v;

    sched_add_domain(v);

    return v;
}

static void vcpu_migrate(struct vcpu *v)
{
    cpumask_t online_affinity;
    unsigned int cpu = smp_processor_id();
    unsigned long flags;
    int old_cpu;

    vcpu_schedule_lock_irqsave(v, flags);

    if (test_bit(_VCPUF_running, &v->vcpu_flags) || 
        !test_and_clear_bit(_VCPUF_migrating, &v->vcpu_flags))
    {
	vcpu_schedule_unlock_irqrestore(v, flags);
	return;
    }

    old_cpu = v->processor;

    v->processor = SCHED_OP(pick_cpu, v);

    spin_unlock_irqrestore(&schedule_data[old_cpu].schedule_lock, flags);

    vcpu_wake(v);
}

void sched_add_domain(struct vcpu *v) 
{
    /* Initialise the per-domain timers. */
    init_timer(&v->singleshot_timer, vcpu_singleshot_timer_fn, v, v->processor);
    init_timer(&v->periodic_timer, vcpu_periodic_timer_fn, v, v->processor);
    init_timer(&v->poll_timer, poll_timer_fn, v, v->processor);

    if ( is_idle_vcpu(v) )
    {
	    schedule_data[v->processor].curr = v;
	    schedule_data[v->processor].idle = v;
	    set_bit(_VCPUF_running, &v->vcpu_flags);
    }

    SCHED_OP(add_task, v);
    TRACE_2D(TRC_SCHED_DOM_ADD, v->domain->domain_id, v->vcpu_id);
}

void sched_rem_domain(struct vcpu *v) 
{
    kill_timer(&v->periodic_timer);
    kill_timer(&v->singleshot_timer);
    kill_timer(&v->poll_timer);

    SCHED_OP(rem_task, v);
    TRACE_2D(TRC_SCHED_DOM_REM, v->domain->domain_id, v->vcpu_id);
}

void vcpu_sleep_nosync(struct vcpu *v)
{
    unsigned long flags;

    vcpu_schedule_lock_irqsave(v, flags);

    if ( likely(!vcpu_runnable(v)) )
    {
        if ( v->runstate.state == RUNSTATE_runnable )
            vcpu_runstate_change(v, RUNSTATE_offline, NOW());

        SCHED_OP(sleep, v);
    }

    vcpu_schedule_unlock_irqrestore(v, flags);

    TRACE_2D(TRC_SCHED_SLEEP, v->domain->domain_id, v->vcpu_id);
}

void vcpu_sleep_sync(struct vcpu *v)
{
    vcpu_sleep_nosync(v);

    while ( !vcpu_runnable(v) && test_bit(_VCPUF_running, &v->vcpu_flags) )
        cpu_relax();	

    sync_vcpu_execstate(v);
}


void vcpu_wake(struct vcpu *v)
{
    unsigned long flags;

    vcpu_schedule_lock_irqsave(v, flags);

    if ( likely(vcpu_runnable(v)) )
    {
        if ( v->runstate.state >= RUNSTATE_blocked )
            vcpu_runstate_change(v, RUNSTATE_runnable, NOW());
        SCHED_OP(wake, v);
    }
    else if ( !test_bit(_VCPUF_blocked, &v->vcpu_flags) )
    {
        if ( v->runstate.state == RUNSTATE_blocked )
            vcpu_runstate_change(v, RUNSTATE_offline, NOW());
    }

    vcpu_schedule_unlock_irqrestore(v, flags);

    TRACE_2D(TRC_SCHED_WAKE, v->domain->domain_id, v->vcpu_id);
}

int vcpu_set_affinity(struct vcpu *v, cpumask_t *affinity)
{
    cpumask_t online_affinity;

	while(1);

    cpus_and(online_affinity, *affinity, cpu_online_map);
    if ( cpus_empty(online_affinity) )
        return -EINVAL;

    return SCHED_OP(set_affinity, v, affinity);
}

/* Block the currently-executing domain until a pertinent event occurs. */
static long do_block(void)
{
    struct vcpu *v = current;

    if ( guest_events_need_delivery(v) )
	return 0;


    do_set_timer_op(NOW() + MILLISECS(8));

    set_bit(_VCPUF_blocked, &v->vcpu_flags);

    TRACE_2D(TRC_SCHED_BLOCK, v->domain->domain_id, v->vcpu_id);

    raise_softirq(SCHEDULE_SOFTIRQ);

    return 0;
}

static long do_poll(struct sched_poll *sched_poll)
{
    struct vcpu  *v = current;
    evtchn_port_t port;
    long          rc = 0;
    unsigned int  i;

    /* Fairly arbitrary limit. */
    if ( sched_poll->nr_ports > 128 )
        return -EINVAL;

    if ( !guest_handle_okay(sched_poll->ports, sched_poll->nr_ports) )
        return -EFAULT;

    /* Ensure that upcalls are disabled: tested by evtchn_set_pending(). */
    if ( guest_event_delivery_is_enabled(v))
        return -EINVAL;

    set_bit(_VCPUF_blocked, &v->vcpu_flags);

    /* Check for events /after/ blocking: avoids wakeup waiting race. */
    for ( i = 0; i < sched_poll->nr_ports; i++ )
    {
	    rc = -EFAULT;
	    if ( __copy_from_guest_offset(&port, sched_poll->ports, i, 1))	// check is needed...
		    goto out;

	    rc = -EINVAL;
        if ( port >= MAX_EVTCHNS )
            goto out;

        rc = 0;
        if ( evtchn_pending(v->domain, port) )
            goto out;
    }

    if ( sched_poll->timeout != 0 )
        set_timer(&v->poll_timer, sched_poll->timeout);

    TRACE_2D(TRC_SCHED_BLOCK, v->domain->domain_id, v->vcpu_id);

    schedule();

    stop_timer(&v->poll_timer);

 out:
    clear_bit(_VCPUF_blocked, &v->vcpu_flags);
    return rc;
}

/* Voluntarily yield the processor for this allocation. */
static long do_yield(void)
{
    TRACE_2D(TRC_SCHED_YIELD, current->domain->domain_id, current->vcpu_id);
    set_bit(_VCPUF_yield, &current->vcpu_flags);

    schedule();

    return 0;
}

long do_sched_op_compat(int cmd, unsigned long arg)
{
    long ret = 0;

    switch ( cmd )
    {
    case SCHEDOP_yield:
    {
        ret = do_yield();
        break;
    }

    case SCHEDOP_block:
    {
        ret = do_block();
        break;
    }

    case SCHEDOP_shutdown:
    {
        TRACE_3D(TRC_SCHED_SHUTDOWN,
                 current->domain->domain_id, current->vcpu_id, arg);
        domain_shutdown(current->domain, (u8)arg);
        break;
    }

    default:
        ret = -ENOSYS;
    }

    return ret;
}


asmlinkage long do_sched_op(int cmd, XEN_GUEST_HANDLE(void) arg)
{
    long ret = 0;

    switch ( cmd )
    {
    case SCHEDOP_yield:
    {
        ret = do_yield();
        break;
    }

    case SCHEDOP_block:
    {
        ret = do_block();
        break;
    }

    case SCHEDOP_shutdown:
    {
        struct sched_shutdown sched_shutdown;

        ret = -EFAULT;
        if ( copy_from_guest(&sched_shutdown, arg, 1) )
            break;

        ret = 0;
        TRACE_3D(TRC_SCHED_SHUTDOWN,
                 current->domain->domain_id, current->vcpu_id,
                 sched_shutdown.reason);
        domain_shutdown(current->domain, (u8)sched_shutdown.reason); 

        break;
    }

    case SCHEDOP_poll:
    {
        struct sched_poll sched_poll;

        ret = -EFAULT;
        if ( copy_from_guest(&sched_poll, arg, 1) )
            break;

        ret = do_poll(&sched_poll);

        break;
    }

    case SCHEDOP_remote_shutdown:
    {
        struct domain *d;
        struct sched_remote_shutdown sched_remote_shutdown;

        if ( !IS_PRIV(current->domain) )
            return -EPERM;

        ret = -EFAULT;
        if ( copy_from_guest(&sched_remote_shutdown, arg, 1) )
            break;

        ret = -ESRCH;
        d = find_domain_by_id(sched_remote_shutdown.domain_id);
        if ( d == NULL )
            break;

        if(!acm_remote_shutdown(d))
            return -EPERM;

        domain_shutdown(d, (u8)sched_remote_shutdown.reason);
        put_domain(d);
        ret = 0;

        break;
    }

    default:
        ret = -ENOSYS;
    }

    return ret;
}

/* sched_id - fetch ID of current scheduler */
int sched_id(void)
{
    return ops.sched_id;
}

long sched_ctl(struct sched_ctl_cmd *cmd)
{
    if ( cmd->sched_id != ops.sched_id )
        return -EINVAL;

    SCHED_OP(control, cmd);
    TRACE_0D(TRC_SCHED_CTL);
    return 0;
}


/* Adjust scheduling parameter for a given domain. */
long sched_adjdom(struct sched_adjdom_cmd *cmd)
{
    struct domain *d;
    struct vcpu *v;
    
    if ( (cmd->sched_id != ops.sched_id) ||
         ((cmd->direction != SCHED_INFO_PUT) &&
          (cmd->direction != SCHED_INFO_GET)) )
        return -EINVAL;

    d = find_domain_by_id(cmd->domain);
    if ( d == NULL )
        return -ESRCH;

    /*
     * Most VCPUs we can simply pause. If we are adjusting this VCPU then
     * we acquire the local schedule_lock to guard against concurrent updates.
     *
     * We only acquire the local schedule lock after we have paused all other
     * VCPUs in this domain. There are two reasons for this:
     * 1- We don't want to hold up interrupts as pausing a VCPU can
     *    trigger a tlb shootdown.
     * 2- Pausing other VCPUs involves briefly locking the schedule
     *    lock of the CPU they are running on. This CPU could be the
     *    same as ours.
     */

    for_each_vcpu ( d, v )
    {
        if ( v != current )
            vcpu_pause(v);
    }

    if ( d == current->domain )
        vcpu_schedule_lock_irq(current);

    SCHED_OP(adjdom, d, cmd);
    TRACE_1D(TRC_SCHED_ADJDOM, d->domain_id);

    if ( d == current->domain )
        vcpu_schedule_unlock_irq(current);

    for_each_vcpu ( d, v )
    {
        if ( v != current )
            vcpu_unpause(v);
    }

    put_domain(d);

    return 0;
}

/* 
 * The main function
 * - deschedule the current domain (scheduler independent).
 * - pick a new domain (scheduler dependent).
 */
void schedule(void)
{
    struct vcpu        *prev = current, *next = NULL;
    int                 cpu = smp_processor_id();
    s_time_t            now = NOW();
    struct task_slice   next_slice;
    s32                 r_time;     /* time for new dom to run */

    ASSERT(!in_irq());

    spin_lock_irq(&schedule_data[cpu].schedule_lock);

    stop_timer(&schedule_data[cpu].s_timer);
	 
#ifdef CONFIG_VMM_SECURITY_ACM	 
    /* Inspect battery usage for scheduler adjustment.  */
    acm_check_battery_saving(prev->domain);
#endif		

    /* get policy-specific decision on scheduling... */
    next_slice = ops.do_schedule(now);

    r_time = next_slice.time;
    next = next_slice.task;

    schedule_data[cpu].curr = next;
    
    set_timer(&schedule_data[cpu].s_timer, now + r_time);

    if ( unlikely(prev == next) )
    {
        spin_unlock_irq(&schedule_data[cpu].schedule_lock);
        return continue_running(prev);
    }

    TRACE_2D(TRC_SCHED_SWITCH_INFPREV,
             prev->domain->domain_id,
             now - prev->runstate.state_entry_time);
    TRACE_3D(TRC_SCHED_SWITCH_INFNEXT,
             next->domain->domain_id,
             (next->runstate.state == RUNSTATE_runnable) ?
             (now - next->runstate.state_entry_time) : 0,
             r_time);

    ASSERT(prev->runstate.state == RUNSTATE_running);
    vcpu_runstate_change(
        prev,
        (test_bit(_VCPUF_blocked, &prev->vcpu_flags) ? RUNSTATE_blocked :
         (vcpu_runnable(prev) ? RUNSTATE_runnable : RUNSTATE_offline)),
        now);

    ASSERT(next->runstate.state != RUNSTATE_running);
    vcpu_runstate_change(next, RUNSTATE_running, now);

#if 0
    ASSERT(!test_bit(_VCPUF_running, &next->vcpu_flags));
#endif

    set_bit(_VCPUF_running, &next->vcpu_flags);

    spin_unlock_irq(&schedule_data[cpu].schedule_lock);		

    perfc_incrc(sched_ctx);

    stop_timer(&prev->periodic_timer);

    //prev->sleep_tick = schedule_data[cpu].tick;

    vcpu_periodic_timer_work(next);

    TRACE_4D(TRC_SCHED_SWITCH,
             prev->domain->domain_id, prev->vcpu_id,
             next->domain->domain_id, next->vcpu_id);

    context_switch(prev, next);
}

void context_saved(struct vcpu *prev)
{
	smp_wmb();

	clear_bit(_VCPUF_running, &(prev)->vcpu_flags);

	/* Check for migration request /after/ clearing running flags. */
	smp_mb();

	if (unlikely(test_bit(_VCPUF_migrating, &prev->vcpu_flags))) {
		printk("VCPU Migrate\n");
		vcpu_migrate(prev);
	}
}

void vcpu_force_reschedule(struct vcpu *v)
{
    vcpu_schedule_lock_irq(v);
    if ( test_bit(_VCPUF_running, &v->vcpu_flags))
        set_bit(_VCPUF_migrating, &v->vcpu_flags);
    vcpu_schedule_unlock_irq(v);

    if ( test_bit(_VCPUF_migrating, &v->vcpu_flags) )
    {
        vcpu_sleep_nosync(v);
        vcpu_migrate(v);
    }
}


/****************************************************************************
 * Timers: the scheduler utilises a number of timers
 * - s_timer: per CPU timer for preemption and scheduling decisions
 * - t_timer: per CPU periodic timer to send timer interrupt to current dom
 * - dom_timer: per domain timer to specifiy timeout values
 ****************************************************************************/

/* The scheduler timer: force a run through the scheduler */
static void s_timer_fn(void *unused)
{
    raise_softirq(SCHEDULE_SOFTIRQ); 
    perfc_incrc(sched_irq);
}

/* Periodic tick timer: send timer event to current domain */
void t_timer_fn(void *unused)
{
	struct vcpu *v   = current;
	unsigned int cpu = smp_processor_id();

	schedule_data[cpu].tick++;

#if 0
	if ( likely(!is_idle_vcpu(v)) )
	{
		//update_dom_time(v);
		send_timer_event(v);
	}
#endif

//	page_scrub_schedule_work();

	SCHED_OP(tick, cpu);

	set_timer(&t_timer[cpu], NOW() + MILLISECS(10));
}

/* Domain timer function, sends a virtual timer interrupt to domain */
static void vcpu_singleshot_timer_fn(void *data)
{
    struct vcpu *v = data;

    send_timer_event(v);
}


void vcpu_periodic_timer_work(struct vcpu *v)
{
    s_time_t now = NOW();
    uint64_t periodic_next_event;

    ASSERT(!active_timer(&v->periodic_timer));

    if (v->periodic_timer_period == 0)
        return;

    periodic_next_event = v->periodic_last_event + v->periodic_timer_period;

    if ( now >= periodic_next_event )
    {
        send_timer_event(v);

        v->periodic_last_event = now;
        periodic_next_event = now + v->periodic_timer_period;
    }

    v->periodic_timer.cpu = smp_processor_id();
    set_timer(&v->periodic_timer, periodic_next_event);
}

static void vcpu_periodic_timer_fn(void *data)
{
    struct vcpu *v = data;

    vcpu_periodic_timer_work(v);
}

/* SCHEDOP_poll timeout callback. */
static void poll_timer_fn(void *data)
{
    struct vcpu *v = data;
    vcpu_unblock(v);
}

/* Initialise the data structures. */
void __init scheduler_init(void)
{
    int i, rc;

    open_softirq(SCHEDULE_SOFTIRQ, schedule);

    for ( i = 0; i < NR_CPUS; i++ )
    {
        spin_lock_init(&schedule_data[i].schedule_lock);
        init_timer(&schedule_data[i].s_timer, s_timer_fn, NULL, i);
        init_timer(&t_timer[i], t_timer_fn, NULL, i);
    }

    for ( i = 0; schedulers[i] != NULL; i++ )
    {
        ops = *schedulers[i];
        if ( strcmp(ops.opt_name, opt_sched) == 0 )
            break;
    }
    
    if ( schedulers[i] == NULL )
        printk("Could not find scheduler: %s\n", opt_sched);

    printk("Using scheduler: %s (%s)\n", ops.name, ops.opt_name);

    SCHED_OP(init);
}

#if 0
/*
 * Start a scheduler for each CPU
 * This has to be done *after* the timers, e.g., APICs, have been initialised
 */
void schedulers_start(void) 
{   
    t_timer_fn(0);
    //smp_send_timer();
    smp_call_function((void *)t_timer_fn, NULL, 1, 1);
}
#endif

void dump_runq(unsigned char key)
{
    s_time_t      now = NOW();
    int           i;
    unsigned long flags;

    local_irq_save(flags);

    printk("Scheduler: %s (%s)\n", ops.name, ops.opt_name);
    SCHED_OP(dump_settings);
    printk("NOW=0x%08X%08X\n",  (u32)(now>>32), (u32)now); 

    for_each_online_cpu ( i )
    {
        spin_lock(&schedule_data[i].schedule_lock);
        printk("CPU[%02d] ", i);
        SCHED_OP(dump_cpu_state,i);
        spin_unlock(&schedule_data[i].schedule_lock);
    }

    local_irq_restore(flags);
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
