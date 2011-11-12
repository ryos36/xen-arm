
#ifndef __XEN_PUBLIC_PHYSDEV_H__
#define __XEN_PUBLIC_PHYSDEV_H__

/* Commands to HYPERVISOR_physdev_op() */
#define PHYSDEVOP_IRQ_UNMASK_NOTIFY     4
#define PHYSDEVOP_IRQ_STATUS_QUERY      5
#define PHYSDEVOP_SET_IOPL              6
#define PHYSDEVOP_SET_IOBITMAP          7
#define PHYSDEVOP_APIC_READ             8
#define PHYSDEVOP_APIC_WRITE            9
#define PHYSDEVOP_ASSIGN_VECTOR         10
#define PHYSDEVOP_EOI			12
#define PHYSDEVOP_SET_IRQ_TYPE		13
#define PHYSDEVOP_AUX		14

#ifndef __ASSEMBLY__
typedef struct physdevop_irq_status_query {
    /* IN */
    uint32_t irq;
    /* OUT */
/* Need to call PHYSDEVOP_IRQ_UNMASK_NOTIFY when the IRQ has been serviced? */
#define PHYSDEVOP_IRQ_NEEDS_UNMASK_NOTIFY (1<<0)
    uint32_t flags;
} physdevop_irq_status_query_t;

typedef struct physdevop_set_iopl {
    /* IN */
    uint32_t iopl;
} physdevop_set_iopl_t;

typedef struct physdevop_set_iobitmap {
    /* IN */
    uint8_t *bitmap;
    uint32_t nr_ports;
} physdevop_set_iobitmap_t;

typedef struct physdevop_apic {
    /* IN */
    unsigned long apic_physbase;
    uint32_t reg;
    /* IN or OUT */
    uint32_t value;
} physdevop_apic_t;

typedef struct physdevop_irq {
    /* IN */
    uint32_t irq;
    /* OUT */
    uint32_t vector;
} physdevop_irq_t;

typedef struct physdevop_eoi {
	uint32_t irq;
} physdevop_eoi_t;

typedef struct physdevop_aux {
    /* IN */
    uint32_t aux;
    /* OUT */
    uint32_t vector;
} physdevop_aux_t;
#endif

#define PIRQT_NONE		(0x00000000)
#define PIRQT_RISING_EDGE	(0x00000001)
#define PIRQT_FALLING_EDGE	(0x00000002)
#define PIRQT_BOTH_EDGE		(0x00000003)
#define PIRQT_HIGH_LEVEL	(0x00000004)
#define PIRQT_LOW_LEVEL		(0x00000008)
#define PIRQT_PROBE		(0x00000010)
#define PIRQT_HID		(0x80000000)
#define PIRQT_MASK		(PIRQT_RISING_EDGE | PRIQT_FALLING_EDGE | PIRQT_HIGH_LEVEL | PIRQT_LOW_LEVEL | PIRQT_HID)

#ifndef __ASSEMBLY__
typedef struct physdevop_set_irq_type {
	uint32_t	irq;
	uint32_t	flags;
} physdevop_set_irq_type_t;

typedef struct physdev_op {
	uint32_t cmd;
	union {
		physdevop_irq_status_query_t	irq_status_query;
		physdevop_set_iopl_t		set_iopl;
		physdevop_set_iobitmap_t	set_iobitmap;
		physdevop_apic_t		apic_op;
		physdevop_irq_t			irq_op;
		physdevop_eoi_t			eoi_op;
		physdevop_set_irq_type_t	set_irq_type_op;
		physdevop_aux_t			aux_op;
	} u;
} physdev_op_t;
DEFINE_XEN_GUEST_HANDLE(physdev_op_t);
#endif
#endif /* __XEN_PUBLIC_PHYSDEV_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
