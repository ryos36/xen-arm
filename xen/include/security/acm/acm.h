#ifndef _ACM_HEADER
#define _ACM_HEADER

#include <xen/sched.h>
#include <public/event_channel.h>
#include <public/xen.h>

/*
 * Object type definition
 */
#define ACM_DOMAIN		0x0001U
#define ACM_VCPU			0x0002U
#define ACM_SCHEDULER	0x0003U
#define ACM_PERFCOUNTER	0x0004U
#define ACM_HANDLE		0x0005U
#define ACM_DEBUG			0x0006U
//#define ACM_CONSOLE	
#define ACM_CONSOLEIO	0x0007U
#define ACM_TRACEBUFFER	0x0008U
#define ACM_TIME			0x0009U
#define ACM_PIRQ			0x000aU
#define ACM_VIRQ			0x000bU
#define ACM_EVENTCHN		0x000cU
#define ACM_GRANTTAB		0x000dU
#define ACM_IOMEM			0x000eU
#define ACM_MEMORY		0x000fU
#define ACM_POLICY		0x0010U
#define ACM_SECUSTORAGE	0x0011U
#define ACM_DMACHN		0x0012U
#define ACM_ETC			0x0020U

/*
 * Permission type (operations on an object) definition
 */
//#define DOMCTX_SET	0x00000001UL
#define DOM_PAUSE			0x00000002UL
#define DOM_UNPAUSE		0x00000004UL
#define DOM_CREATE		0x00000008UL
#define DOM_DESTROY		0x00000010UL
#define DOM_SHUTDOWN		0x00000020UL
#define DOM_GET_INFO		0x00000040UL
#define DOM_COPY_IMAGE	0x00000080UL

#define VCPU_CONTROL			0x00000001UL
#define VCPU_SET_MAX			0x00000002UL
#define VCPU_SET_AFFINITY	0x00000004UL
#define VCPU_SET_CONTEXT	0x00000008UL
#define VCPU_GET_CONTEXT	0x00000010UL
#define VCPU_GET_INFO		0x00000020UL

#define SCHED_SET_CTX_SWITCH_TIME	0x00000001UL
#define SCHED_GET_INFO					0x00000002UL
#define SCHED_ADJUST						0x00000004UL
#define SCHED_GET_ID						0x00000008UL

//#define PERFC_RESET		0x00000001UL
//#define PERFC_QUERY		0x00000002UL
#define PERFC_CONTROL		0x00000001UL

#define HANDLE_SET			0x00000001UL

#define DEBUG_SET				0x00000001UL

#define CONSOLE_READ			0x00000001UL
#define CONSOLE_WRITE		0x00000002UL

#define TRACE_CONTROL		0x00000001UL

#define TIME_SET				0x00000001UL

#define PIRQ_CONTROL			0x00000001UL
#define PIRQ_DENY				0x00000002UL
#define PIRQ_QUERY_STAT		0x00000004UL
#define PIRQ_ACCESS			0x0000000cUL	// ACCESS includes QUERY_STAT
#define PIRQ_BIND_SHARE		0x00000010UL
#define PIRQ_SET_HID			0x00000020UL
#define PIRQ_SET_TYPE		0x00000040UL

#define IOMEM_CONTROL			0x00000001UL  //0x00010000UL
#define IOMEM_GET_STATUS		0x00020000UL
#define IOMEM_ACCESS				0x00060000UL	// ACCESS includes GET_STATUS
/* INDIRECT_ACCESS: access io registers via VMM ex) DMA */
#define IOMEM_INDIRECT_ACCESS 0x000a0000UL	// INDIRECT_ACCESS includes GET_STATUS
/*
#define IOMEM_SET_PERMISSION	0x00000001UL
*/

#define VIRQ_GET_STATUS		0x00000001UL
#define VIRQ_BIND				0x00000003UL	// BIND includes GET_STATUS

#define EVTCHN_CONTROL		0x00000001UL
#define EVTCHN_GET_STATUS	0x00000002UL
#define EVTCHN_OPEN			0x10000000UL
#define EVTCHN_SEND			0x20000000UL
#define EVTCHN_COMMUNICATE	0x30000000UL   // EVTCHN_COMMUNICATE includes EVTCHN_OPEN, EVTCHN_SEND

