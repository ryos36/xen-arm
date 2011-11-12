
#ifndef __SCHED_H__
#define __SCHED_H__

#include <xen/config.h>
#include <xen/types.h>
#include <xen/spinlock.h>
#include <xen/smp.h>
#include <public/xen.h>
#include <public/dom0_ops.h>
#include <public/vcpu.h>
#include <xen/time.h>
#include <xen/timer.h>
#include <xen/grant_table.h>
#include <xen/rangeset.h>
#include <asm/domain.h>
#include <asm/atomic.h>

extern rwlock_t domlist_lock;

/* A global pointer to the initial domain (DOM0). */
extern struct domain *dom0;

#define MAX_EVTCHNS        NR_EVENT_CHANNELS
#define EVTCHNS_PER_BUCKET 128
#define NR_EVTCHN_BUCKETS  (MAX_EVTCHNS / EVTCHNS_PER_BUCKET)

struct evtchn
{
#define ECS_FREE         0 /* Channel is available for use.                  */
#define ECS_RESERVED     1 /* Channel is reserved.                           */
#define ECS_UNBOUND      2 /* Channel is waiting to bind to a remote domain. */
#define ECS_INTERDOMAIN  3 /* Channel is bound to another domain.            */
#define ECS_PIRQ         4 /* Channel is bound to a physical IRQ line.       */
#define ECS_VIRQ         5 /* Channel is bound to a virtual IRQ line.        */
#define ECS_IPI          6 /* Channel is bound to a virtual IPI line.        */
    u16 state;             /* ECS_* */
    u16 notify_vcpu_id;    /* VCPU for local delivery notification */
    union {
        struct {
            domid_t remote_domid;
	    u32 use;
        } unbound;     /* state == ECS_UNBOUND */
        struct {
            u16            remote_port;
            struct domain *remote_dom;
	    u32 use;
        } interdomain; /* state == ECS_INTERDOMAIN */
	/* use for ACM */
        u16 pirq;      /* state == ECS_PIRQ */
        u16 virq;      /* state == ECS_VIRQ */
    } u;
}__cacheline_aligned;

int  evtchn_init(struct domain *d);
void evtchn_destroy(struct domain *d);

struct vcpu 
{
    int              vcpu_id;

    int              processor;

    vcpu_info_t     *vcpu_info;

    struct domain   *domain;

    struct vcpu     *next_in_list;

    struct timer     singleshot_timer;         /* one-shot timer for timeout values */
    struct timer     periodic_timer;
    struct timer     poll_timer;    /* timeout for SCHEDOP_poll */

    uint64_t         periodic_last_event;
    uint64_t         periodic_timer_period;
    unsigned long    sleep_tick;    /* tick at which this vcpu started sleep */

    void            *sched_priv;    /* scheduler-specific data */

    struct vcpu_runstate_info runstate;

    unsigned long    vcpu_flags;

    u16              virq_to_evtchn[NR_VIRQS];

    atomic_t         pausecnt;

    /* Bitmask of CPUs on which this VCPU may run. */
    cpumask_t        cpu_affinity;

    unsigned long    nmi_addr;      /* NMI callback address. */
    /* Bitmask of CPUs which are holding onto this VCPU's state. */
    cpumask_t        vcpu_dirty_cpumask;

    struct arch_vcpu arch;
}__cacheline_aligned;

/* Per-domain lock can be recursively acquired in fault handlers. */
#define LOCK_BIGLOCK(_d)	spin_lock_recursive(&(_d)->big_lock)
#define UNLOCK_BIGLOCK(_d)	spin_unlock_recursive(&(_d)->big_lock)

struct domain
{
    domid_t          domain_id;

    struct shared_info *shared_info;     /* shared data area */

    spinlock_t       big_lock;

    spinlock_t	     page_alloc_lock; /* protects all the following fields  */
    struct list_head page_list;       /* linked list, of size tot_pages     */
    struct list_head xenpage_list;    /* linked list, of size xenheap_pages */
    unsigned int     tot_pages;       /* number of pages currently possesed */
    unsigned int     max_pages;       /* maximum value for tot_pages        */
    unsigned int     xenheap_pages;   /* # pages allocated from Xen heap    */

