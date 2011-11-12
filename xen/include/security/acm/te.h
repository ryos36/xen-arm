#ifndef __ACDM_TE_H
#define __ACDM_TE_H

#include <xen/types.h>
#include <security/acm/acm.h>

/*
 * te_policy: start of binary TE policy 
 * The binary consists of as follow:
 * struct acm_policy || struct te_policy(contains acm_policy) || struct te_domain 
 * || te domain labels || [struct EVTCHN_Label]* || [mapping an evtch obj to a type]*  
 * || ....   || nr_policy_set number of matrix
 *
 * policy matrix:  two-dimensional matrix (rows: TE domains(suject types), columns: TE types(object types)) 
 * Don't be confused TE domains with domains in virtual machine terminology 
 * and TE types with object types defined in acm.h.
 * Policies are devided into policy sets according to object types defined in acm.h. 
 * However, for an object type with only one operation, those object types are grouped
 * together and put into one policy set(matrix).
 * Object types having individual policy set: ACM_EVTCHN, ACM_GNTTAB, ACM_PIRQ
 * Other object types are put in one policy set.	
 */

#define TE_MAGIC_NUMBER		0xe094ad0
#define TE_STRING_SIZE 		32
#define POLICY_SYSTEM_DEF	1
#define POLICY_USER_DEF		0

typedef struct objtype_policy{
	int te_types_offset;
	int nr_te_type;
	int nr_objects;
	int label_offset;
	int label_size;
	int matrix_offset;
	int matrix_size;
	int obj_to_type_size;
}objtype_policy_t;

/*
 * device_policy : pirq, iomem
 * domain_res_policy: applies to the objects which is represented 
 * 						with an domain id and an object type.
 * simple_res_policy: applies to the rest of object types
 */
struct te_policy{
	int te_domains_offset;
	int nr_te_domain;
	int vm_label_offset;
	int nr_vm_label;
	int vm_to_domain_size;
	int vm_label_size;
	objtype_policy_t evtchn_policy;
	objtype_policy_t gnttab_policy;
	objtype_policy_t virq_policy;
	objtype_policy_t device_policy;
	objtype_policy_t memory_policy;
	objtype_policy_t domain_res_policy;
	objtype_policy_t simple_res_policy;
};

struct te_domain_name{
	char name[TE_STRING_SIZE];
	uint32_t index;
};

struct te_type_name{
	char name[TE_STRING_SIZE];
	uint32_t index;
};

/*
 * a mapping between a vm and domain types defined by TE
 */
struct VM_Label{
	uint32_t nr_domains;
	uint32_t domains_offset;
	unsigned long vmid;
	char vm_name[TE_STRING_SIZE];
};

/*
 * nr_types: indicates how many object type defined by TE a resource(=object) has.
 * types_offset: starting offset of object type statement
 */
struct COMMON_Label{
	uint32_t nr_types;
	uint32_t types_offset;
};

#define VM_LABEL	0
#define DOMAIN_LABEL	1

struct EVTCHN_Label{
	uint32_t nr_types;
	uint32_t types_offset;
	//uint8_t owner_type;	// Either index of VMLabel or index of Domains
	uint16_t owner_index;
	uint32_t use;
};

struct GNTTAB_Label{
	uint32_t nr_types;
	uint32_t types_offset;
	//uint8_t owner_type;
	uint16_t owner_index;
	uint32_t use;
	uint16_t mem_space;
};

#define PIRQ_LABEL	0x1000
#define IOMEM_LABEL	0x0
struct DEVICE_Label{
	uint32_t nr_types;
	uint32_t types_offset;
	/* left most 16bits: label indication, right most 16bits: device id */
	uint16_t device;
};

struct SIMPLE_RESOURCE_Label{
	uint32_t nr_types;
	uint32_t types_offset;
	uint32_t value;
};

#endif
