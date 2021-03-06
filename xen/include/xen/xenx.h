/******************************************************************************
 * xen.h
 * 
 * Guest OS interface to Xen.
 * 
 * Copyright (c) 2004, K A Fraser
 */

#ifndef __XEN_PUBLIC_XEN_H__
#define __XEN_PUBLIC_XEN_H__

#if defined(__i386__)
#include "arch-x86_32.h"
#elif defined(__x86_64__)
#include "arch-x86_64.h"
#elif defined(__ia64__)
#include "arch-ia64.h"

#elif defined(__arm__)	
#include "arch-arm.h"
#else
#error "Unsupported architecture"
#endif


/*
 * for domain shared info mapping
 */
#define DOM_SHARED_INFO_PAGE_BASE_VADDR 0xC0009000


/*
 * XEN "SYSTEM CALLS" (a.k.a. HYPERCALLS).
 */

/*
 * x86_32: EAX = vector; EBX, ECX, EDX, ESI, EDI = args 1, 2, 3, 4, 5.
 *         EAX = return value
 *         (argument registers may be clobbered on return)
 * x86_64: RAX = vector; RDI, RSI, RDX, R10, R8, R9 = args 1, 2, 3, 4, 5, 6. 
 *         RAX = return value
 *         (argument registers not clobbered on return; RCX, R11 are)
 */
#define __HYPERVISOR_set_trap_table        0
#define __HYPERVISOR_mmu_update            1
#define __HYPERVISOR_set_gdt               2
#define __HYPERVISOR_stack_switch          3
#define __HYPERVISOR_set_callbacks         4
#define __HYPERVISOR_fpu_taskswitch        5
#define __HYPERVISOR_sched_op_compat       6 /* compat as of 0x00030101 */
#define __HYPERVISOR_dom0_op               7
#define __HYPERVISOR_set_debugreg          8
#define __HYPERVISOR_get_debugreg          9
#define __HYPERVISOR_update_descriptor    10
#define __HYPERVISOR_memory_op            12
#define __HYPERVISOR_multicall            13
#define __HYPERVISOR_update_va_mapping    14
#define __HYPERVISOR_set_timer_op         15
#define __HYPERVISOR_event_channel_op     16
#define __HYPERVISOR_xen_version          17
#define __HYPERVISOR_console_io           18
#define __HYPERVISOR_physdev_op           19
#define __HYPERVISOR_grant_table_op       20
#define __HYPERVISOR_vm_assist            21
#define __HYPERVISOR_update_va_mapping_otherdomain 22
#define __HYPERVISOR_iret                 23 /* x86 only */
#define __HYPERVISOR_vcpu_op              24
#define __HYPERVISOR_set_segment_base     25 /* x86/64 only */
#define __HYPERVISOR_mmuext_op            26
#define __HYPERVISOR_acm_op               27
#define __HYPERVISOR_nmi_op               28
#define __HYPERVISOR_sched_op             29
#define __HYPERVISOR_set_cpu_domain       30
#define __HYPERVISOR_restore_guest_context 31
#define __HYPERVISOR_do_print_profile	  32
#define __HYPERVISOR_do_set_foreground_domain 33
#define __HYPERVISOR_do_set_HID_irq	   34
#define __HYPERVISOR_dummy		   35
#define __HYPERVISOR_sra_op		   36
#define __HYPERVISOR_dma_op		   37
#define __HYPERVISOR_get_system_time 38
#define __HYPERVISOR_do_gcov_op		39

/* 
 * VIRTUAL INTERRUPTS
 * 
 * Virtual interrupts that a guest OS may receive from Xen.
 */