    /* Scheduling. */
    int              shutdown_code; /* code value from OS (if DOMF_shutdown) */
    void            *sched_priv;    /* scheduler-specific data */

    struct domain   *next_in_list;
    struct domain   *next_in_hashbucket;

    struct list_head rangesets;
    spinlock_t       rangesets_lock;

    /* Event channel information. */
    struct evtchn   *evtchn[NR_EVTCHN_BUCKETS];
    spinlock_t       evtchn_lock;

    grant_table_t   *grant_table;

    /*
     * Interrupt to event-channel mappings. Updates should be protected by the 
     * domain's event-channel spinlock. Read accesses can also synchronise on 
     * the lock, but races don't usually matter.
     */
#define NR_PIRQS 256 /* Put this somewhere sane! */
    u16             pirq_to_evtchn[NR_PIRQS];
    unsigned long   pirq_mask[NR_PIRQS/32];

    /* I/O capabilities (access to IRQs and memory-mapped I/O). */
    struct rangeset *iomem_caps;
    struct rangeset *irq_caps;
    struct rangeset *dma_caps;

    unsigned long    domain_flags;
    unsigned long    vm_assist;

    atomic_t         refcnt;

    struct vcpu      *vcpu[MAX_VIRT_CPUS];

    /* Bitmask of CPUs which are holding onto this domain's state. */
    cpumask_t        domain_dirty_cpumask;

    struct arch_domain arch;

    void *ssid; /* sHype security subject identifier */

    /* Control-plane tools handle for this domain. */
    xen_domain_handle_t handle;

    uint32_t      store_port;
    unsigned long store_mfn;

    uint32_t      console_port;
    unsigned long console_mfn;

    /* For ACM */
    unsigned long scid;
    uint8_t acm_batterylife;
    uint16_t acm_battery_save_mode;
    uint16_t acm_shutdown;
	
    uint32_t evtchn_count;
}__cacheline_aligned;

struct domain_setup_info
{
    /* Initialised by caller. */
    unsigned long image_addr;
    unsigned long image_len;

    unsigned long p_start;
    unsigned long p_end;

    /* Initialised by loader: Public. */
    unsigned long v_start;
    unsigned long v_end;
    unsigned long v_kernstart;
    unsigned long v_kernend;
    unsigned long v_kernentry;
    /* Initialised by loader: Private. */
    unsigned int  load_symtab;
    unsigned long symtab_addr;
    unsigned long symtab_len;
    /* Indicate whether it's xen specific image */
    char *xen_section_string;
#ifdef CONFIG_VMM_SECURITY_SCID
    /* security class id */
    unsigned long scid;
#endif

};

extern struct vcpu *idle_vcpu[NR_CPUS];
#define IDLE_DOMAIN_ID   (0x7FFFU)
#define is_idle_domain(d) ((d)->domain_id == IDLE_DOMAIN_ID)
#define is_idle_vcpu(v)   (is_idle_domain((v)->domain))

struct vcpu *alloc_vcpu(
    struct domain *d, unsigned int vcpu_id, unsigned int cpu_id);

struct domain *alloc_domain(void);
void free_domain(struct domain *d);

#define DOMAIN_DESTROYED (1<<31) /* assumes atomic_t is >= 32 bits */
#define put_domain(_d) \
  if ( atomic_dec_and_test(&(_d)->refcnt) ) domain_destroy(_d)

/*
 * Use this when you don't have an existing reference to @d. It returns
 * FALSE if @d is being destroyed.
 */
static always_inline int get_domain(struct domain *d)
{
    atomic_t old, new, seen = d->refcnt;
    do
    {
        old = seen;
        if ( unlikely(_atomic_read(old) & DOMAIN_DESTROYED) )
            return 0;
        _atomic_set(new, _atomic_read(old) + 1);
        seen = atomic_compareandswap(old, new, &d->refcnt);
    }
    while ( unlikely(_atomic_read(seen) != _atomic_read(old)) );
    return 1;
}

/*
 * Use this when you already have, or are borrowing, a reference to @d.
 * In this case we know that @d cannot be destroyed under our feet.
 */
static inline void get_knownalive_domain(struct domain *d)
{
    atomic_inc(&d->refcnt);
    ASSERT(!(atomic_read(&d->refcnt) & DOMAIN_DESTROYED));
}

