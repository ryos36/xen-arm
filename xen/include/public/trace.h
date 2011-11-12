/******************************************************************************
 * include/public/trace.h
 * 
 * Mark Williamson, (C) 2004 Intel Research Cambridge
 * Copyright (C) 2005 Bin Ren
 */

#ifndef __XEN_PUBLIC_TRACE_H__
#define __XEN_PUBLIC_TRACE_H__

/* Trace classes */
#define TRC_CLS_SHIFT 16
#define TRC_GEN     0x0001f000    /* General trace            */
#define TRC_SCHED   0x0002f000    /* Xen Scheduler trace      */
#define TRC_DOM0OP  0x0004f000    /* Xen DOM0 operation trace */
#define TRC_VMX     0x0008f000    /* Xen VMX trace            */
#define TRC_MEM     0x000af000    /* Xen memory trace         */
#define TRC_PV       0x0020f000    /* Xen PV traces            */
#define TRC_SHADOW   0x0040f000    /* Xen shadow tracing       */
#define TRC_PM       0x0080f000    /* Xen power management trace */
#define TRC_ALL     0xfffff000
#define TRC_HD_TO_EVENT(x) ((x)&0x0fffffff)
#define TRC_HD_CYCLE_FLAG (1UL<<31)
#define TRC_HD_INCLUDES_CYCLE_COUNT(x) ( !!( (x) & TRC_HD_CYCLE_FLAG ) )
#define TRC_HD_EXTRA(x)    (((x)>>TRACE_EXTRA_SHIFT)&TRACE_EXTRA_MAX)


/* Trace subclasses */
#define TRC_SUBCLS_SHIFT 12

/* trace subclasses for SVM */
#define TRC_HVM_ENTRYEXIT 0x00081000   /* VMENTRY and #VMEXIT       */
#define TRC_HVM_HANDLER   0x00082000   /* various HVM handlers      */

#define TRC_SCHED_MIN       0x00021000   /* Just runstate changes */
#define TRC_SCHED_VERBOSE   0x00028000   /* More inclusive scheduling */

/* trace subclasses for VMX */
#define TRC_VMXEXIT  0x00081000   /* VMX exit trace            */
#define TRC_VMXTIMER 0x00082000   /* VMX timer trace           */
#define TRC_VMXINT   0x00084000   /* VMX interrupt trace       */
#define TRC_VMXIO    0x00088000   /* VMX io emulation trace  */

/* Trace events per class */
#define TRC_SCHED_RUNSTATE_CHANGE   (TRC_SCHED_MIN + 1)
#define TRC_SCHED_CONTINUE_RUNNING  (TRC_SCHED_MIN + 2)
#define TRC_SCHED_DOM_ADD       (TRC_SCHED +  1)
#define TRC_SCHED_DOM_REM       (TRC_SCHED +  2)
#define TRC_SCHED_SLEEP         (TRC_SCHED +  3)
#define TRC_SCHED_WAKE          (TRC_SCHED +  4)
#define TRC_SCHED_YIELD         (TRC_SCHED +  5)
#define TRC_SCHED_BLOCK         (TRC_SCHED +  6)
#define TRC_SCHED_SHUTDOWN      (TRC_SCHED +  7)
#define TRC_SCHED_CTL           (TRC_SCHED +  8)
#define TRC_SCHED_ADJDOM        (TRC_SCHED +  9)
#define TRC_SCHED_SWITCH        (TRC_SCHED + 10)
#define TRC_SCHED_S_TIMER_FN    (TRC_SCHED + 11)
#define TRC_SCHED_T_TIMER_FN    (TRC_SCHED + 12)
#define TRC_SCHED_DOM_TIMER_FN  (TRC_SCHED + 13)
#define TRC_SCHED_SWITCH_INFPREV (TRC_SCHED + 14)
#define TRC_SCHED_SWITCH_INFNEXT (TRC_SCHED + 15)

#define TRC_MEM_PAGE_GRANT_MAP      (TRC_MEM + 1)
#define TRC_MEM_PAGE_GRANT_UNMAP    (TRC_MEM + 2)
#define TRC_MEM_PAGE_GRANT_TRANSFER (TRC_MEM + 3)

#define TRC_PV_HYPERCALL             (TRC_PV +  1)
#define TRC_PV_TRAP                  (TRC_PV +  3)
#define TRC_PV_PAGE_FAULT            (TRC_PV +  4)
#define TRC_PV_FORCED_INVALID_OP     (TRC_PV +  5)
#define TRC_PV_EMULATE_PRIVOP        (TRC_PV +  6)
#define TRC_PV_EMULATE_4GB           (TRC_PV +  7)
#define TRC_PV_MATH_STATE_RESTORE    (TRC_PV +  8)
#define TRC_PV_PAGING_FIXUP          (TRC_PV +  9)
#define TRC_PV_GDT_LDT_MAPPING_FAULT (TRC_PV + 10)
#define TRC_PV_PTWR_EMULATION        (TRC_PV + 11)
#define TRC_PV_PTWR_EMULATION_PAE    (TRC_PV + 12)

/* Indicates that addresses in trace record are 64 bits */
#define TRC_64_FLAG               (0x100)

