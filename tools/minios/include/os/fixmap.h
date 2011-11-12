/*
 * fixmap.h: compile-time virtual memory allocation
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1998 Ingo Molnar
 *
 * Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999
 */

#ifndef _ASM_FIXMAP_H
#define _ASM_FIXMAP_H

//#include <linux/config.h>

/* used by vmalloc.c, vsyscall.lds.S.
 *
 * Leave one empty page between vmalloc'ed areas and
 * the start of the fixmap.
 */
#define __FIXADDR_TOP	(HYPERVISOR_VIRT_START - 2 * PAGE_SIZE)

#ifndef __ASSEMBLY__
//#include <linux/kernel.h>
//#include <asm/acpi.h>
//#include <asm/apicdef.h>
//#include <asm/page.h>
#include <os/gnttab.h>
#ifdef CONFIG_HIGHMEM
//#include <linux/threads.h>
//#include <asm/kmap_types.h>
#endif

/*
 * Here we define all the compile-time 'special' virtual
 * addresses. The point is to have a constant address at
 * compile time, but to set the physical address only
 * in the boot process. We allocate these special addresses
 * from the end of virtual memory (0xfffff000) backwards.
 * Also this lets us do fail-safe vmalloc(), we
 * can guarantee that these special addresses and
 * vmalloc()-ed addresses never overlap.
 *
 * these 'compile-time allocated' memory buffers are
 * fixed-size 4k pages. (or larger if used with an increment
 * highger than 1) use fixmap_set(idx,phys) to associate
 * physical memory with fixmap indices.
 *
 * TLB entries of such buffers will not be flushed across
 * task switches.
 */
enum fixed_addresses {
	FIX_HOLE,
	FIX_VSYSCALL,
	FIX_SHARED_INFO,
	FIX_GNTTAB_BEGIN,
	FIX_GNTTAB_END = FIX_GNTTAB_BEGIN + NR_GRANT_FRAMES - 1,
	__end_of_permanent_fixed_addresses,
	/* temporary boot-time mappings, used before ioremap() is functional */
#define NR_FIX_BTMAPS	16
	FIX_BTMAP_END = __end_of_permanent_fixed_addresses,
	FIX_BTMAP_BEGIN = FIX_BTMAP_END + NR_FIX_BTMAPS - 1,
	FIX_WP_TEST,
	__end_of_fixed_addresses
};
//extern void __set_fixmap(
//	enum fixed_addresses idx, maddr_t phys, pgprot_t flags);

#define set_fixmap(idx, phys) \
		__set_fixmap(idx, phys, PAGE_KERNEL)
/*
 * Some hardware wants to get fixmapped without caching.
 */
#define set_fixmap_nocache(idx, phys) \
		__set_fixmap(idx, phys, PAGE_KERNEL_NOCACHE)

#define clear_fixmap(idx) \
		__set_fixmap(idx, 0, __pgprot(0))

#define FIXADDR_TOP	((unsigned long)__FIXADDR_TOP)

#define __FIXADDR_SIZE	(__end_of_permanent_fixed_addresses << PAGE_SHIFT)
#define __FIXADDR_BOOT_SIZE	(__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START		(FIXADDR_TOP - __FIXADDR_SIZE)
#define FIXADDR_BOOT_START	(FIXADDR_TOP - __FIXADDR_BOOT_SIZE)

#define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

/*
 * This is the range that is readable by user mode, and things
 * acting like user mode such as get_user_pages.
 */
#define FIXADDR_USER_START	(__fix_to_virt(FIX_VSYSCALL))
#define FIXADDR_USER_END	(FIXADDR_USER_START + PAGE_SIZE)


//extern void __this_fixmap_does_not_exist(void);

/*
 * 'index to address' translation. If anyone tries to use the idx
 * directly without tranlation, we catch the bug with a NULL-deference
 * kernel oops. Illegal ranges of incoming indices are caught too.
 */
static inline unsigned long fix_to_virt(const unsigned int idx)
{
	/*
	 * this branch gets completely eliminated after inlining,
	 * except when someone tries to use fixaddr indices in an
	 * illegal way. (such as mixing up address types or using
	 * out-of-range indices).
	 *
	 * If it doesn't get removed, the linker will complain
	 * loudly with a reasonably clear error message..
	 */
	#if 0
	if (idx >= __end_of_fixed_addresses)
		__this_fixmap_does_not_exist();
#endif
        return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
	if(vaddr >= FIXADDR_TOP || vaddr < FIXADDR_START)
		printf("BUG found in virt_to_fix at %s:%d\n", __FILE__, __LINE__);
	return __virt_to_fix(vaddr);
}

#endif /* !__ASSEMBLY__ */
#endif
