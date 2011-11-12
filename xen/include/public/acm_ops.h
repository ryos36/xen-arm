#ifndef __ACM_OPS_H
#define __ACM_OPS_H

#define PC_MAGIC_NUM		0x98dea01
#define DDOM_MAGIC_NUM		0xd87204c

typedef enum policy_type{
	VMM,
	DRVDOM
}policy_type_t;

#define ACM_SETPOLICY	1
struct acm_setpolicy {
	/* INPUT variables */
	policy_type_t type_flag;
	void *binpolicy;
	uint32_t policy_size;
};

#define ACM_GETPOLICY	2

struct acm_getpolicy{
	/* INPUT variables */
	policy_type_t type_flag;
	int32_t index;	// for vmm acm policies, not for driver doms'
	void *binpolicy;
	uint32_t alloc_size;
	/* OUTPUT variables */
	int result;
	uint32_t policy_size;
};

#define ACM_GETACDMLIST 3
#define ACDM_NAME_MAXLEN	32
typedef struct acdminfo{
	char model_name[ACDM_NAME_MAXLEN];
	uint32_t magic_num;
	uint32_t policy_size;
}acdminfo_t;
DEFINE_XEN_GUEST_HANDLE(acdminfo_t);

struct acm_getacdmlist{
	/* INPUT variable */
	policy_type_t type_flag;
	XEN_GUEST_HANDLE(acdminfo_t) buffer;
	/* OUTPUT variables */
	int acdm_count;
};

#define ACM_GETVERSION  4
struct acm_getversion {
	/* INPUT variable */
	policy_type_t type_flag;
	uint32_t magic_num;
	/* OUTPUT variables */
	uint32_t version;
};

typedef struct acm_op {
	uint32_t cmd;
	union {
		struct acm_setpolicy	setpolicy;
		struct acm_getpolicy	getpolicy;
		struct acm_getacdmlist	getacdmlist;
		struct acm_getversion   getversion;
	} u;
} acm_op_t;
DEFINE_XEN_GUEST_HANDLE(acm_op_t);


long
do_acm_op(
    XEN_GUEST_HANDLE(acm_op_t) u_acm_op);

#endif
