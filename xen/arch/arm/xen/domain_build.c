/*
 * domain_build.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          ChanJu Park <bestworld@samsung.com>
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/compile.h>
#include <xen/sched.h>
#include <xen/elf.h>
#include <xen/domain.h>
#include <xen/mm.h>
#include <xen/errno.h>
#include <xen/iocap.h>
#include <xen/xmalloc.h>
#include <xen/preempt.h>
#include <public/xen.h>
#include <public/version.h>
#include <public/security/secure_storage_struct.h>
#include <security/ssm-xen/sra_func.h>
#include <security/crypto/crypto.h>
#include <security/acm/acm_hooks.h>

extern struct domain * idle_domain;

#ifdef CONFIG_VMM_SECURITY_IMAGE_VERIFICATION
static void image_verification_for_security(unsigned long image_addr, domid_t domain_id);
#endif

static const char *feature_names[XENFEAT_NR_SUBMAPS * 32] = {
	[XENFEAT_writable_page_tables]       = "writable_page_tables",
	[XENFEAT_auto_translated_physmap]    = "auto_translated_physmap",
	[XENFEAT_supervisor_mode_kernel]     = "supervisor_mode_kernel",
};

static void parse_features(
	const char *feats,
	uint32_t supported[XENFEAT_NR_SUBMAPS],
	uint32_t required[XENFEAT_NR_SUBMAPS])
{
	const char *end, *p;
	int i, req;

	if ((end = strchr(feats, ',')) == NULL)
		end = feats + strlen(feats);

	while (feats < end) {
		p = strchr(feats, '|');
		if ((p == NULL) || (p > end))
			p = end;

		req = (*feats == '!');
		if (req)
			feats++;

		for (i = 0; i < XENFEAT_NR_SUBMAPS * 32; i++) {
			if (feature_names[i] == NULL)
				continue;

			if (strncmp(feature_names[i], feats, p - feats) == 0) {
				set_bit(i, (unsigned long *)supported);
			if (req)
				set_bit(i, (unsigned long *)required);
			break;
		}
	}

	if (i == XENFEAT_NR_SUBMAPS * 32) {
		printk("Unknown kernel feature \"%.*s\".\n",
		(int)(p-feats), feats);
		if (req)
			PANIC("Domain 0 requires an unknown hypervisor feature.\n");
	}

	feats = p;
	if ( *feats == '|' )
		feats++;
	}
}

unsigned long prepare_guest_tables(unsigned long map, unsigned long va, unsigned long pa, int len)
{
	pde_t *pgd;
	pte_t *pgt;
	int idx;

	pgd = (pde_t *)map;
	memcpy(pgd, (void *)idle_domain->vcpu[0]->arch.ctx.ttbr0, PGD_SIZE);
	map += PGD_SIZE;

	idx = PGD_IDX(va);

	do {
		pgd[idx++] = MK_PDE(pa, PDE_GUEST_SECTION);
		pa += 0x100000;
		len -= 0x100000;
	} while(len > 0);

	pgt = (pte_t *)map;

	clear_page(pgt);

	map += PAGE_SIZE;

	copy_page(map, GET_HVT_PAGE());

	pgt[PGT_IDX(VECTORS_BASE)] = MK_PTE(map, PTE_VECTOR_PAGE);
	pgd[PGD_IDX(VECTORS_BASE)] = MK_PDE((unsigned long)&pgt[0], PDE_VECTOR_TABLE);

	/* 1 page table and 1 page for exception vector */
	return ((PGD_SIZE >> PAGE_SHIFT) + 2);
}

extern void return_to_guest();

void new_thread(struct vcpu *v,
	unsigned long start_pc,
	unsigned long start_stack,
	unsigned long start_info)
{
	void *domain_stack;
	struct cpu_info *ci;
	struct cpu_ctx *cpu_ctx;

	domain_stack = alloc_xenheap_pages(STACK_ORDER);
	if(domain_stack == NULL) {
		return;
	}

	ci = (struct cpu_info *)domain_stack;
	ci->vcpu = v;
	ci->vspsr = PSR_MODE_SVC;
	ci->vsp = 0;
	ci->vdacr = DOMAIN_KERNEL_VALUE;

	domain_stack += (STACK_SIZE - sizeof(struct cpu_ctx));

	cpu_ctx = (struct cpu_ctx *)domain_stack;
	cpu_ctx->r0 = 0;
	cpu_ctx->r12 = start_info;
	cpu_ctx->usp = start_stack;
	cpu_ctx->ulr = 0;
	cpu_ctx->ssp = (unsigned long)(domain_stack + sizeof(struct cpu_ctx));
	cpu_ctx->pc = start_pc;

	cpu_ctx->spsr = 0x10;

	v->arch.ctx.r13 = (unsigned long)domain_stack;
	v->arch.ctx.r14 = (unsigned long)return_to_guest;

	v->arch.ctx.dacr = DOMAIN_KERNEL_VALUE;
	v->arch.ctx.fcseidr = 0;
	v->arch.ctx.contextidr = 0;
	v->arch.ctx.cpar = (0x40) | (1 << 13);
}