#define VIRQ_TIMER      0  /* Timebase update, and/or requested timeout.  */
#define VIRQ_DEBUG      1  /* Request guest to dump debug info.           */
#define VIRQ_CONSOLE    2  /* (DOM0) Bytes received on emergency console. */
#define VIRQ_DOM_EXC    3  /* (DOM0) Exceptional event for some domain.   */
#define VIRQ_DEBUGGER   6  /* (DOM0) A domain has paused for debugging.   */
#define VIRQ_DOM_TIMER	8
#define VIRQ_DOM_CRASH	9
#define VIRQ_DDOM_POLICY 10 /* (Driver domain) Notify policy update */
#define VIRQ_BATTERY_CONTROL 11 /* (Control domain) Control battery usage by shutting down unnecessary domains */
#define NR_VIRQS        12

/* ------------------ why next definition is omitted?? */
#define VIRQ_TBUF       4  /* G. (DOM0) Trace buffer has records available.  */
#define VIRQ_XENOPROF   7  /* V. XenOprofile interrupt: new sample available */

/*
 * MMU-UPDATE REQUESTS
 * 
 * HYPERVISOR_mmu_update() accepts a list of (ptr, val) pairs.
 * A foreigndom (FD) can be specified (or DOMID_SELF for none).
 * Where the FD has some effect, it is described below.
 * ptr[1:0] specifies the appropriate MMU_* command.
 * 
 * ptr[1:0] == MMU_NORMAL_PT_UPDATE:
 * Updates an entry in a page table. If updating an L1 table, and the new
 * table entry is valid/present, the mapped frame must belong to the FD, if
 * an FD has been specified. If attempting to map an I/O page then the
 * caller assumes the privilege of the FD.
 * FD == DOMID_IO: Permit /only/ I/O mappings, at the priv level of the caller.
 * FD == DOMID_XEN: Map restricted areas of Xen's heap space.
 * ptr[:2]  -- Machine address of the page-table entry to modify.
 * val      -- Value to write.
 * 
 * ptr[1:0] == MMU_MACHPHYS_UPDATE:
 * Updates an entry in the machine->pseudo-physical mapping table.
 * ptr[:2]  -- Machine address within the frame whose mapping to modify.
 *             The frame must belong to the FD, if one is specified.
 * val      -- Value to write into the mapping entry.
 */
#define MMU_NORMAL_PT_UPDATE     0 /* checked '*ptr = val'. ptr is MA.       */
#define MMU_MACHPHYS_UPDATE      1 /* ptr = MA of frame to modify entry for  */

/*
 * MMU EXTENDED OPERATIONS
 * 
 * HYPERVISOR_mmuext_op() accepts a list of mmuext_op structures.
 * A foreigndom (FD) can be specified (or DOMID_SELF for none).
 * Where the FD has some effect, it is described below.
 * 
 * cmd: MMUEXT_(UN)PIN_*_TABLE
 * mfn: Machine frame number to be (un)pinned as a p.t. page.
 *      The frame must belong to the FD, if one is specified.
 * 
 * cmd: MMUEXT_NEW_BASEPTR
 * mfn: Machine frame number of new page-table base to install in MMU.
 * 
 * cmd: MMUEXT_NEW_USER_BASEPTR [x86/64 only]
 * mfn: Machine frame number of new page-table base to install in MMU
 *      when in user space.
 * 
 * cmd: MMUEXT_TLB_FLUSH_LOCAL
 * No additional arguments. Flushes local TLB.
 * 
 * cmd: MMUEXT_INVLPG_LOCAL
 * linear_addr: Linear address to be flushed from the local TLB.
 * 
 * cmd: MMUEXT_TLB_FLUSH_MULTI
 * vcpumask: Pointer to bitmap of VCPUs to be flushed.
 * 
 * cmd: MMUEXT_INVLPG_MULTI
 * linear_addr: Linear address to be flushed.
 * vcpumask: Pointer to bitmap of VCPUs to be flushed.
 * 
 * cmd: MMUEXT_TLB_FLUSH_ALL
 * No additional arguments. Flushes all VCPUs' TLBs.
 * 
 * cmd: MMUEXT_INVLPG_ALL
 * linear_addr: Linear address to be flushed from all VCPUs' TLBs.
 * 
 * cmd: MMUEXT_FLUSH_CACHE
 * No additional arguments. Writes back and flushes cache contents.
 * 
 * cmd: MMUEXT_SET_LDT
 * linear_addr: Linear address of LDT base (NB. must be page-aligned).
 * nr_ents: Number of entries in LDT.
 */
