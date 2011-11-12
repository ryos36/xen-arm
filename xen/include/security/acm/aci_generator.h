#include "acm.h"


void get_domain_info(struct domain *dom, aci_domain *aci_dom);
void get_environmental_context(aci_context *context);
void get_iomem_info(unsigned long mfn, struct aci_iomem *iomem);
unsigned long get_dev_unit_size(unsigned long paddr);
void get_pirq_info(uint8_t pirq, struct aci_pirq *st_pirq);
void get_vcpu_info(struct domain *obj_dom, struct aci_vcpu *vcpu);
void get_memory_info(struct domain *dom, struct aci_memory *mem);
int acm_belong_to_iomem(unsigned long paddr);
void get_evtchn_info(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use, struct aci_evtchn *evtchn);
void get_granttab_info(struct domain *dom, uint32_t mfn, struct aci_gnttab *gnttab);

#ifdef CONFIG_VMM_SECURITY_ACM
void create_evtchn_stat(domid_t domid);
void destroy_evtchn_stat(domid_t domid);
void update_evtchn_stat(domid_t domid, domid_t rdomid, uint32_t use, int operand);
void get_evtchn_stat(domid_t domid, domid_t rdomid, uint32_t use, int *evtchn_sum, int *rdom_evtchn_cnt, int *use_cnt);

#else
static inline void create_evtchn_stat(domid_t domid){ return; }
static inline void destroy_evtchn_stat(domid_t domid){ return; }
static inline void update_evtchn_stat(domid_t domid, domid_t rdomid, uint32_t use, int operand){ return; }
#endif