#define TRC_SHADOW_NOT_SHADOW                 (TRC_SHADOW +  1)
#define TRC_SHADOW_FAST_PROPAGATE             (TRC_SHADOW +  2)
#define TRC_SHADOW_FAST_MMIO                  (TRC_SHADOW +  3)
#define TRC_SHADOW_FALSE_FAST_PATH            (TRC_SHADOW +  4)
#define TRC_SHADOW_MMIO                       (TRC_SHADOW +  5)
#define TRC_SHADOW_FIXUP                      (TRC_SHADOW +  6)
#define TRC_SHADOW_DOMF_DYING                 (TRC_SHADOW +  7)
#define TRC_SHADOW_EMULATE                    (TRC_SHADOW +  8)
#define TRC_SHADOW_EMULATE_UNSHADOW_USER      (TRC_SHADOW +  9)
#define TRC_SHADOW_EMULATE_UNSHADOW_EVTINJ    (TRC_SHADOW + 10)
#define TRC_SHADOW_EMULATE_UNSHADOW_UNHANDLED (TRC_SHADOW + 11)
#define TRC_SHADOW_WRMAP_BF                   (TRC_SHADOW + 12)
#define TRC_SHADOW_PREALLOC_UNPIN             (TRC_SHADOW + 13)
#define TRC_SHADOW_RESYNC_FULL                (TRC_SHADOW + 14)
#define TRC_SHADOW_RESYNC_ONLY                (TRC_SHADOW + 15)

/* trace events per subclass */
#define TRC_VMX_VMEXIT          (TRC_VMXEXIT + 1)
#define TRC_VMX_VMENTRY         (TRC_VMXEXIT + 2)
#define TRC_HVM_VMEXIT64        (TRC_HVM_ENTRYEXIT + TRC_64_FLAG + 0x02)
#define TRC_HVM_PF_XEN          (TRC_HVM_HANDLER + 0x01)
#define TRC_HVM_PF_XEN64        (TRC_HVM_HANDLER + TRC_64_FLAG + 0x01)
#define TRC_HVM_PF_INJECT       (TRC_HVM_HANDLER + 0x02)
#define TRC_HVM_PF_INJECT64     (TRC_HVM_HANDLER + TRC_64_FLAG + 0x02)
#define TRC_HVM_INJ_EXC         (TRC_HVM_HANDLER + 0x03)
#define TRC_HVM_INJ_VIRQ        (TRC_HVM_HANDLER + 0x04)
#define TRC_HVM_REINJ_VIRQ      (TRC_HVM_HANDLER + 0x05)
#define TRC_HVM_IO_READ         (TRC_HVM_HANDLER + 0x06)
#define TRC_HVM_IO_WRITE        (TRC_HVM_HANDLER + 0x07)
#define TRC_HVM_CR_READ         (TRC_HVM_HANDLER + 0x08)
#define TRC_HVM_CR_READ64       (TRC_HVM_HANDLER + TRC_64_FLAG + 0x08)
#define TRC_HVM_CR_WRITE        (TRC_HVM_HANDLER + 0x09)
#define TRC_HVM_CR_WRITE64      (TRC_HVM_HANDLER + TRC_64_FLAG + 0x09)
#define TRC_HVM_DR_READ         (TRC_HVM_HANDLER + 0x0A)
#define TRC_HVM_DR_WRITE        (TRC_HVM_HANDLER + 0x0B)
#define TRC_HVM_MSR_READ        (TRC_HVM_HANDLER + 0x0C)
#define TRC_HVM_MSR_WRITE       (TRC_HVM_HANDLER + 0x0D)
#define TRC_HVM_CPUID           (TRC_HVM_HANDLER + 0x0E)
#define TRC_HVM_INTR            (TRC_HVM_HANDLER + 0x0F)
#define TRC_HVM_NMI             (TRC_HVM_HANDLER + 0x10)
#define TRC_HVM_SMI             (TRC_HVM_HANDLER + 0x11)
#define TRC_HVM_VMMCALL         (TRC_HVM_HANDLER + 0x12)
#define TRC_HVM_HLT             (TRC_HVM_HANDLER + 0x13)
#define TRC_HVM_INVLPG          (TRC_HVM_HANDLER + 0x14)
#define TRC_HVM_INVLPG64        (TRC_HVM_HANDLER + TRC_64_FLAG + 0x14)
#define TRC_HVM_MCE             (TRC_HVM_HANDLER + 0x15)
#define TRC_HVM_IOPORT_READ     (TRC_HVM_HANDLER + 0x16)
#define TRC_HVM_IOMEM_READ      (TRC_HVM_HANDLER + 0x17)
#define TRC_HVM_CLTS            (TRC_HVM_HANDLER + 0x18)
#define TRC_HVM_LMSW            (TRC_HVM_HANDLER + 0x19)
#define TRC_HVM_LMSW64          (TRC_HVM_HANDLER + TRC_64_FLAG + 0x19)
#define TRC_HVM_INTR_WINDOW     (TRC_HVM_HANDLER + 0x20)
#define TRC_HVM_IOPORT_WRITE    (TRC_HVM_HANDLER + 0x216)
#define TRC_HVM_IOMEM_WRITE     (TRC_HVM_HANDLER + 0x217)

/* trace subclasses for power management */
#define TRC_PM_FREQ     0x00801000      /* xen cpu freq events */
#define TRC_PM_IDLE     0x00802000      /* xen cpu idle events */

/* trace events for per class */
#define TRC_PM_FREQ_CHANGE      (TRC_PM_FREQ + 0x01)
#define TRC_PM_IDLE_ENTRY       (TRC_PM_IDLE + 0x01)
#define TRC_PM_IDLE_EXIT        (TRC_PM_IDLE + 0x02)

/* This structure represents a single trace buffer record. */
struct t_rec {
    uint64_t cycles;          /* cycle counter timestamp */
    uint32_t event;           /* event ID                */
    unsigned long data[5];    /* event data items        */
};

/*
 * This structure contains the metadata for a single trace buffer.  The head
 * field, indexes into an array of struct t_rec's.
 */
struct t_buf {
    uint32_t cons;      /* Next item to be consumed by control tools. */
    uint32_t prod;      /* Next item to be produced by Xen.           */
    /* 'nr_recs' records follow immediately after the meta-data header.    */
};

#endif /* __XEN_PUBLIC_TRACE_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
