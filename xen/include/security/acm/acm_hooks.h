/*
 * ACM hooks
 */
 
#ifndef _ACM_HOOKS_HEADER
#define _ACM_HOOKS_HEADER

#include <xen/sched.h>
#include <public/event_channel.h>
#include <public/security/secure_storage_struct.h>
#include <public/xen.h>

#ifdef CONFIG_VMM_SECURITY_ACM
int acm_pause_domain(struct domain *dom);
int acm_unpause_domain(struct domain *dom);
int acm_create_domain(struct domain *dom);
int acm_copy_domain_image(struct domain *dom);
int acm_get_domaininfo(struct domain *dom);
int acm_destroy_domain(struct domain *dom);
int acm_remote_shutdown(struct domain *dom);
int acm_sched_ctl(void);
int acm_adjust_dom(void);
int acm_sched_get_id(void);
int acm_set_maxvcpus(struct domain *dom);
int acm_set_vcpuaffinity(struct domain *dom, struct vcpu *v);
int acm_set_vcpucontext(struct domain *dom, struct vcpu *v);
int acm_get_vcpucontext(struct domain *dom, struct vcpu *v);
int acm_get_vcpuinfo(struct domain *dom, struct vcpu *v);
int acm_set_domainmaxmem(struct domain *dom);
int acm_set_domainhandle(struct domain *dom);
int acm_set_debugging(struct domain *dom);
int acm_irq_permission(struct domain *dom, uint8_t pirq, int nr_pirq);
int acm_irq_permit_access(struct domain *dom, unsigned int start_pirq, unsigned int end_pirq);
int acm_irq_access_permitted(struct domain *dom, unsigned int start_pirq, unsigned int end_pirq);
int acm_send_guest_pirq(struct domain *dom, uint8_t pirq);
int acm_set_HID_irq(uint32_t irq);
int acm_irq_status_query(uint32_t irq);
int acm_set_irq_type(void);
//int acm_iomem_permission(struct domain *dom, unsigned long first_mfn, unsigned long nr_mfns, uint8_t allow_access);
int acm_iomem_permit_access(struct domain *dom, unsigned long first_mfn, unsigned long end_mfn);
int acm_iomem_access_permitted(struct domain *dom, unsigned int start_mfn, unsigned int end_mfn);
int acm_settime(void);
int acm_readconsole(uint32_t clear);
int acm_tbuf_control(void);
int acm_perfc_control(void);
int acm_evtchn_alloc_unbound(struct domain *dom, domid_t rdomid, evtchn_port_t port, uint32_t use);
int acm_evtchn_bind_interdomain(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use);
int acm_evtchn_send(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use);
int acm_evtchn_status(struct domain *dom, evtchn_port_t port, uint32_t use);
int acm_evtchn_bindvirq(uint32_t vcpu, uint32_t virq);
int acm_evtchn_virq_status(struct domain *dom, evtchn_port_t port, uint32_t virq);
int acm_evtchn_bindpirq(uint32_t pirq);
int acm_evtchn_bindvcpu(uint32_t vcpu, evtchn_port_t port);

//int acm_granttable_op(struct domain *subjdom, struct domain *objdom, grant_ref_t ref, uint32_t use);
int acm_granttable_share(struct domain *subjdom, struct domain *objdom, grant_ref_t ref, uint32_t use, uint32_t flag);
int acm_granttable_transfer(struct domain *subjdom, struct domain *objdom, grant_ref_t ref, uint32_t use);
int acm_granttable_setup(struct domain *dom);
int acm_granttable_dumptab(struct domain *dom);

// controls how much memory space a domain can take.
int acm_alloc_chunk(struct domain *dom, unsigned int order);
int acm_set_guest_pages(struct domain *dom, unsigned int size);
// controls privileges to reserving memory space for other domains
int acm_increase_reservation(struct domain *dom, unsigned int extent_order);
int acm_decrease_reservation(struct domain *dom);
int acm_populate_physmap(struct domain *dom, unsigned int extent_order);
int acm_current_reservation(struct domain *dom);
int acm_translate_gpfn_list(struct domain *dom);

int acm_modify_pte(pte_t nl1e); 
int acm_modify_pde(pde_t nl2e); 
int acm_mod_default_entry(unsigned long paddr); 
int acm_mmu_update(unsigned long mfn);
int acm_mmuext_op(void);

// also refer iomem_caps in struct domain *iomem_access_permitted <- get_page_from_l1e() <- mod_l1_entry()
// int acm_mod_l2entry(l1_pgentry_t nl1e);
int acm_update_va_mapping(struct domain *dom);

int acm_console_io(int cmd);

int acm_set_foregrounddom(void);
int acm_access_policy(int flag);
int acm_sra_read(partition_type_t part);
int acm_sra_read_enc_part(partition_type_t part);
int acm_sra_write(partition_type_t part, image_type_t type);

/* Hooks for resource usage management */
void acm_update_domain_state(struct domain *dom);
void acm_weight_dom_cpu(struct domain *d);
int acm_check_battery_saving(struct domain *d);

#else