#define MMUEXT_PIN_L1_TABLE      	0
#define MMUEXT_PIN_L2_TABLE      	1
#define MMUEXT_PIN_L3_TABLE      	2
#define MMUEXT_PIN_L4_TABLE      	3
#define MMUEXT_UNPIN_TABLE       	4
#define MMUEXT_NEW_BASEPTR       	5
#define MMUEXT_TLB_FLUSH_LOCAL   	6
#define MMUEXT_INVLPG_LOCAL      	7
#define MMUEXT_TLB_FLUSH_MULTI   	8
#define MMUEXT_INVLPG_MULTI      	9
#define MMUEXT_TLB_FLUSH_ALL    	10
#define MMUEXT_INVLPG_ALL       	11
#define MMUEXT_FLUSH_CACHE      	12
#define MMUEXT_SET_LDT          	13
#define MMUEXT_NEW_USER_BASEPTR 	15
#define MMUEXT_DCACHE_CLEAN_AREA 	16		/* for cpu_arm926_dcache_clean_area */
#define MMUEXT_FLUSH_TLB_PAGE 		17		/* for flush_tlb_page */
#define MMUEXT_FLUSH_TLB_KERNEL_PAGE 	18		/* for flush_tlb_kernel_page */
#define MMUEXT_FLUSH_PMD_ENTRY		19		/* for flush_pmd_entry */
#define MMUEXT_CLEAN_PMD_ENTRY		20		/* for clean_pmd_entry */
#define MMUEXT_FLUSH_USER_TLB_RANGE	21		/* for v4wbi_flush_user_tlb_range */
#define MMUEXT_FLUSH_KERN_TLB_RANGE	22		/* for v4wbi_flush_kern_tlb_range */
#define MMUEXT_COHERENT_KERN_RANGE	23		/* for arm926_coherent_kern_range */
#define MMUEXT_COHERENT_USER_RANGE	24		/* for arm926_coherent_user_range */
#define MMUEXT_FLUSH_USER_CACHE_RANGE 	25		/* for arm926_flush_user_cache_range */
#define MMUEXT_FLUSH_KERN_DCACHE_PAGE 	26		/* for arm926_flush_kern_dcache_page */
#define MMUEXT_CLEAR_USER_PAGE		27		/* for v4wb_clear_user_page */
#define MMUEXT_COPY_USER_PAGE		28		/* for v4wb_copy_user_page */
#define MMUEXT_GET_PGD			29		/* for cpu_get_pgd */
#define MMUEXT_INVALIDATE_I_TLB_ENTRY 	30
#define MMUEXT_INVALIDATE_D_TLB_ENTRY 	31
#define MMUEXT_DRAIN_WB			32
#define MMUEXT_DMAC_INV_RANGE		33		/* for arm926_dma_inv_range */
#define MMUEXT_DMAC_CLEAN_RANGE		34		/* for arm926_dma_clean_range */
#define MMUEXT_DMAC_FLUSH_RANGE		35		/* for arm926_dma_flush_range */
#define MMUEXT_FLUSH_TLB_ALL		36		/* for flush_tlb_all */
#define MMUEXT_FLUSH_TLB_MM		37		/* for flush_tlb_mm */
#define MMUEXT_DEBUG_TLB_DUMP		38		/* for tlb dump */

#define DMAOP_ENABLE			0
#define DMAOP_DISABLE			1
#define DMAOP_START			2
#define DMAOP_ACK_INT			3
#define DMAOP_REQUEST			4
#define DMAOP_RELEASE			5
#define DMAOP_SET_COUNT			6
#define DMAOP_SET_TIMEOUT		7
#define DMAOP_SET_ADDRESS		8
#define DMAOP_SET_CONFIG		9
#define DMAOP_GET_STATUS		10
#define DMAOP_GET_CONFIG		11
#define DMAOP_GET_RESIDUE		12