#define GNTTAB_SETUP			0x00000001UL
#define GNTTAB_DUMP_TABLE	0x00000002UL
// ops below are for domain communication
#define GNTTAB_TRANSFER		0x10000000UL
#define GNTTAB_READ			0x20000000UL
#define GNTTAB_WRITE			0x40000000UL

#define MEM_CONTROL				0x00000001UL
#define MEM_GET_STAT				0x00000002UL
#define MEM_TRANSLATE_ADDR		0x00000004UL
#define MEM_ALLOC_DOMHEAP		0x00000008UL

#define POLICY_ACCESS_VMM	0x00000001UL
#define POLICY_ACCESS_DDOM 0x00000002UL

/* 
 * Only read operation is valid for MBB.
 * Operations on SP2 is done through do_acm_op().
 */
#define SECUSTR_READ_ENCMBB		0x00000001UL
#define SECUSTR_READ_ENCSP1		0x00000002UL
#define SECUSTR_READ_ENCSP2		0x00000004UL
#define SECUSTR_READ_ENCSP3		0x00000008UL
#define SECUSTR_READ_ENCOSIMG		0x00000010UL
#define SECUSTR_READ_SP3			0x00000020UL
#define SECUSTR_READ_OSIMG			0x00000040UL
#define SECUSTR_WRITE_CERT			0x00000080UL
#define SECUSTR_WRITE_VMMSIG		0x00000100UL
#define SECUSTR_WRITE_OSIMG		0x00000200UL
#define SECUSTR_WRITE_OSSIG		0x00000400UL
#define SECUSTR_WRITE_SP3			0x00000800UL

#define ETC_SET_FOREGROUNDDOM			0x00000002UL

#define objtype_t	uint16_t

/*
 * Data structures of request for the decision making.
 */
typedef struct _aci_domain{
	unsigned long id;
}aci_domain;

enum object_struct_type{
	OBJST_TYPE_OTHER = 0,
	OBJST_TYPE_ONDOMAIN,
	OBJST_TYPE_SIMPLE
};

typedef struct _aci_object{
	objtype_t object_type;
	void *object_info;
	int struct_type;
}aci_object;

typedef struct _aci_context{

}aci_context;

struct aci_pirq{
	uint32_t device;
};

struct aci_iomem{
	uint32_t device;
};


struct aci_vcpu{
	aci_domain objdom_id;
	uint32_t vcpu_id;
	struct vcpu *vcpu;
};

struct aci_memory{
	unsigned int xenheap_pages;
	unsigned int req_pages;
	unsigned long sys_total;	
};

struct aci_virq{
	uint32_t vcpu_id;
	uint32_t virq;
	struct vcpu *vcpu;
};

struct aci_evtchn{
	aci_domain objdom_id;
	uint32_t use;
	/* Additional Info */
	evtchn_port_t port;
	int tot_evtchn;
	unsigned int per_rdom;
	unsigned int per_rdom_use;
};

struct aci_gnttab{
#define MEM_SHARED_INFO		0x1
#define MEM_IO_SPACE			0x2
#define MEM_DOM_IMAGE		0x10
#define MEM_DOM_HEAP			0x11
#define MEM_DOM_RAMDISK		0x12
#define MEM_VMM_SPACE		0x20

	aci_domain objdom_id;
	uint32_t use;
	/* Additional Info */
	uint32_t mfn;
	uint16_t mem_space;
};

struct aci_dmachn{
	uint16_t src_type;
	uint16_t dst_type;
};

#define ACI_SUBJID_SIZE		sizeof(unsigned long)
#define ACI_REQTYPE_SIZE	sizeof(uint32_t)
#define ACI_PIRQ_SIZE		sizeof(struct aci_pirq)
#define ACI_IOMEM_SIZE		sizeof(struct aci_iomem)
#define ACI_EVTCHN_SIZE		sizeof(struct aci_evtchn)
#define ACI_GNTTAB_SIZE		sizeof(struct aci_gnttab)
 


/*
	DECISION RESULT VALUES
*/
#define ACM_DECISION_PERMIT		0x1
#define ACM_DECISION_UNDEF			0x2
#define ACM_DECISION_NOTPERMIT	0x3

#define ACM_DECISION_RESCHEDULE 	0x10
#define ACM_DECISION_SHUTDOWN 	0x20

int acm_is_granted(aci_domain *subject, aci_object *object, uint32_t request_type, aci_context *context);
void acm_control_cpu_usage(struct domain *d);
int acm_control_battery_usage(unsigned int batterylife, struct domain *d);

#endif
