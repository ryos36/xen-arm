
/*
 * biba_blp.h
 *
 * SRC OSV, 2007, Samsung Electronics Co., Ltd.
 */

#ifndef __ACDM_BIBA_BLP_H
#define __ACDM_BIBA_BLP_H


#include <xen/types.h>
#include <security/acm/acm.h>

/* 
 * The binary consists of as follow:
 * struct acm_policy_header || 
 * struct biba_blp_policy || 
 * struct biba_blp_subject [] ||
 * struct biba_blp_object []
 */

#define BIBA_MAGIC_NUMBER   0x43984398
#define BLP_MAGIC_NUMBER    0x43994399
#define STRING_SIZE         32
#define POLICY_SYSTEM_DEF   1
#define POLICY_USER_DEF     0

#define BBLP_INF_FLOW_UNKNOWN      0
#define BBLP_INF_FLOW_SUBJ_TO_OBJ  1
#define BBLP_INF_FLOW_OBJ_TO_SUBJ  2

//typedef uint16_t domid_t;

typedef struct
{
    domid_t  domid;
    uint16_t dummy;
    uint32_t sec_label_min;
    uint32_t sec_label_max;    
} biba_blp_subject;

typedef struct
{
    uint32_t driver_signature;
    uint32_t sec_label;
} biba_blp_object;

typedef struct {
    int subjects_offset;
    int nr_subjects;
    int objects_offset;
    int nr_objects;
} biba_blp_policy;

#endif	/* __ACDM_BIBA_BLP_H */