#ifndef __ASSEMBLY__
struct dma_status {
	unsigned long interrupt;
	unsigned long burst_timeout;
	unsigned long transfer_error;
	unsigned long buffer_overflow;
	unsigned long request_timeout;
};

struct dma_config {
	unsigned long length;
	unsigned long source_address;
	unsigned long destination_address;
	unsigned long request;
	unsigned long count;
	unsigned long direction;
	unsigned long source_type;
	unsigned long destination_type;
	unsigned long source_port;
	unsigned long destination_port;
	unsigned long repeat;
};

typedef struct dma_op {
	unsigned int	cmd;
	unsigned int	flags;
	unsigned int	channel;
	union {
		unsigned long	address;
		unsigned long	count;
		unsigned long	timeout;
	}param;
	struct dma_status status;
	struct dma_config config;
}dma_op_t;
DEFINE_GUEST_HANDLE(dma_op_t);

typedef struct mmuext_op {
    unsigned int cmd;
    union {
        /* [UN]PIN_TABLE, NEW_BASEPTR, NEW_USER_BASEPTR */
        unsigned long mfn;
        /* INVLPG_LOCAL, INVLPG_ALL, SET_LDT */
        unsigned long linear_addr;
        unsigned long phys_addr;
        unsigned int  flags;
    } arg1;
    union {
        /* SET_LDT */
        unsigned int nr_ents;
        /* TLB_FLUSH_MULTI, INVLPG_MULTI */
        void *vcpumask;
        unsigned int size;
        unsigned long end;
        unsigned long addr;
    } arg2;
    union {
        unsigned int flags;
        unsigned long addr;
    } arg3;
} mmuext_op_t;
DEFINE_GUEST_HANDLE(mmuext_op_t);
#endif

/* These are passed as 'flags' to update_va_mapping. They can be ORed. */
/* When specifying UVMF_MULTI, also OR in a pointer to a CPU bitmap.   */
/* UVMF_LOCAL is merely UVMF_MULTI with a NULL bitmap pointer.         */
#define UVMF_NONE               (0UL<<0) /* No flushing at all.   */
#define UVMF_TLB_FLUSH          (1UL<<0) /* Flush entire TLB(s).  */
#define UVMF_INVLPG             (2UL<<0) /* Flush only one entry. */
#define UVMF_FLUSHTYPE_MASK     (3UL<<0)
#define UVMF_MULTI              (0UL<<2) /* Flush subset of TLBs. */
#define UVMF_LOCAL              (0UL<<2) /* Flush local TLB.      */
#define UVMF_ALL                (1UL<<2) /* Flush all TLBs.       */
#define UVMF_SHARED_INFO		(1UL<<3) /* this is a request for establishing a shared info page */

/*
 * Commands to HYPERVISOR_console_io().
 */
#define CONSOLEIO_write         0
#define CONSOLEIO_read          1

/*
 * Commands to HYPERVISOR_vm_assist().
 */
#define VMASST_CMD_enable                0
#define VMASST_CMD_disable               1
#define VMASST_TYPE_4gb_segments         0
#define VMASST_TYPE_4gb_segments_notify  1
#define VMASST_TYPE_writable_pagetables  2
#define MAX_VMASST_TYPE 2




#ifndef __ASSEMBLY__

typedef uint16_t domid_t;

/* Domain ids >= DOMID_FIRST_RESERVED cannot be used for ordinary domains. */
#define DOMID_FIRST_RESERVED (0x7FF0U)

/* DOMID_SELF is used in certain contexts to refer to oneself. */
#define DOMID_SELF (0x7FF0U)

/*
 * DOMID_IO is used to restrict page-table updates to mapping I/O memory.
 * Although no Foreign Domain need be specified to map I/O pages, DOMID_IO
 * is useful to ensure that no mappings to the OS's own heap are accidentally
 * installed. (e.g., in Linux this could cause havoc as reference counts
 * aren't adjusted on the I/O-mapping code path).
 * This only makes sense in MMUEXT_SET_FOREIGNDOM, but in that context can
 * be specified by any calling domain.
 */