int construct_domain(struct domain *d,
	unsigned long guest_start, unsigned long guest_size,
	unsigned long image_start, unsigned long image_size,
	unsigned long initrd_start, unsigned long initrd_size,
	char *cmdline)
{
	char    *p = NULL;
	int     i;
	int     rc;

	unsigned long nr_pt_pages;
	unsigned long vstart, vend, map_track;

	struct start_info *si  = NULL;
	struct domain_setup_info dsi;
	struct vcpu *v         = NULL;

	BUG_ON(d == NULL);

	BUG_ON(d->vcpu[0] == NULL);

	/* Guest partition should be aligned to 1MB boundary */
	ASSERT((guest_size & 0xFFFFF) == 0);
	ASSERT((guest_start & 0xFFFFF) == 0);

	v = d->vcpu[0];

	BUG_ON(test_bit(_VCPUF_initialised, &v->vcpu_flags));

	write_ptbase(idle_domain->vcpu[0]);

	memset(&dsi, 0, sizeof(struct domain_setup_info));

	dsi.image_addr = image_start;
	dsi.image_len  = image_size;

	dsi.p_start = guest_start;
	dsi.p_end   = guest_start + guest_size;

	printk("Loading domain %d\n", d->domain_id);
	printk(" - partition : 0x%x ~ 0x%x\n", dsi.p_start, dsi.p_end);
	printk(" - image : 0x%x ~ 0x%x\n", dsi.image_addr, dsi.image_addr + dsi.image_len);

	d->max_pages = ~0U;

#ifdef CONFIG_VMM_SECURITY_IMAGE_VERIFICATION
	image_verification_for_security(dsi.image_addr, d->domain_id);
#endif

	rc = parseelfimage(&dsi);
	if (rc != 0) {
		return rc;
	}

#ifdef CONFIG_VMM_SECURITY_ACM
	d->scid = dsi.scid;
	d->acm_batterylife = 100;

	acm_weight_dom_cpu(d);
#else
	d->scid = ~(0x0UL);
#endif

	if (dsi.xen_section_string == NULL) {
		printk("Not a Xen-ELF image: '__xen_guest' section not found.\n");
		return -EINVAL;
	}

	if (set_guest_pages(d, guest_start, guest_size, ~ALLOC_DOM_DMA) == NULL) {
		printk("Not enough RAM for domain %d allocation.\n", d->domain_id);
		return -ENOMEM;
	}

	if (d->tot_pages < (guest_size >> PAGE_SHIFT))
		printk(" (%lu pages are allocated)", d->tot_pages);

	/* Mask all upcalls... */
	for (i = 0; i < MAX_VIRT_CPUS; i++)
		d->shared_info->vcpu_info[i].arch.cpsr |=  (VPSR_I_BIT | VPSR_F_BIT | VPSR_MODE_SVC);

	for (i = 1; i < num_online_cpus(); i++) {
		(void)alloc_vcpu(d, i, i);
	}

#define GPA_TO_GVA(gpa)	(gpa - guest_start + vstart)

	vstart = dsi.v_start & (~0xFFFFF);
	vend   = dsi.v_end;

	map_track = guest_start + PGD_ALIGN(vend - vstart);

	v->arch.ctx.ttbr0 = map_track;

	nr_pt_pages = prepare_guest_tables(v->arch.ctx.ttbr0, vstart, guest_start, guest_size);

	write_ptbase(v);

	map_track += (nr_pt_pages << PAGE_SHIFT);

	si = (start_info_t *)GPA_TO_GVA(map_track);

	memset(si, 0, PAGE_SIZE);

	si->nr_pages     = d->tot_pages;
	si->shared_info  = (unsigned long)d->shared_info;
	si->pt_base      = GPA_TO_GVA(v->arch.ctx.ttbr0);
	si->nr_pt_frames = nr_pt_pages;
	si->mfn_list     = 0;
	si->min_mfn      = guest_start >> PAGE_SHIFT;
	si->flags = 0;

	if(d->domain_id == 0) {
		si->flags = SIF_PRIVILEGED | SIF_INITDOMAIN;
	}

	map_track += PAGE_SIZE;

	if (d->domain_id != 0) {
		memset((void *)map_track, 0, (PAGE_SIZE * 2));

		si->store_mfn = map_track >> PAGE_SHIFT;
		si->store_evtchn = d->store_port;

		map_track += PAGE_SIZE;

		si->console_mfn = map_track >> PAGE_SHIFT;
		si->console_evtchn = d->console_port;

		map_track += PAGE_SIZE;

		printk("Store MFN = 0x%x\n",   si->store_mfn);
		printk("Console MFN = 0x%x\n", si->console_mfn);

		d->console_mfn = si->console_mfn;
		d->store_mfn = si->store_mfn;
	}

	/* Copy the OS image and free temporary buffer. */
	loadelfimage(&dsi);

	if (initrd_size != 0) {
		si->mod_start = GPA_TO_GVA(map_track);
		si->mod_len = initrd_size;

		printk("Initrd len 0x%lx, start at 0x%lx\n", si->mod_len, si->mod_start);

		memcpy((void *)si->mod_start, (const void *)(initrd_start), initrd_size);

		map_track = round_pgup(map_track + initrd_size);
	}

	memset(si->cmd_line, 0, sizeof(si->cmd_line));
	if (cmdline != NULL)
		strncpy((char *)si->cmd_line, cmdline, sizeof(si->cmd_line)-1);

	write_ptbase(current);

	new_thread(v, dsi.v_kernentry, map_track + PAGE_SIZE, (unsigned long)si);

	set_bit(_VCPUF_initialised, &v->vcpu_flags);

	if (d->domain_id == 0)
		set_bit(_DOMF_privileged, &d->domain_flags);

	i = 0;

#ifndef CONFIG_VMM_SECURITY_ACM
	i |= ioports_permit_access(d, 0, 0xFFFF);
	i |= iomem_permit_access(d, 0UL, ~0UL);
	i |= irqs_permit_access(d, 0, NR_PIRQS-1);
#endif

	BUG_ON(i != 0);

	return 0;
}

