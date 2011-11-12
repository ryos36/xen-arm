/******************************************************************************
 * iocap.h
 * 
 * Per-domain I/O capabilities.
 */

#ifndef __XEN_IOCAP_H__
#define __XEN_IOCAP_H__

#include <xen/rangeset.h>


#define ioports_permit_access(d, s, e)                  \
    rangeset_add_range((d)->arch.ioport_caps, s, e)
#define ioports_deny_access(d, s, e)                    \
    rangeset_remove_range((d)->arch.ioport_caps, s, e)
#define ioports_access_permitted(d, s, e)               \
    rangeset_contains_range((d)->arch.ioport_caps, s, e)

#ifdef CONFIG_VMM_SECURITY_ACM
	#define cache_flush_permitted(d)	1
#else
	#define cache_flush_permitted(d)                       \
   	 (!rangeset_is_empty((d)->iomem_caps))
#endif

#ifdef CONFIG_VMM_SECURITY_ACM
	#define iomem_permit_access(d, s, e)                    \
		acm_iomem_permit_access(d, s, e)
	#define iomem_access_permitted(d, s, e)                 \
		acm_iomem_access_permitted(d, s, e)
#else
	#define iomem_permit_access(d, s, e)                    \
   	 rangeset_add_range((d)->iomem_caps, s, e)
	#define iomem_access_permitted(d, s, e)                 \
   	 rangeset_contains_range((d)->iomem_caps, s, e)
#endif

#define iomem_deny_access(d, s, e)                      \
    rangeset_remove_range((d)->iomem_caps, s, e)

#ifdef CONFIG_VMM_SECURITY_ACM
	#define irq_permit_access(d, i)                         \
		acm_irq_permit_access(d, i, i)
	#define irqs_permit_access(d, s, e)                     \
		acm_irq_permit_access(d, s, e)
	#define irq_access_permitted(d, i)                      \
   	 acm_irq_access_permitted(d, i, i)
#else
	#define irq_permit_access(d, i)                         \
   	 rangeset_add_singleton((d)->irq_caps, i)
	#define irqs_permit_access(d, s, e)                     \
   	 rangeset_add_range((d)->irq_caps, s, e)
	#define irq_access_permitted(d, i)                      \
   	 rangeset_contains_singleton((d)->irq_caps, i)
#endif

#define irq_deny_access(d, i)                           \
    rangeset_remove_singleton((d)->irq_caps, i)
#define irqs_deny_access(d, s, e)                       \
    rangeset_remove_range((d)->irq_caps, s, e)

#ifdef CONFIG_VMM_SECURITY_ACM
	#define multipage_allocation_permitted(d)	1
#else
	#define multipage_allocation_permitted(d)               \
   	 (!rangeset_is_empty((d)->iomem_caps))
#endif

#define dmachn_permit_access(d, i)                         \
    rangeset_add_singleton((d)->dma_caps, i)
#define dmachn_deny_access(d, i)                           \
    rangeset_remove_singleton((d)->dma_caps, i)
#define dmachns_permit_access(d, s, e)                     \
    rangeset_add_range((d)->dma_caps, s, e)
#define dmachns_deny_access(d, s, e)                       \
    rangeset_remove_range((d)->dma_caps, s, e)
#define dmachn_access_permitted(d, i)                      \
    rangeset_contains_singleton((d)->dma_caps, i)

#endif /* __XEN_IOCAP_H__ */