#define DOMID_IO   (0x7FF1U)

/*
 * DOMID_XEN is used to allow privileged domains to map restricted parts of
 * Xen's heap space (e.g., the machine_to_phys table).
 * This only makes sense in MMUEXT_SET_FOREIGNDOM, and is only permitted if
 * the caller is privileged.
 */
#define DOMID_XEN  (0x7FF2U)

/*
 * Send an array of these to HYPERVISOR_mmu_update().
 * NB. The fields are natural pointer/address size for this architecture.
 */
typedef struct mmu_update {
    uint64_t ptr;       /* Machine address of PTE. */
    uint64_t val;       /* New contents of PTE.    */
} mmu_update_t;
DEFINE_GUEST_HANDLE(mmu_update_t);

/*
 * Send an array of these to HYPERVISOR_multicall().
 * NB. The fields are natural register size for this architecture.
 */
typedef struct multicall_entry {
    unsigned long op, result;
    unsigned long args[6];
} multicall_entry_t;
DEFINE_GUEST_HANDLE(multicall_entry_t);

/*
 * Event channel endpoints per domain:
 *  1024 if a long is 32 bits; 4096 if a long is 64 bits.
 */
#define NR_EVENT_CHANNELS (sizeof(unsigned long) * sizeof(unsigned long) * 64)

typedef struct vcpu_time_info {
    /*
     * Updates to the following values are preceded and followed by an
     * increment of 'version'. The guest can therefore detect updates by
     * looking for changes to 'version'. If the least-significant bit of
     * the version number is set then an update is in progress and the guest
     * must wait to read a consistent set of values.
     * The correct way to interact with the version number is similar to
     * Linux's seqlock: see the implementations of read_seqbegin/read_seqretry.
     */
    uint32_t version;
    uint32_t pad0;
    //uint64_t tsc_timestamp;   /* TSC at last update of time vals.  */
    uint64_t offset;		/* timer offset, in nanosecs, since last tick */
    uint64_t system_time;     /* Time, in nanosecs, since boot.    */
    /*
     * Current system time:
     *   system_time + ((tsc - tsc_timestamp) << tsc_shift) * tsc_to_system_mul
     * CPU frequency (Hz):
     *   ((10^9 << 32) / tsc_to_system_mul) >> tsc_shift
     */
    uint32_t tsc_to_system_mul;
    int8_t   tsc_shift;
    int8_t   pad1[3];
} vcpu_time_info_t; /* 32 bytes */

typedef struct vcpu_info {
    /*
     * 'evtchn_upcall_pending' is written non-zero by Xen to indicate
     * a pending notification for a particular VCPU. It is then cleared 
     * by the guest OS /before/ checking for pending work, thus avoiding
     * a set-and-check race. Note that the mask is only accessed by Xen
     * on the CPU that is currently hosting the VCPU. This means that the
     * pending and mask flags can be updated by the guest without special
     * synchronisation (i.e., no need for the x86 LOCK prefix).
     * This may seem suboptimal because if the pending flag is set by
     * a different CPU then an IPI may be scheduled even when the mask
     * is set. However, note:
     *  1. The task of 'interrupt holdoff' is covered by the per-event-
     *     channel mask bits. A 'noisy' event that is continually being
     *     triggered can be masked at source at this very precise
     *     granularity.
     *  2. The main purpose of the per-VCPU mask is therefore to restrict
     *     reentrant execution: whether for concurrency control, or to
     *     prevent unbounded stack usage. Whatever the purpose, we expect
     *     that the mask will be asserted only for short periods at a time,
     *     and so the likelihood of a 'spurious' IPI is suitably small.
     * The mask is read before making an event upcall to the guest: a
     * non-zero mask therefore guarantees that the VCPU will not receive
     * an upcall activation. The mask is cleared when the VCPU requests
     * to block: this avoids wakeup-waiting races.
     */
    uint8_t evtchn_upcall_pending;
    uint8_t evtchn_upcall_mask;
    unsigned long evtchn_pending_sel;
    arch_vcpu_info_t arch;
    vcpu_time_info_t time;
} vcpu_info_t; /* 64 bytes (x86) */