#ifdef CONFIG_VMM_SECURITY_IMAGE_VERIFICATION
/**
 *  @param dom_id domain id
 *  @return 0 if succeed, 1 if no image exists, -1 if fails
 **/
static int verify_image(void* image, int dom_id)
{
	void* signature = NULL;
	size_t image_size;
	size_t sig_size;
	image_type_t image_type;
	image_type_t sig_type;
	default_struct_t* part = NULL;

	/* get image and hash */
	switch (dom_id) {
		case 0:
			image_type = SECURE_DOM_IMG;
			sig_type = SECURE_DOM_SIGNED_HASH;
			break;
		case 1:
			image_type = DRIVER_DOM_IMG;
			sig_type = DRIVER_DOM_SIGNED_HASH;
			break;
		case 2:
			image_type = NORMAL_DOM1_IMG;
			sig_type = NORMAL_DOM1_SIGNED_HASH;
			break;
		case 3:
			image_type = NORMAL_DOM2_IMG;
			sig_type = NORMAL_DOM2_SIGNED_HASH;
			break;
		case 4:
			image_type = NORMAL_DOM3_IMG;
			sig_type = NORMAL_DOM3_SIGNED_HASH;
			break;
		case 5:
			image_type = NORMAL_DOM4_IMG;
			sig_type = NORMAL_DOM4_SIGNED_HASH;
			break;
		default:
			printk("verify_image(): Image is not registered\n");
			return 1;
	}

	/* get image */
	part = sra_get_image(PART_OS_IMAGE, image_type);

	ASSERT(part);

	if (part == NULL) {
		printk("Can't get image part %d\n", image_type);
		return 1;
	}
	image_size = part->size;

	/* get signature */
	part = sra_get_image(PART_SP1, sig_type);

	ASSERT(part);

	if (part == NULL) {
		printk("Can't get signature %d\n", sig_type);
		return 1;
	}
	signature = part->u.ptr;
	sig_size = part->size;

	return crypto_verify_data(image, image_size, signature, sig_size);
}

static void image_verification_for_security( unsigned long image_addr, domid_t domain_id)
{
	if (verify_image( (void *)image_addr, domain_id) != 0) {   
		printk("Verification of DOM%d fails\n", (domid_t) domain_id);
		return;
	}
        else {
		printk("Verification of DOM%d succeeds \n", (domid_t) domain_id);
	}
}
#endif

int elf_sanity_check(Elf_Ehdr *ehdr)
{
	if ( !IS_ELF(*ehdr) ||
		(ehdr->e_ident[EI_DATA] != ELFDATA2LSB) ||
		(ehdr->e_type != ET_EXEC) ) {
		printk("DOM0 image is not a Xen-compatible Elf image.\n");
		return 0;
	}

	return 1;
}
