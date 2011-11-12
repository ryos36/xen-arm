#include <os/os.h>
#include <os/irq.h>
#include <os/traps.h>
#include <os/evtchn.h>
#include <os/time.h>
#include <os/sched.h>
#include <os/lib.h>
#include <os/mm.h> 
#include <os/xmalloc.h>
#include <os/hypervisor.h>
#include <os/gnttab.h>

unsigned long min_mfn;
unsigned char zero_page[PAGE_SIZE];

start_info_t start_info;
shared_info_t *HYPERVISOR_shared_info = (shared_info_t *)&zero_page[0];

void show_start_info(start_info_t *si)
{
	printk("Xen Minimal OS!\n");
        printk("Magic : %s\n", si->magic);
	printk("Total Pages allocated to this domain : %ld\n", si->nr_pages);
	printk("VIRTUAL address of shared info struct : 0x%x\n", si->shared_info);
	printk("VIRTUAL address of page directory : 0x%x\n", si->pt_base);
	printk("Number of bootstrap p.t. frames : %ld\n", si->nr_pt_frames);
	printk("VIRTUAL address of page-frame list : 0x%x\n", si->mfn_list);
	printk("VIRTUAL address of pre-loaded module : 0x%x\n", si->mod_start);
	printk("Size (bytes) of pre-loaded modules : %ld\n", si->mod_len);
	printk("min mfn(min_page in xen) : %ld\n", si->min_mfn);
	printk("Command-Linux Address : 0x%x\n", si->cmd_line);
	printk("Command-Line String : %s\n", si->cmd_line);
	printk("  flags       : 0x%x\n",  (unsigned int)si->flags);

}

/* execute guest domain from dom0 */
void create_guest_domain()
{
        int ret;
        unsigned long domain_id;
        dom0_op_t dom0_op = { 0 };

        dom0_op.cmd = DOM0_CREATEDOMAIN;
        ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain execution failed!! \n");
        domain_id = dom0_op.u.createdomain.domain;

        dom0_op.cmd = DOM0_GUEST_IMAGE_CTL;
        dom0_op.u.guest_image_info.domain = domain_id;
        dom0_op.u.guest_image_info.sub_cmd = CMD_GUEST_CREATE;
        ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain execution failed!! \n");
	
	dom0_op.cmd = DOM0_UNPAUSEDOMAIN;
	dom0_op.u.unpausedomain.domain = domain_id;
	ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain unpause failed!! \n");
}

int is_current_dom0(void)
{
	if( (unsigned long *)HYPERVISOR_shared_info == (unsigned long *)0xFEBFF000 )
		return 1;
	else
		return 0;
}

void start_kernel(start_info_t *si)
{
	static char banner[] = "Bootstrapping .....  \n";

	printk(banner);
	
	show_start_info(si);
	memcpy(&start_info, (start_info_t *)si, sizeof(start_info_t));

	HYPERVISOR_shared_info = (shared_info_t *)start_info.shared_info;;
	printk("Shared Info : 0x%x\n", (unsigned long)HYPERVISOR_shared_info);

#if 0
	if( is_current_dom0() )
		create_guest_domain();
#endif

	while(1) 
		printk("[0] dom0\n");
}