#define MAX_TIME_STAMP 1
#define MAX_LOG_TIME_STAMP 256
#define MAX_EVENT_LOG 1
/*
 * Xen/kernel shared data -- pointer provided in start_info.
 * NB. We expect that this struct is smaller than a page.
 */
typedef struct shared_info {
    vcpu_info_t vcpu_info[MAX_VIRT_CPUS];

    /*
     * A domain can create "event channels" on which it can send and receive
     * asynchronous event notifications. There are three classes of event that
     * are delivered by this mechanism:
     *  1. Bi-directional inter- and intra-domain connections. Domains must
     *     arrange out-of-band to set up a connection (usually by allocating
     *     an unbound 'listener' port and avertising that via a storage service
     *     such as xenstore).
     *  2. Physical interrupts. A domain with suitable hardware-access
     *     privileges can bind an event-channel port to a physical interrupt
     *     source.
     *  3. Virtual interrupts ('events'). A domain can bind an event-channel
     *     port to a virtual interrupt source, such as the virtual-timer
     *     device or the emergency console.
     * 
     * Event channels are addressed by a "port index". Each channel is
     * associated with two bits of information:
     *  1. PENDING -- notifies the domain that there is a pending notification
     *     to be processed. This bit is cleared by the guest.
     *  2. MASK -- if this bit is clear then a 0->1 transition of PENDING
     *     will cause an asynchronous upcall to be scheduled. This bit is only
     *     updated by the guest. It is read-only within Xen. If a channel
     *     becomes pending while the channel is masked then the 'edge' is lost
     *     (i.e., when the channel is unmasked, the guest must manually handle
     *     pending notifications as no upcall will be scheduled by Xen).
     * 
     * To expedite scanning of pending notifications, any 0->1 pending
     * transition on an unmasked channel causes a corresponding bit in a
     * per-vcpu selector word to be set. Each bit in the selector covers a
     * 'C long' in the PENDING bitfield array.
     */
    unsigned long evtchn_pending[sizeof(unsigned long) * 8];
    unsigned long evtchn_mask[sizeof(unsigned long) * 8];

    /*
     * Wallclock time: updated only by control software. Guests should base
     * their gettimeofday() syscall on this wallclock-base value.
     */
    uint32_t wc_version;      /* Version counter: see vcpu_time_info_t. */
    uint32_t wc_sec;          /* Secs  00:00:00 UTC, Jan 1, 1970.  */
    uint32_t wc_nsec;         /* Nsecs 00:00:00 UTC, Jan 1, 1970.  */

	arch_shared_info_t arch;
	uint32_t	lost_ticks;

	unsigned long prof_time_count[MAX_TIME_STAMP];
	unsigned long prof_start_time[MAX_TIME_STAMP];
	unsigned long prof_start_flag[MAX_TIME_STAMP];
	unsigned long long prof_time_sum[MAX_TIME_STAMP];

	unsigned int prof_id_log[MAX_LOG_TIME_STAMP];
	unsigned long prof_time_stamp_log[MAX_LOG_TIME_STAMP];
	unsigned int curr_index;
	unsigned int log_enabled;

	unsigned long long time_prof_log[MAX_EVENT_LOG];
	unsigned int time_prof_id[MAX_EVENT_LOG];
	unsigned int time_prof_index;

	/* for access control */
	unsigned int batterylife;
	domid_t shutdown_domain;

} shared_info_t;

