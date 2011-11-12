/*
 * acm.h: Xen access control module interface defintions
 *
 * Reiner Sailer <sailer@watson.ibm.com>
 * Copyright (c) 2005, International Business Machines Corporation.
 */

#ifndef _XEN_PUBLIC_ACM_H
#define _XEN_PUBLIC_ACM_H

#include "xen.h"
#include "sched_ctl.h"

/* if ACM_DEBUG defined, all hooks should
 * print a short trace message (comment it out
 * when not in testing mode )
 */
/* #define ACM_DEBUG */

#ifdef ACM_DEBUG
#  define printkd(fmt, args...) printk(fmt,## args)
#else
#  define printkd(fmt, args...)
#endif

/* default ssid reference value if not supplied */
#define ACM_DEFAULT_SSID  0x0
#define ACM_DEFAULT_LOCAL_SSID  0x0

/* Internal ACM ERROR types */
#define ACM_OK     0
#define ACM_UNDEF   -1
#define ACM_INIT_SSID_ERROR  -2
#define ACM_INIT_SOID_ERROR  -3
#define ACM_ERROR          -4

/* External ACCESS DECISIONS */
#define ACM_ACCESS_PERMITTED        0
#define ACM_ACCESS_DENIED           -111
#define ACM_NULL_POINTER_ERROR      -200

/* primary policy in lower 4 bits */
#define ACM_NULL_POLICY 0
#define ACM_CHINESE_WALL_POLICY 1
#define ACM_SIMPLE_TYPE_ENFORCEMENT_POLICY 2
#define ACM_POLICY_UNDEFINED 15

/* combinations have secondary policy component in higher 4bit */
#define ACM_CHINESE_WALL_AND_SIMPLE_TYPE_ENFORCEMENT_POLICY \
    ((ACM_SIMPLE_TYPE_ENFORCEMENT_POLICY << 4) | ACM_CHINESE_WALL_POLICY)

/* policy: */
#define ACM_POLICY_NAME(X) \
 ((X) == (ACM_NULL_POLICY)) ? "NULL policy" :                        \
    ((X) == (ACM_CHINESE_WALL_POLICY)) ? "CHINESE WALL policy" :        \
    ((X) == (ACM_SIMPLE_TYPE_ENFORCEMENT_POLICY)) ? "SIMPLE TYPE ENFORCEMENT policy" : \
    ((X) == (ACM_CHINESE_WALL_AND_SIMPLE_TYPE_ENFORCEMENT_POLICY)) ? "CHINESE WALL AND SIMPLE TYPE ENFORCEMENT policy" : \
     "UNDEFINED policy"

/* the following policy versions must be increased
 * whenever the interpretation of the related
 * policy's data structure changes
 */
#define ACM_POLICY_VERSION 1
#define ACM_CHWALL_VERSION 1
#define ACM_STE_VERSION  1

/* defines a ssid reference used by xen */
typedef uint32_t ssidref_t;

/* hooks that are known to domains */
enum acm_hook_type {NONE=0, SHARING};

/* -------security policy relevant type definitions-------- */

/* type identifier; compares to "equal" or "not equal" */
typedef uint16_t domaintype_t;

/* CHINESE WALL POLICY DATA STRUCTURES
 *
 * current accumulated conflict type set:
 * When a domain is started and has a type that is in
 * a conflict set, the conflicting types are incremented in
 * the aggregate set. When a domain is destroyed, the 
 * conflicting types to its type are decremented.
 * If a domain has multiple types, this procedure works over
 * all those types.
 *
 * conflict_aggregate_set[i] holds the number of
 *   running domains that have a conflict with type i.
 *
 * running_types[i] holds the number of running domains
 *        that include type i in their ssidref-referenced type set
 *
 * conflict_sets[i][j] is "0" if type j has no conflict
 *    with type i and is "1" otherwise.
 */
/* high-16 = version, low-16 = check magic */
#define ACM_MAGIC  0x0001debc

/* each offset in bytes from start of the struct they
 * are part of */

/* each buffer consists of all policy information for
 * the respective policy given in the policy code
 *
 * acm_policy_buffer, acm_chwall_policy_buffer,
 * and acm_ste_policy_buffer need to stay 32-bit aligned
 * because we create binary policies also with external
 * tools that assume packed representations (e.g. the java tool)
 */
struct acm_policy_buffer {
    uint32_t policy_version; /* ACM_POLICY_VERSION */
    uint32_t magic;
    uint32_t len;
    uint32_t primary_policy_code;
    uint32_t primary_buffer_offset;
    uint32_t secondary_policy_code;
    uint32_t secondary_buffer_offset;
};

struct acm_chwall_policy_buffer {
    uint32_t policy_version; /* ACM_CHWALL_VERSION */
    uint32_t policy_code;
    uint32_t chwall_max_types;
    uint32_t chwall_max_ssidrefs;
    uint32_t chwall_max_conflictsets;
    uint32_t chwall_ssid_offset;
    uint32_t chwall_conflict_sets_offset;
    uint32_t chwall_running_types_offset;
    uint32_t chwall_conflict_aggregate_offset;
};

struct acm_ste_policy_buffer {
    uint32_t policy_version; /* ACM_STE_VERSION */
    uint32_t policy_code;
    uint32_t ste_max_types;
    uint32_t ste_max_ssidrefs;
    uint32_t ste_ssid_offset;
};

struct acm_stats_buffer {
    uint32_t magic;
    uint32_t len;
    uint32_t primary_policy_code;
    uint32_t primary_stats_offset;
    uint32_t secondary_policy_code;
    uint32_t secondary_stats_offset;
};

struct acm_ste_stats_buffer {
    uint32_t ec_eval_count;
    uint32_t gt_eval_count;
    uint32_t ec_denied_count;
    uint32_t gt_denied_count;
    uint32_t ec_cachehit_count;
    uint32_t gt_cachehit_count;
};

struct acm_ssid_buffer {
    uint32_t len;
    ssidref_t ssidref;
    uint32_t primary_policy_code;
    uint32_t primary_max_types;
    uint32_t primary_types_offset;
    uint32_t secondary_policy_code;
    uint32_t secondary_max_types;
    uint32_t secondary_types_offset;
};

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
