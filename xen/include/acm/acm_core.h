/****************************************************************
 * acm_core.h 
 * 
 * Copyright (C) 2005 IBM Corporation
 *
 * Author:
 * Reiner Sailer <sailer@watson.ibm.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 * sHype header file describing core data types and constants
 *    for the access control module and relevant policies
 *
 */

#ifndef _ACM_CORE_H
#define _ACM_CORE_H

#include <xen/spinlock.h>
#include <public/acm.h>
#include <xen/acm_policy.h>
#include <public/acm_ops.h>

/* Xen-internal representation of the binary policy */
struct acm_binary_policy {
    u16 primary_policy_code;
    u16 secondary_policy_code;
};

struct chwall_binary_policy {
    u32 max_types;
    u32 max_ssidrefs;
    u32 max_conflictsets;
    domaintype_t *ssidrefs;     /* [max_ssidrefs][max_types]  */
    domaintype_t *conflict_aggregate_set;  /* [max_types]      */
    domaintype_t *running_types;    /* [max_types]      */
    domaintype_t *conflict_sets;   /* [max_conflictsets][max_types]*/
};

struct ste_binary_policy {
    u32 max_types;
    u32 max_ssidrefs;
    domaintype_t *ssidrefs;     /* [max_ssidrefs][max_types]  */
    atomic_t ec_eval_count, gt_eval_count;
    atomic_t ec_denied_count, gt_denied_count; 
    atomic_t ec_cachehit_count, gt_cachehit_count;
};

/* global acm policy */
extern u16 acm_active_security_policy;
extern struct acm_binary_policy acm_bin_pol;
extern struct chwall_binary_policy chwall_bin_pol;
extern struct ste_binary_policy ste_bin_pol;
/* use the lock when reading / changing binary policy ! */
extern rwlock_t acm_bin_pol_rwlock;

/* subject and object type definitions */
enum acm_datatype { DOMAIN };

/* defines number of access decisions to other domains can be cached
 * one entry per domain, TE does not distinguish evtchn or grant_table */
#define ACM_TE_CACHE_SIZE 8
enum acm_ste_flag { VALID, FREE };

/* cache line:
 * if cache_line.valid==VALID, then
 *    STE decision is cached as "permitted" 
 *                 on domain cache_line.id
 */
struct acm_ste_cache_line {
    enum acm_ste_flag valid;
    domid_t id;
};

/* general definition of a subject security id */
struct acm_ssid_domain {
    enum acm_datatype datatype; /* type of subject (e.g., partition) */
    ssidref_t ssidref;   /* combined security reference */
    void *primary_ssid;   /* primary policy ssid part (e.g. chinese wall) */
    void *secondary_ssid;    /* secondary policy ssid part (e.g. type enforcement) */
    struct domain *subject;     /* backpointer to subject structure */
    domid_t domainid;   /* replicate id */
};

/* chinese wall ssid type */
struct chwall_ssid {
    ssidref_t chwall_ssidref;
};

/* simple type enforcement ssid type */
struct ste_ssid {
    ssidref_t ste_ssidref;
    struct acm_ste_cache_line ste_cache[ACM_TE_CACHE_SIZE]; /* decision cache */
};

/* macros to access ssidref for primary / secondary policy 
 * primary ssidref   = lower 16 bit
 *  secondary ssidref = higher 16 bit
 */
#define ACM_PRIMARY(ssidref) \
 ((ssidref) & 0xffff)

#define ACM_SECONDARY(ssidref) \
 ((ssidref) >> 16)

#define GET_SSIDREF(POLICY, ssidref) \
 ((POLICY) == acm_bin_pol.primary_policy_code) ? \
 ACM_PRIMARY(ssidref) : ACM_SECONDARY(ssidref)

/* macros to access ssid pointer for primary / secondary policy */
#define GET_SSIDP(POLICY, ssid) \
 ((POLICY) == acm_bin_pol.primary_policy_code) ? \
 ((ssid)->primary_ssid) : ((ssid)->secondary_ssid)

/* protos */
int acm_init_domain_ssid(domid_t id, ssidref_t ssidref);
void acm_free_domain_ssid(struct acm_ssid_domain *ssid);
int acm_init_binary_policy(u32 policy_code);
int acm_set_policy(void *buf, u32 buf_size, int isuserbuffer);
int acm_get_policy(void *buf, u32 buf_size);
int acm_dump_statistics(void *buf, u16 buf_size);
int acm_get_ssid(ssidref_t ssidref, u8 *buf, u16 buf_size);
int acm_get_decision(ssidref_t ssidref1, ssidref_t ssidref2, enum acm_hook_type hook);

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