static inline int acm_pause_domain(struct domain *dom){ return 1; }
static inline int acm_unpause_domain(struct domain *dom){ return 1; }
static inline int acm_create_domain(struct domain *dom){ return 1; }
static inline int acm_copy_domain_image(struct domain *dom){	return 1; }
static inline int acm_destroy_domain(struct domain *dom){ return 1; }
static inline int acm_remote_shutdown(struct domain *dom){ return 1; }
static inline int acm_get_domaininfo(struct domain *dom){ return 1; }
static inline int acm_sched_ctl(void){ return 1; }
static inline int acm_adjust_dom(void){ return 1; }
static inline int acm_sched_get_id(void){ 	return 1; }
static inline int acm_vcpu_common_op(struct domain *dom, struct vcpu* v, uint32_t req_type){ 	return 1; }
static inline int acm_set_vcpucontext(struct domain *dom, struct vcpu *v){ return 1; }
static inline int acm_get_vcpucontext(struct domain *dom, struct vcpu *v){ return 1; }
static inline int acm_get_vcpuinfo(struct domain *dom, struct vcpu *v){ return 1; }
static inline int acm_set_maxvcpus(struct domain *dom){ return 1; }
static inline int acm_set_vcpuaffinity(struct domain *dom, struct vcpu *v){ return 1; }
static inline int acm_set_domainmaxmem(struct domain *dom){ 	return 1; }
static inline int acm_set_domainhandle(struct domain *dom){ 	return 1; }
static inline int acm_set_debugging(struct domain *dom){ return 1; }
static inline int acm_irq_permission(struct domain *dom, uint8_t pirq, int nr_pirq){ return 1; }
static inline int acm_irq_permit_access(struct domain *dom, unsigned int start_pirq, unsigned int end_pirq){ return 1; }
static inline int acm_send_guest_pirq(struct domain *dom, uint8_t pirq){ return 1; }
static inline int acm_irq_status_query(uint32_t pirq){ return 1; }
static inline int acm_set_irq_type(void){ return 1; }
static inline int acm_iomem_permission(struct domain *dom, unsigned long first_mfn, unsigned long nr_mfns, uint8_t allow_access){ return 1; }
static inline int acm_iomem_permit_access(struct domain *dom, unsigned long first_mfn, unsigned long end_mfn){ 	return 0; }
static inline int acm_settime(void){ return 1; }
static inline int acm_readconsole(uint32_t clear){ 	return 1; }
static inline int acm_tbuf_control(void){ return 1; }
static inline int acm_sched_id(void){ return 1; }
static inline int acm_perfc_control(void){ return 1; }
static inline int acm_evtchn_alloc_unbound(struct domain *dom, domid_t rdomid, evtchn_port_t port, uint32_t use){ 	return 1; }
static inline int acm_evtchn_bind_interdomain(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use){ return 1; }
static inline int acm_evtchn_send(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use){ return 1; }
static inline int acm_evtchn_bindpirq(uint32_t pirq){ return 1; }
static inline int acm_evtchn_status(struct domain *dom, evtchn_port_t port, uint32_t use){ return 1; }
static inline int acm_evtchn_bindvirq(uint32_t virq, uint32_t vcpu){ 	return 1; }
static inline int acm_evtchn_virq_status(struct domain *dom, evtchn_port_t port, uint32_t virq){ return 1; }
static inline int acm_granttable_op(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use, uint32_t req_type){ return 1; }
static inline int acm_granttable_share(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use, uint32_t flags){ return 1; }
static inline int acm_granttable_transfer(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use){ return 1; }
static inline int acm_granttable_setup(struct domain *dom){ return 1; }
static inline int acm_granttable_dumptab(struct domain *dom){ return 1; }
static inline int acm_alloc_domheap(struct domain *dom, unsigned int pages, uint32_t req_type){ 	return 1; }
static inline int acm_increase_reservation(struct domain *dom, unsigned int extent_order){ return 1; }
static inline int acm_decrease_reservation(struct domain *dom){ return 1; }
static inline int acm_populate_physmap(struct domain *dom, unsigned int extent_order){ return 1; }
static inline int acm_current_reservation(struct domain *dom){ return 1; }
static inline int acm_translate_gpfn_list(struct domain *dom){ return 1; }
static inline int acm_alloc_chunk(struct domain *dom, unsigned int order){ return 1; }
static inline int acm_set_guest_pages(struct domain *dom, unsigned int size){ return 1; }
static inline int acm_modify_pte(pte_t nl1e){ return 1; }
static inline int acm_modify_pde(pde_t nl2e){ return 1; }
static inline int acm_mmu_update(unsigned long mfn){ return 1; }
static inline int acm_mod_default_entry(unsigned long paddr){ return 1; }
static inline int acm_mmuext_op(void){ 	return 1; }
static inline int acm_update_va_mapping(struct domain *dom){	return 1; }
static inline int acm_set_foregrounddom(void){ return 1; }
static inline int acm_access_policy(int flag){ return 1; }
static inline int acm_sra_read(partition_type_t part){ return 1; }
static inline int acm_sra_write(partition_type_t part, image_type_t type){ return 1; }
static inline int acm_sra_read_enc_part(partition_type_t part){ return 1;}
static inline int acm_console_io(int cmd){ return 1; }
static inline int acm_set_HID_irq(unsigned int irq){ return 1; }
static inline void acm_update_domain_state(struct domain *domid){ return; }
static inline void acm_weight_dom_cpu(struct domain *d){ return; }
static inline int acm_check_battery_saving(struct domain *d){ return 0; }

#endif

#endif
