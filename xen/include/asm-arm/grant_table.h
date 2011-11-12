#ifndef __ARM_GRANT_TABLE_H__
#define __ARM_GRANT_TABLE_H__

#include <asm/mm.h>

#define ORDER_GRANT_FRAMES 2

/*
 * Caller must own caller's BIGLOCK, is responsible for flushing the TLB, and
 * must hold a reference to the page.
 */
int create_grant_host_mapping(unsigned long, unsigned long, unsigned int);
int destroy_grant_host_mapping(unsigned long, unsigned long, unsigned int);

int steal_page_for_grant_transfer(struct domain *, struct page_info *);

#define gnttab_create_shared_page(d, t, i)								\
	do {																\
		share_xen_page_with_guest(										\
			virt_to_page((char *)(t)->shared + ((i) * PAGE_SIZE)),		\
			(d), XENSHARE_writable);									\
	} while ( 0 )

#define gnttab_shared_mfn(d, t, i)                      \
	((va_to_ma((t)->shared) >> PAGE_SHIFT) + (i))

#define gnttab_shared_gmfn(d, t, i)			\
    (mfn_to_gmfn(d, gnttab_shared_mfn(d, t, i)))

#define gnttab_log_dirty(d, f)

#endif /* __ARM_GRANT_TABLE_H__ */
