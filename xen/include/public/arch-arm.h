/**
 * @file	arch-arm.h	(include/public/arch-arm.h)
 * @brief	archtecture specifiec configuration
 * 
 * @author	chanju, park	(beastworld@samsung.com)
 * @version	2006-06-08	basic setup, modified from ia-64, ppc
 * @copyright	Copyright (c) samsung electronics, co.
 */

#ifndef __XEN_PUBLIC_ARCH_ARM_32_H__
#define __XEN_PUBLIC_ARCH_ARM_32_H__

#define VPSR_MODE_USR26		0x00000000
#define VPSR_MODE_FIQ26		0x00000001
#define VPSR_MODE_IRQ26		0x00000002
#define VPSR_MODE_SVC26		0x00000003
#define VPSR_MODE_USR		0x00000010
#define VPSR_MODE_FIQ		0x00000011
#define VPSR_MODE_IRQ		0x00000012
#define VPSR_MODE_SVC		0x00000013
#define VPSR_MODE_ABT		0x00000017
#define VPSR_MODE_UND		0x0000001b
#define VPSR_MODE_SYS		0x0000001f
#define VPSR_MODE_MASK		0x0000001f
#define VPSR_T_BIT		0x00000020
#define VPSR_F_BIT		0x00000040
#define VPSR_I_BIT		0x00000100
#define VPSR_J_BIT		0x01000000
#define VPSR_Q_BIT		0x08000000
#define VPSR_V_BIT		0x10000000
#define VPSR_C_BIT		0x20000000
#define VPSR_Z_BIT		0x40000000
#define VPSR_N_BIT		0x80000000

/*
 * Groups of PSR bits
 */
#define VPSR_MASK_INTR		(VPSR_I_BIT | VPSR_F_BIT)
#define VPSR_MASK_MODE		0x000001f
#define VPSR_MASK_FLAGS		0xff000000      /* Flags                */
#define VPSR_MASK_STATUS	0x00ff0000      /* Status               */
#define VPSR_MASK_EXTENSION	0x0000ff00      /* Extension            */
#define VPSR_MASK_CONTROL	0x000000ff      /* Control              */


#define TLBF_ITLB		1
#define TLBF_DTLB		2
#define TLBF_ASID		4

#define CMD_FMRX		0
#define CMD_FMXR		1

#define FPEXC_XEN		0
#define FPINST_XEN		1
#define FPINST2_XEN		2
#define MVFR0_XEN		3

/* FPEXC bits */
#define FPEXC_EXCEPTION		(1<<31)
#define FPEXC_ENABLE		(1<<30)

#ifndef __ASSEMBLY__
#ifdef __XEN__
#define __DEFINE_XEN_GUEST_HANDLE(name, type) \
    typedef struct { type *p; } __guest_handle_ ## name
#else
#define __DEFINE_XEN_GUEST_HANDLE(name, type) \
    typedef type * __guest_handle_ ## name
#endif
    
#define DEFINE_XEN_GUEST_HANDLE(name) __DEFINE_XEN_GUEST_HANDLE(name, name)
#define XEN_GUEST_HANDLE(name)        __guest_handle_ ## name
    
/* Guest handles for primitive C types. */
__DEFINE_XEN_GUEST_HANDLE(uchar, unsigned char);
__DEFINE_XEN_GUEST_HANDLE(uint,  unsigned int);
__DEFINE_XEN_GUEST_HANDLE(ulong, unsigned long);
DEFINE_XEN_GUEST_HANDLE(char);
DEFINE_XEN_GUEST_HANDLE(int);
DEFINE_XEN_GUEST_HANDLE(long);
DEFINE_XEN_GUEST_HANDLE(void);

/*
 * Virtual addresses beyond this are not modifiable by guest OSes. The 
 * machine->physical mapping table starts at this address, read-only.
 */
#define __HYPERVISOR_VIRT_START 0xFC000000

#ifndef HYPERVISOR_VIRT_START
#define HYPERVISOR_VIRT_START mk_unsigned_long(__HYPERVISOR_VIRT_START)
#endif

#ifndef machine_to_phys_mapping
#define machine_to_phys_mapping ((unsigned long *)HYPERVISOR_VIRT_START)
#endif

#if 0
typedef cpu_user_regs_t	cpu_bounce_frame_t;
#endif

typedef struct trap_info {
	unsigned long flags;
	unsigned long address;
}trap_info_t;

DEFINE_XEN_GUEST_HANDLE(trap_info_t);

typedef struct vcpu_guest_context {
	unsigned long	r0;
	unsigned long	r1;
	unsigned long	r2;
	unsigned long	r3;
	unsigned long	r4;
	unsigned long	r5;
	unsigned long	r6;
	unsigned long	r7;
	unsigned long	r8;
	unsigned long	r9;
	unsigned long	r10;
	unsigned long	r11;
	unsigned long	r12;
	unsigned long	r13;
	unsigned long	r14;
	unsigned long	r15;
	unsigned long   vbar;
	unsigned long   dacr;
	unsigned long   contextidr;
	unsigned long   fcseidr;
	unsigned long   ttbr0;
	unsigned long   ttbr1;
	unsigned long   ttbcr;
	unsigned long	cpar;
	unsigned long	event_callback;
	unsigned long	failsafe_callback;
} vcpu_guest_context_t;
DEFINE_XEN_GUEST_HANDLE(vcpu_guest_context_t);

typedef struct trap_frame {
	unsigned long sp;
	unsigned long lr;
	unsigned long spsr;
	unsigned long trap;
}trap_frame_t;
DEFINE_XEN_GUEST_HANDLE(trap_frame_t);

typedef struct arch_vcpu_info {
	unsigned long	sp;
	unsigned long	lr;
	unsigned long	cpsr;
	unsigned long	spsr;
	unsigned long	cr;
	unsigned long	cpar;
	unsigned long	dacr;
	unsigned long	pidr;
	unsigned long	far;
	unsigned long	fsr;
	unsigned long	reserved10;
	unsigned long	reserved11;
	unsigned long	reserved12;
	unsigned long	reserved13;
	unsigned long	reserved14;
} arch_vcpu_info_t;

#define MAX_VIRT_CPUS 4

typedef struct arch_shared_info {
	unsigned long	platform;
	unsigned long	max_pfn;
	unsigned long	pfn_to_mfn_frame_list_list;
#if 0
	unsigned long	nmi_reason;
#endif
} arch_shared_info_t;

#endif
#endif
