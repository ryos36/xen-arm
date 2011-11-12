#include "acm.h"
#include <asm-arm/page.h>

#define ACMCACHE_DECISION_BITS	2
#define ACMCACHE_DECISION_RMASK	0x03	
#define ACMCACHE_NOCACHE 			0x00
/*
	DECISION RESULT VALUES with CACHING INDICATION
*/
#define ACMCACHE_CACHING	0x0
#define ACMCACHE_NOCACHING	0x1
#define ACMCACHE_CACHING_SHIFT	2
/* 
   0xx : do caching 
   1xx: no caching 
*/
#define ACMCACHE_NOCACHE_MASK			(ACMCACHE_NOCACHING << ACMCACHE_CACHING_SHIFT) 
#define ACM_CACHING_DECISION_PERMIT		ACM_DECISION_PERMIT
#define ACM_CACHING_DECISION_UNDEF		ACM_DECISION_UNDEF
#define ACM_CACHING_DECISION_NOTPERMIT		ACM_DECISION_NOTPERMIT
#define ACM_NOCACHING_DECISION_PERMIT		(ACMCACHE_NOCACHE_MASK | ACM_DECISION_PERMIT)
#define ACM_NOCACHING_DECISION_UNDEF		(ACMCACHE_NOCACHE_MASK | ACM_DECISION_UNDEF)
#define ACM_NOCACHING_DECISION_NOTPERMIT	(ACMCACHE_NOCACHE_MASK | ACM_DECISION_NOTPERMIT)



 
#define ACMCACHE_KEY_SIZE	20
#define ACMCACHE_MAX_MODEL	(16/ACMCACHE_DECISION_BITS)	//= (the size of each entry :16bits)/(the size of each decision bits: 2bits)

/*
 * decision_cache: decision cache entry
 * It is associative LRU cache.
 * key: combination of subject id(uint16),object id(uint64), request type(uint32) = 14bytes
 * decision values: 2 bits for each access control model's decision result. 
 *                  Thus, can hold up to 8 ac models' decisions
 *		    00 : not in the cache
 *		    01 : permitted
		    10 : undefined 			
 *		    11 : not permitted
*/
typedef struct _decision_cache{
	uint16_t index;
	uint16_t decisions;
	char key[ACMCACHE_KEY_SIZE]; // might be changed with hashing value in the future
	uint32_t padding;
}decision_cache;

/*
 * vacancy : the number of occupied entries in a bucket
 */
struct decision_lru_cache{
	uint8_t next_replace;
	int vacancy;
};

typedef struct _acm_caching_info{
	uint32_t bucket_idx;
	uint32_t bucket_offset;
	decision_cache *cache_entry;
}acm_caching_info;

#define ACI_PIRQ_KEYSTR_SIZE	sizeof(uint32_t)
#define ACI_IOMEM_KEYSTR_SIZE	sizeof(uint32_t)
#define ACI_VCPU_KEYSTR_SIZE	sizeof(aci_domain)+sizeof(uint32_t)
#define ACI_VIRQ_KEYSTR_SIZE	sizeof(uint32_t) + sizeof(uint32_t)
#define ACI_EVTCHN_KEYSTR_SIZE	sizeof(aci_domain) + sizeof(uint32_t)
#define ACI_GNTTAB_KEYSTR_SIZE 	sizeof(aci_domain) + sizeof(uint32_t)
#define ACI_MEMORY_KEYSTR_SIZE	sizeof(unsigned int) + sizeof(unsigned int)

/*
 * ACMCACHE_TOTAL_BUCKET: number of buckets in the 2^ACMCACHE_ORDER page(s)
 */
#define ACMCACHE_ORDER		0
#define ACMCACHE_BUCKET_ORDER	1
#define ACMCACHE_BUCKET_ELEMENTS  (1UL << ACMCACHE_BUCKET_ORDER)
#define ACMCACHE_BUCKET_SIZE 	(sizeof(decision_cache) << ACMCACHE_BUCKET_ORDER) // = 2^5 << 2 = 2^7
#define ACMCACHE_MEM (1UL << (ACMCACHE_ORDER + PAGE_SHIFT))
#define ACMCACHE_TOTAL_BUCKET	(ACMCACHE_MEM/ACMCACHE_BUCKET_SIZE)
#define ACMCACHE_BUCKET_MASK	~((1 << ACMCACHE_BUCKET_ORDER) - 1)


int init_decision_cache(void);
void get_key_string(aci_domain *subject, aci_object *object, uint32_t request_type, uint8_t *key_str);
uint32_t constitute_key(uint8_t *key_str);
uint16_t lookup_cache(uint8_t *key_str, uint32_t key, acm_caching_info *caching);


int update_cache_by_name(aci_domain *subject, aci_object *object, uint32_t request_type, char *acmodel_name, uint16_t val);
int update_cache(uint8_t *key_str, uint32_t key, int  acmodel_idx, uint16_t val, acm_caching_info *caching);
void flush_cache(int index);
void print_map_cache(void);