extern struct domain *domain_create(domid_t dom_id, unsigned int cpu);
extern int construct_dom0(
    struct domain *d,
    unsigned long guest_start_paddr, unsigned long guest_size,
    unsigned long image_start, unsigned long image_len, 
    unsigned long initrd_start, unsigned long initrd_len,
    char *cmdline);

extern int set_info_guest(struct domain *d, dom0_setvcpucontext_t *);

struct domain *find_domain_by_id(domid_t dom);
extern void domain_destroy(struct domain *d);
extern void domain_kill(struct domain *d);
extern void domain_shutdown(struct domain *d, u8 reason);
extern void domain_pause_for_debugger(void);

/*
 * Mark specified domain as crashed. This function always returns, even if the
 * caller is the specified domain. The domain is not synchronously descheduled
 * from any processor.
 */
extern void __domain_crash(struct domain *d);
#define domain_crash(d) do {                                              \
    printk("domain_crash called from %s:%d\n", __FILE__, __LINE__);       \
    __domain_crash(d);                                                    \
} while (0)

/*
 * Mark current domain as crashed and synchronously deschedule from the local
 * processor. This function never returns.
 */
extern void __domain_crash_synchronous(void) __attribute__((noreturn));
#define domain_crash_synchronous() do {                                   \
    printk("domain_crash_sync called from %s:%d\n", __FILE__, __LINE__);  \
    __domain_crash_synchronous();                                         \
} while (0)

#define set_current_state(_s) do { current->state = (_s); } while (0)
void scheduler_init(void);
void schedulers_start(void);
void sched_add_domain(struct vcpu *);
void sched_rem_domain(struct vcpu *);
long sched_ctl(struct sched_ctl_cmd *);
long sched_adjdom(struct sched_adjdom_cmd *);
int  sched_id(void);
void vcpu_wake(struct vcpu *d);
void vcpu_sleep_nosync(struct vcpu *d);
void vcpu_sleep_sync(struct vcpu *d);

/*
 * Force synchronisation of given VCPU's state. If it is currently descheduled,
 * this call will ensure that all its state is committed to memory and that
 * no CPU is using critical state (e.g., page tables) belonging to the VCPU.
 */
extern void sync_vcpu_execstate(struct vcpu *v);

/*
 * Called by the scheduler to switch to another VCPU. This function must
 * call context_saved(@prev) when the local CPU is no longer running in
 * @prev's context, and that context is saved to memory. Alternatively, if
 * implementing lazy context switching, it suffices to ensure that invoking
 * sync_vcpu_execstate() will switch and commit @prev's state.
 */
extern void context_switch(struct vcpu *prev, struct vcpu *next);

/*
 * As described above, context_switch() must call this function when the
 * local CPU is no longer running in @prev's context, and @prev's context is
 * saved to memory. Alternatively, if implementing lazy context switching,
 * ensure that invoking sync_vcpu_execstate() will switch and commit @prev.
 */
extern void context_saved(struct vcpu *prev);

/* Called by the scheduler to continue running the current VCPU. */
extern void continue_running(struct vcpu *same);

void startup_cpu_idle_loop(void);

/*
 * Creates a continuation to resume the current hypercall. The caller should
 * return immediately, propagating the value returned from this invocation.
 * The format string specifies the types and number of hypercall arguments.
 * It contains one character per argument as follows:
 *  'i' [unsigned] {char, int}
 *  'l' [unsigned] long
 *  'h' guest handle (GUEST_HANDLE(foo))
 */
unsigned long hypercall_create_continuation(
    unsigned int op, const char *format, ...);

#define hypercall_preempt_check() (unlikely(    \
        softirq_pending(smp_processor_id()) |   \
        guest_events_need_delivery(current)     \
    ))

/* This domain_hash and domain_list are protected by the domlist_lock. */
#define DOMAIN_HASH_SIZE 256
#define DOMAIN_HASH(_id) ((int)(_id)&(DOMAIN_HASH_SIZE-1))
extern struct domain *domain_hash[DOMAIN_HASH_SIZE];
extern struct domain *domain_list;

#define for_each_domain(_d)                     \
 for ( (_d) = domain_list;                      \
       (_d) != NULL;                            \
       (_d) = (_d)->next_in_list )

