
/*
 * chinesewall.h
 *
 * SRC OSV, 2007, Samsung Electronics Co., Ltd.
 */

#ifndef __ACDM_CW_H
#define __ACDM_CW_H

#include <xen/types.h>
#include <acm/acm.h>

/* 
 * cw_policy: start of binary CW policy 
 * The binary consists of as follow:
 * struct acm_policy || struct cw_policy || permission matrix
 */

#define CW_MAGIC_NUMBER     0x12131213
#define CW_STRING_SIZE      32
#define POLICY_SYSTEM_DEF   1
#define POLICY_USER_DEF     0

typedef struct cw_objtype_policy
{
    int cw_types_offset;
    int nr_cw_type;
    int nr_objects;
    int label_offset;
    int label_size;
    int matrix_offset;
    int matrix_size;
    int obj_to_type_size;
} cw_objtype_policy_t;

struct cw_policy {
    int cw_domains_offset;
    int nr_cw_domain;
    int vm_label_offset;
    int nr_vm_label;
    int vm_to_domain_size;
    int vm_label_size;
    cw_objtype_policy_t domain_res_policy;
};

struct cw_domain_name{
    char name[CW_STRING_SIZE];
    uint32_t index;
};

struct cw_type_name{
    char name[CW_STRING_SIZE];
    uint32_t index;
};

struct cw_VM_Label{
    uint32_t nr_domains;
    uint32_t domains_offset;
    domid_t vmid;
    char vm_name[CW_STRING_SIZE];
};

#endif	/* __ACDM_CW_H */