/*
 * Start-of-day memory layout for the initial domain (DOM0):
 *  1. The domain is started within contiguous virtual-memory region.
 *  2. The contiguous region begins and ends on an aligned 4MB boundary.
 *  3. The region start corresponds to the load address of the OS image.
 *     If the load address is not 4MB aligned then the address is rounded down.
 *  4. This the order of bootstrap elements in the initial virtual region:
 *      a. relocated kernel image
 *      b. initial ram disk              [mod_start, mod_len]
 *      c. list of allocated page frames [mfn_list, nr_pages]
 *      d. start_info_t structure        [register ESI (x86)]
 *      e. bootstrap page tables         [pt_base, CR3 (x86)]
 *      f. bootstrap stack               [register ESP (x86)]
 *  5. Bootstrap elements are packed together, but each is 4kB-aligned.
 *  6. The initial ram disk may be omitted.
 *  7. The list of page frames forms a contiguous 'pseudo-physical' memory
 *     layout for the domain. In particular, the bootstrap virtual-memory
 *     region is a 1:1 mapping to the first section of the pseudo-physical map.
 *  8. All bootstrap elements are mapped read-writable for the guest OS. The
 *     only exception is the bootstrap page table, which is mapped read-only.
 *  9. There is guaranteed to be at least 512kB padding after the final
 *     bootstrap element. If necessary, the bootstrap virtual region is
 *     extended by an extra 4MB to ensure this.
 */

#define MAX_GUEST_CMDLINE 1024
typedef struct start_info {
    /* THE FOLLOWING ARE FILLED IN BOTH ON INITIAL BOOT AND ON RESUME.    */
    char magic[32];             /* "xen-<version>-<platform>".            */
    unsigned long nr_pages;     /* Total pages allocated to this domain.  */
    unsigned long shared_info;  /* MACHINE address of shared info struct. */
    uint32_t flags;             /* SIF_xxx flags.                         */
    unsigned long store_mfn;    /* MACHINE page number of shared page.    */
    uint32_t store_evtchn;      /* Event channel for store communication. */
    unsigned long console_mfn;  /* MACHINE address of console page.       */
    uint32_t console_evtchn;    /* Event channel for console messages.    */
    unsigned int store_mfn_gref;  /* to make store_mfn -kcr */	
    /* THE FOLLOWING ARE ONLY FILLED IN ON INITIAL BOOT (NOT RESUME).     */
    unsigned long pt_base;      /* VIRTUAL address of page directory.     */
    unsigned long nr_pt_frames; /* Number of bootstrap p.t. frames.       */
    unsigned long mfn_list;     /* VIRTUAL address of page-frame list.    */
    unsigned long mod_start;    /* VIRTUAL address of pre-loaded module.  */
    unsigned long mod_len;      /* Size (bytes) of pre-loaded module.     */
    int8_t cmd_line[MAX_GUEST_CMDLINE];
    unsigned long min_mfn;		/* min mfn (min_page in xen) 			  */	
} start_info_t;

/* These flags are passed in the 'flags' field of start_info_t. */
#define SIF_PRIVILEGED    (1<<0)  /* Is the domain privileged? */
#define SIF_INITDOMAIN    (1<<1)  /* Is this the initial control domain? */

typedef uint64_t cpumap_t;

typedef uint8_t xen_domain_handle_t[16];

/* Turn a plain number into a C unsigned long constant. */
#define __mk_unsigned_long(x) x ## UL
#define mk_unsigned_long(x) __mk_unsigned_long(x)

#else /* __ASSEMBLY__ */

/* In assembly code we cannot use C numeric constant suffixes. */
#define mk_unsigned_long(x) x

#endif /* !__ASSEMBLY__ */

#include "xen-compat.h"

#endif /* __XEN_PUBLIC_XEN_H__ */

#if CONFIG_GCOV_XEN
#define GCOV_XEN_GET_LEN		0
#define GCOV_XEN_COPY_BB_ALL	1
#define GCOV_XEN_BB_CUR_INIT	2
#define GCOV_XEN_ALL_RESET		3
#define GCOV_XEN_RESET			4
#define GCOV_XEN_TEST			5
#define GCOV_XEN_COPY_BB		6
#endif

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