#define for_each_vcpu(_d,_v)                    \
 for ( (_v) = (_d)->vcpu[0];                    \
       (_v) != NULL;                            \
       (_v) = (_v)->next_in_list )

/*
 * Per-VCPU flags (vcpu_flags).
 */
 /* Has the FPU been initialised? */
#define _VCPUF_fpu_initialised 0
#define VCPUF_fpu_initialised  (1UL<<_VCPUF_fpu_initialised)
 /* Has the FPU been used since it was last saved? */
#define _VCPUF_fpu_dirtied     1
#define VCPUF_fpu_dirtied      (1UL<<_VCPUF_fpu_dirtied)
 /* Domain is blocked waiting for an event. */
#define _VCPUF_blocked         2
#define VCPUF_blocked          (1UL<<_VCPUF_blocked)
 /* Currently running on a CPU? */
#define _VCPUF_running         3
#define VCPUF_running          (1UL<<_VCPUF_running)
 /* Initialization completed. */
#define _VCPUF_initialised     4
#define VCPUF_initialised      (1UL<<_VCPUF_initialised)
 /* VCPU is not-runnable */
#define _VCPUF_down            5
#define VCPUF_down             (1UL<<_VCPUF_down)
 /* NMI callback pending for this VCPU? */
#define _VCPUF_nmi_pending     8
#define VCPUF_nmi_pending      (1UL<<_VCPUF_nmi_pending)
 /* Avoid NMI reentry by allowing NMIs to be masked for short periods. */
#define _VCPUF_nmi_masked      9
#define VCPUF_nmi_masked       (1UL<<_VCPUF_nmi_masked)
/* To get the reason of __enter_scheduler, added VCPUF_yield */
#define _VCPUF_yield	    10
#define VCPUF_yield     	 (1UL<<_VCPUF_yield)

#define _VCPUF_migrating	11
#define VCPUF_migrating		(1UL<<_VCPUF_migrating)
/*
 * Per-domain flags (domain_flags).
 */
 /* Is this domain privileged? */
#define _DOMF_privileged       0
#define DOMF_privileged        (1UL<<_DOMF_privileged)
 /* Guest shut itself down for some reason. */
#define _DOMF_shutdown         1
#define DOMF_shutdown          (1UL<<_DOMF_shutdown)
 /* Death rattle. */
#define _DOMF_dying            2
#define DOMF_dying             (1UL<<_DOMF_dying)
 /* Domain is paused by controller software. */
#define _DOMF_ctrl_pause       3
#define DOMF_ctrl_pause        (1UL<<_DOMF_ctrl_pause)
 /* Domain is being debugged by controller software. */
#define _DOMF_debugging        4
#define DOMF_debugging         (1UL<<_DOMF_debugging)

static inline int vcpu_runnable(struct vcpu *v)
{
    return ( (atomic_read(&v->pausecnt) == 0) &&
             !(v->vcpu_flags & (VCPUF_blocked|VCPUF_down|VCPUF_migrating)) &&
             !(v->domain->domain_flags & (DOMF_shutdown|DOMF_ctrl_pause)) );
}

void vcpu_pause(struct vcpu *v);
void domain_pause(struct domain *d);
void vcpu_unpause(struct vcpu *v);
void domain_unpause(struct domain *d);
void domain_pause_by_systemcontroller(struct domain *d);
void domain_unpause_by_systemcontroller(struct domain *d);
void cpu_init(void);

int vcpu_set_affinity(struct vcpu *v, cpumask_t *affinity);

void vcpu_runstate_get(struct vcpu *v, struct vcpu_runstate_info *runstate);

static inline void vcpu_unblock(struct vcpu *v)
{
    if ( test_and_clear_bit(_VCPUF_blocked, &v->vcpu_flags) )
        vcpu_wake(v);
}

#ifdef CONFIG_SUBSTITUTE_IS_PRIV
#define IS_PRIV(_d) 1
#else
#define IS_PRIV(_d)                                         \
    (test_bit(_DOMF_privileged, &(_d)->domain_flags))
#endif

#define VM_ASSIST(_d,_t) (test_bit((_t), &(_d)->vm_assist))

#endif /* __SCHED_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
