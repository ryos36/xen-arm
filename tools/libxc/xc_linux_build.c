/******************************************************************************
 * xc_linux_build.c
 */

/*
 * Some codes in this file was added
 * for supporting ARM processor by Samsung Electronics.
 */

#include "xg_private.h"
#include "xc_private.h"
#include <xenctrl.h>

#if defined(__i386__)
#define ELFSIZE 32
#endif

#if defined(__x86_64__) || defined(__ia64__)
#define ELFSIZE 64
#endif

#include "xc_elf.h"
#include "xc_aout9.h"
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

#if defined(__i386__)
#define L1_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED)
#define L2_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY|_PAGE_USER)
#define L3_PROT (_PAGE_PRESENT)
#endif

#if defined(__x86_64__)
#define L1_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_USER)
#define L2_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY|_PAGE_USER)
#define L3_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY|_PAGE_USER)
#define L4_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY|_PAGE_USER)
#endif

#ifdef __ia64__
#define get_tot_pages xc_get_max_pages
#else
#define get_tot_pages xc_get_tot_pages
#endif

#define round_pgup(_p)    (((_p)+(PAGE_SIZE-1))&PAGE_MASK)
#define round_pgdown(_p)  ((_p)&PAGE_MASK)

#ifdef __ia64__
#define probe_aout9(image,image_size,load_funcs) 1
#endif

struct initrd_info {
    enum { INITRD_none, INITRD_file, INITRD_mem } type;
    unsigned long len;
    union {
        gzFile file_handle;
        char *mem_addr;
    } u;
};

#if 0
static const char *feature_names[XENFEAT_NR_SUBMAPS*32] = {
    [XENFEAT_writable_page_tables]       = "writable_page_tables",
    [XENFEAT_writable_descriptor_tables] = "writable_descriptor_tables",
    [XENFEAT_auto_translated_physmap]    = "auto_translated_physmap",
    [XENFEAT_supervisor_mode_kernel]     = "supervisor_mode_kernel",
    [XENFEAT_pae_pgdir_above_4gb]        = "pae_pgdir_above_4gb"
};
#endif

static inline void set_feature_bit (int nr, uint32_t *addr)
{
    addr[nr>>5] |= (1<<(nr&31));
}

static inline int test_feature_bit(int nr, uint32_t *addr)
{
    return !!(addr[nr>>5] & (1<<(nr&31)));
}

#if 0
static int parse_features(
    const char *feats,
    uint32_t supported[XENFEAT_NR_SUBMAPS],
    uint32_t required[XENFEAT_NR_SUBMAPS])
{
    const char *end, *p;
    int i, req;

    if ( (end = strchr(feats, ',')) == NULL )
        end = feats + strlen(feats);

    while ( feats < end )
    {
        p = strchr(feats, '|');
        if ( (p == NULL) || (p > end) )
            p = end;

        req = (*feats == '!');
        if ( req )
            feats++;

        for ( i = 0; i < XENFEAT_NR_SUBMAPS*32; i++ )
        {
            if ( feature_names[i] == NULL )
                continue;

            if ( strncmp(feature_names[i], feats, p-feats) == 0 )
            {
                set_feature_bit(i, supported);
                if ( required && req )
                    set_feature_bit(i, required);
                break;
            }
        }

        if ( i == XENFEAT_NR_SUBMAPS*32 )
        {
            ERROR("Unknown feature \"%.*s\".", (int)(p-feats), feats);
            if ( req )
            {
                ERROR("Kernel requires an unknown hypervisor feature.");
                return -EINVAL;
            }
        }

        feats = p;
        if ( *feats == '|' )
            feats++;
    }

    return -EINVAL;
}
#endif

#if 0
static int probeimageformat(const char *image,
                            unsigned long image_size,
                            struct load_funcs *load_funcs)
{
    if ( probe_elf(image, image_size, load_funcs) &&
         probe_bin(image, image_size, load_funcs) &&
         probe_aout9(image, image_size, load_funcs) )
    {
        ERROR( "Unrecognized image format" );
        return -EINVAL;
    }

    return 0;
}
#endif

int load_initrd(int xc_handle, domid_t dom,
                struct initrd_info *initrd,
                unsigned long physbase,
                unsigned long *phys_to_mach)
{
    char page[PAGE_SIZE];
    unsigned long pfn_start, pfn, nr_pages;

    if ( initrd->type == INITRD_none )
        return 0;

    pfn_start = physbase >> PAGE_SHIFT;
    nr_pages  = (initrd->len + PAGE_SIZE - 1) >> PAGE_SHIFT;

    for ( pfn = pfn_start; pfn < (pfn_start + nr_pages); pfn++ )
    {
        if ( initrd->type == INITRD_mem )
        {
            xc_copy_to_domain_page(
                xc_handle, dom, phys_to_mach[pfn],
                &initrd->u.mem_addr[(pfn - pfn_start) << PAGE_SHIFT]);
        }
        else
        {
            if ( gzread(initrd->u.file_handle, page, PAGE_SIZE) == -1 )
            {
                PERROR("Error reading initrd image, could not");
                return -EINVAL;
            }
            xc_copy_to_domain_page(xc_handle, dom, phys_to_mach[pfn], page);
        }
    }

    return 0;
}

#define alloc_pt(ltab, vltab, pltab)                                    \
do {                                                                    \
    pltab = ppt_alloc++;                                                \
    ltab = (uint64_t)page_array[pltab] << PAGE_SHIFT;                   \
    pltab <<= PAGE_SHIFT;                                               \
    if ( vltab != NULL )                                                \
        munmap(vltab, PAGE_SIZE);                                       \
    if ( (vltab = xc_map_foreign_range(xc_handle, dom, PAGE_SIZE,       \
                                       PROT_READ|PROT_WRITE,            \
                                       ltab >> PAGE_SHIFT)) == NULL )   \
        goto error_out;                                                 \
    memset(vltab, 0x0, PAGE_SIZE);                                      \
} while ( 0 )

#if defined(__i386__)

static int setup_pg_tables(int xc_handle, uint32_t dom,
                           vcpu_guest_context_t *ctxt,
                           unsigned long dsi_v_start,
                           unsigned long v_end,
                           unsigned long *page_array,
                           unsigned long vpt_start,
                           unsigned long vpt_end,
                           unsigned shadow_mode_enabled)
{
    l1_pgentry_t *vl1tab=NULL, *vl1e=NULL;
    l2_pgentry_t *vl2tab=NULL, *vl2e=NULL;
    unsigned long l1tab = 0, pl1tab;
    unsigned long l2tab = 0, pl2tab;
    unsigned long ppt_alloc;
    unsigned long count;

    ppt_alloc = (vpt_start - dsi_v_start) >> PAGE_SHIFT;
    alloc_pt(l2tab, vl2tab, pl2tab);
    vl2e = &vl2tab[l2_table_offset(dsi_v_start)];
    if (shadow_mode_enabled)
        ctxt->ctrlreg[3] = pl2tab;
    else
        ctxt->ctrlreg[3] = l2tab;

    for ( count = 0; count < ((v_end - dsi_v_start) >> PAGE_SHIFT); count++ )
    {
        if ( ((unsigned long)vl1e & (PAGE_SIZE-1)) == 0 )
        {
            alloc_pt(l1tab, vl1tab, pl1tab);
            vl1e = &vl1tab[l1_table_offset(dsi_v_start + (count<<PAGE_SHIFT))];
            if (shadow_mode_enabled)
                *vl2e = pl1tab | L2_PROT;
            else
                *vl2e = l1tab | L2_PROT;
            vl2e++;
        }

        if ( shadow_mode_enabled )
        {
            *vl1e = (count << PAGE_SHIFT) | L1_PROT;
        }
        else
        {
            *vl1e = (page_array[count] << PAGE_SHIFT) | L1_PROT;
            if ( (count >= ((vpt_start-dsi_v_start)>>PAGE_SHIFT)) && 
                 (count <  ((vpt_end  -dsi_v_start)>>PAGE_SHIFT)) )
                *vl1e &= ~_PAGE_RW;
        }
        vl1e++;
    }
    munmap(vl1tab, PAGE_SIZE);
    munmap(vl2tab, PAGE_SIZE);
    return 0;

 error_out:
    if (vl1tab)
        munmap(vl1tab, PAGE_SIZE);
    if (vl2tab)
        munmap(vl2tab, PAGE_SIZE);
    return -1;
}

static int setup_pg_tables_pae(int xc_handle, uint32_t dom,
                               vcpu_guest_context_t *ctxt,
                               unsigned long dsi_v_start,
                               unsigned long v_end,
                               unsigned long *page_array,
                               unsigned long vpt_start,
                               unsigned long vpt_end,
                               unsigned shadow_mode_enabled)
{
    l1_pgentry_64_t *vl1tab = NULL, *vl1e = NULL;
    l2_pgentry_64_t *vl2tab = NULL, *vl2e = NULL;
    l3_pgentry_64_t *vl3tab = NULL, *vl3e = NULL;
    uint64_t l1tab, l2tab, l3tab, pl1tab, pl2tab, pl3tab;
    unsigned long ppt_alloc, count, nmfn;

    /* First allocate page for page dir. */
    ppt_alloc = (vpt_start - dsi_v_start) >> PAGE_SHIFT;

    if ( page_array[ppt_alloc] > 0xfffff )
    {
        nmfn = xc_make_page_below_4G(xc_handle, dom, page_array[ppt_alloc]);
        if ( nmfn == 0 )
        {
            fprintf(stderr, "Couldn't get a page below 4GB :-(\n");
            goto error_out;
        }
        page_array[ppt_alloc] = nmfn;
    }

    alloc_pt(l3tab, vl3tab, pl3tab);
    vl3e = &vl3tab[l3_table_offset_pae(dsi_v_start)];
    if (shadow_mode_enabled)
        ctxt->ctrlreg[3] = pl3tab;
    else
        ctxt->ctrlreg[3] = l3tab;

    for ( count = 0; count < ((v_end - dsi_v_start) >> PAGE_SHIFT); count++)
    {
        if ( !((unsigned long)vl1e & (PAGE_SIZE-1)) )
        {
            if ( !((unsigned long)vl2e & (PAGE_SIZE-1)) )
            {
                alloc_pt(l2tab, vl2tab, pl2tab);
                vl2e = &vl2tab[l2_table_offset_pae(
                    dsi_v_start + (count << PAGE_SHIFT))];
                if (shadow_mode_enabled)
                    *vl3e = pl2tab | L3_PROT;
                else
                    *vl3e++ = l2tab | L3_PROT;
            }

            alloc_pt(l1tab, vl1tab, pl1tab);
            vl1e = &vl1tab[l1_table_offset_pae(
                dsi_v_start + (count << PAGE_SHIFT))];
            if (shadow_mode_enabled)
                *vl2e = pl1tab | L2_PROT;
            else
                *vl2e++ = l1tab | L2_PROT;
        }
        
        if ( shadow_mode_enabled )
        {
            *vl1e = (count << PAGE_SHIFT) | L1_PROT;
        }
        else
        {
            *vl1e = ((uint64_t)page_array[count] << PAGE_SHIFT) | L1_PROT;
            if ( (count >= ((vpt_start-dsi_v_start)>>PAGE_SHIFT)) &&
                 (count <  ((vpt_end  -dsi_v_start)>>PAGE_SHIFT)) ) 
                *vl1e &= ~_PAGE_RW;
        }
        vl1e++;
    }
     
    munmap(vl1tab, PAGE_SIZE);
    munmap(vl2tab, PAGE_SIZE);
    munmap(vl3tab, PAGE_SIZE);
    return 0;

 error_out:
    if (vl1tab)
        munmap(vl1tab, PAGE_SIZE);
    if (vl2tab)
        munmap(vl2tab, PAGE_SIZE);
    if (vl3tab)
        munmap(vl3tab, PAGE_SIZE);
    return -1;
}

#endif

#if defined(__x86_64__)

static int setup_pg_tables_64(int xc_handle, uint32_t dom,
                              vcpu_guest_context_t *ctxt,
                              unsigned long dsi_v_start,
                              unsigned long v_end,
                              unsigned long *page_array,
                              unsigned long vpt_start,
                              unsigned long vpt_end,
                              int shadow_mode_enabled)
{
    l1_pgentry_t *vl1tab=NULL, *vl1e=NULL;
    l2_pgentry_t *vl2tab=NULL, *vl2e=NULL;
    l3_pgentry_t *vl3tab=NULL, *vl3e=NULL;
    l4_pgentry_t *vl4tab=NULL, *vl4e=NULL;
    unsigned long l2tab = 0, pl2tab;
    unsigned long l1tab = 0, pl1tab;
    unsigned long l3tab = 0, pl3tab;
    unsigned long l4tab = 0, pl4tab;
    unsigned long ppt_alloc;
    unsigned long count;

    /* First allocate page for page dir. */
    ppt_alloc = (vpt_start - dsi_v_start) >> PAGE_SHIFT;
    alloc_pt(l4tab, vl4tab, pl4tab);
    vl4e = &vl4tab[l4_table_offset(dsi_v_start)];
    if (shadow_mode_enabled)
        ctxt->ctrlreg[3] = pl4tab;
    else
        ctxt->ctrlreg[3] = l4tab;
    
    for ( count = 0; count < ((v_end-dsi_v_start)>>PAGE_SHIFT); count++)
    {
        if ( !((unsigned long)vl1e & (PAGE_SIZE-1)) )
        {
            alloc_pt(l1tab, vl1tab, pl1tab);
            
            if ( !((unsigned long)vl2e & (PAGE_SIZE-1)) )
            {
                alloc_pt(l2tab, vl2tab, pl2tab);
                if ( !((unsigned long)vl3e & (PAGE_SIZE-1)) )
                {
                    alloc_pt(l3tab, vl3tab, pl3tab);
                    vl3e = &vl3tab[l3_table_offset(dsi_v_start + (count<<PAGE_SHIFT))];
                    if (shadow_mode_enabled)
                        *vl4e = pl3tab | L4_PROT;
                    else
                        *vl4e = l3tab | L4_PROT;
                    vl4e++;
                }
                vl2e = &vl2tab[l2_table_offset(dsi_v_start + (count<<PAGE_SHIFT))];
                if (shadow_mode_enabled)
                    *vl3e = pl2tab | L3_PROT;
                else
                    *vl3e = l2tab | L3_PROT;
                vl3e++;
            }
            vl1e = &vl1tab[l1_table_offset(dsi_v_start + (count<<PAGE_SHIFT))];
            if (shadow_mode_enabled)
                *vl2e = pl1tab | L2_PROT;
            else
                *vl2e = l1tab | L2_PROT;
            vl2e++;
        }
        
        if ( shadow_mode_enabled )
        {
            *vl1e = (count << PAGE_SHIFT) | L1_PROT;
        }
        else
        {
            *vl1e = (page_array[count] << PAGE_SHIFT) | L1_PROT;
            if ( (count >= ((vpt_start-dsi_v_start)>>PAGE_SHIFT)) &&
                 (count <  ((vpt_end  -dsi_v_start)>>PAGE_SHIFT)) ) 
                {
                    *vl1e &= ~_PAGE_RW;
                }
        }
        vl1e++;
    }
     
    munmap(vl1tab, PAGE_SIZE);
    munmap(vl2tab, PAGE_SIZE);
    munmap(vl3tab, PAGE_SIZE);
    munmap(vl4tab, PAGE_SIZE);
    return 0;

 error_out:
    if (vl1tab)
        munmap(vl1tab, PAGE_SIZE);
    if (vl2tab)
        munmap(vl2tab, PAGE_SIZE);
    if (vl3tab)
        munmap(vl3tab, PAGE_SIZE);
    if (vl4tab)
        munmap(vl4tab, PAGE_SIZE);
    return -1;
}
#endif

#ifdef __ia64__
extern unsigned long xc_ia64_fpsr_default(void);

static int setup_guest(int xc_handle,
                       uint32_t dom,
                       const char *image, unsigned long image_size,
                       struct initrd_info *initrd,
                       unsigned long nr_pages,
                       unsigned long *pvsi, unsigned long *pvke,
                       unsigned long *pvss, vcpu_guest_context_t *ctxt,
                       const char *cmdline,
                       unsigned long shared_info_frame,
                       unsigned long flags,
                       unsigned int store_evtchn, unsigned long *store_mfn,
                       unsigned int console_evtchn, unsigned long *console_mfn,
                       uint32_t required_features[XENFEAT_NR_SUBMAPS])
{
    unsigned long *page_array = NULL;
    struct load_funcs load_funcs;
    struct domain_setup_info dsi;
    unsigned long vinitrd_start;
    unsigned long vinitrd_end;
    unsigned long v_end;
    unsigned long start_page, pgnr;
    start_info_t *start_info;
    int rc;

    rc = probeimageformat(image, image_size, &load_funcs);
    if ( rc != 0 )
        goto error_out;

    memset(&dsi, 0, sizeof(struct domain_setup_info));

    rc = (load_funcs.parseimage)(image, image_size, &dsi);
    if ( rc != 0 )
        goto error_out;

    dsi.v_start      = round_pgdown(dsi.v_start);
    vinitrd_start    = round_pgup(dsi.v_end);
    vinitrd_end      = vinitrd_start + initrd->len;
    v_end            = round_pgup(vinitrd_end);

    start_page = dsi.v_start >> PAGE_SHIFT;
    pgnr = (v_end - dsi.v_start) >> PAGE_SHIFT;
    if ( (page_array = malloc(pgnr * sizeof(unsigned long))) == NULL )
    {
        PERROR("Could not allocate memory");
        goto error_out;
    }

    if ( xc_ia64_get_pfn_list(xc_handle, dom, page_array,
                              start_page, pgnr) != pgnr )
    {
        PERROR("Could not get the page frame list");
        goto error_out;
    }

#define _p(a) ((void *) (a))

    printf("VIRTUAL MEMORY ARRANGEMENT:\n"
           " Loaded kernel: %p->%p\n"
           " Init. ramdisk: %p->%p\n"
           " TOTAL:         %p->%p\n",
           _p(dsi.v_kernstart), _p(dsi.v_kernend), 
           _p(vinitrd_start),   _p(vinitrd_end),
           _p(dsi.v_start),     _p(v_end));
    printf(" ENTRY ADDRESS: %p\n", _p(dsi.v_kernentry));

    (load_funcs.loadimage)(image, image_size, xc_handle, dom, page_array,
                           &dsi);

    if ( load_initrd(xc_handle, dom, initrd,
                     vinitrd_start - dsi.v_start, page_array) )
        goto error_out;

    *pvke = dsi.v_kernentry;

    /* Now need to retrieve machine pfn for system pages:
     *  start_info/store/console
     */
    pgnr = 3;
    if ( xc_ia64_get_pfn_list(xc_handle, dom, page_array,
                              nr_pages - 3, pgnr) != pgnr )
    {
        PERROR("Could not get page frame for xenstore");
        goto error_out;
    }

    *store_mfn = page_array[1];
    *console_mfn = page_array[2];
    printf("start_info: 0x%lx at 0x%lx, "
           "store_mfn: 0x%lx at 0x%lx, "
           "console_mfn: 0x%lx at 0x%lx\n",
           page_array[0], nr_pages,
           *store_mfn,    nr_pages - 2,
           *console_mfn,  nr_pages - 1);

    start_info = xc_map_foreign_range(
        xc_handle, dom, PAGE_SIZE, PROT_READ|PROT_WRITE, page_array[0]);
    memset(start_info, 0, sizeof(*start_info));
    rc = xc_version(xc_handle, XENVER_version, NULL);
    sprintf(start_info->magic, "xen-%i.%i-ia64", rc >> 16, rc & (0xFFFF));
    start_info->flags        = flags;
    start_info->store_mfn    = nr_pages - 2;
    start_info->store_evtchn = store_evtchn;
    start_info->console_mfn   = nr_pages - 1;
    start_info->console_evtchn = console_evtchn;
    start_info->nr_pages       = nr_pages; // FIXME?: nr_pages - 2 ????
    if ( initrd->len != 0 )
    {
        ctxt->initrd.start    = vinitrd_start;
        ctxt->initrd.size     = initrd->len;
    }
    else
    {
        ctxt->initrd.start    = 0;
        ctxt->initrd.size     = 0;
    }
    if ( cmdline != NULL )
    {
        strncpy((char *)ctxt->cmdline, cmdline, IA64_COMMAND_LINE_SIZE);
        ctxt->cmdline[IA64_COMMAND_LINE_SIZE-1] = '\0';
    }
    munmap(start_info, PAGE_SIZE);

    free(page_array);
    return 0;

 error_out:
    free(page_array);
    return -1;
}
#else /* x86 */

#if 0
/* Check if the platform supports the guest kernel format */
static int compat_check(int xc_handle, struct domain_setup_info *dsi)
{
    xen_capabilities_info_t xen_caps = "";

    if (xc_version(xc_handle, XENVER_capabilities, &xen_caps) != 0) {
        ERROR("Cannot determine host capabilities.");
        return 0;
    }

    if (strstr(xen_caps, "xen-3.0-x86_32p")) {
        if (!dsi->pae_kernel) {
            ERROR("Non PAE-kernel on PAE host.");
            return 0;
        }
    } else if (dsi->pae_kernel) {
        ERROR("PAE-kernel on non-PAE host.");
        return 0;
    }

    return 1;
}
#endif


#if 0
static int setup_guest(int xc_handle,
                       uint32_t dom,
                       const char *image, unsigned long image_size,
                       struct initrd_info *initrd,
                       unsigned long nr_pages,
                       unsigned long *pvsi, unsigned long *pvke,
                       unsigned long *pvss, vcpu_guest_context_t *ctxt,
                       const char *cmdline,
                       unsigned long shared_info_frame,
                       unsigned long flags,
                       unsigned int store_evtchn, unsigned long *store_mfn,
                       unsigned int console_evtchn, unsigned long *console_mfn,
                       uint32_t required_features[XENFEAT_NR_SUBMAPS])
{
    unsigned long *page_array = NULL;
    unsigned long count, i, hypercall_pfn;
    start_info_t *start_info;
    shared_info_t *shared_info;
    xc_mmu_t *mmu = NULL;
    char *p;
    DECLARE_DOM0_OP;
    int rc;

    unsigned long nr_pt_pages;
    unsigned long physmap_pfn;
    unsigned long *physmap, *physmap_e;

    struct load_funcs load_funcs;
    struct domain_setup_info dsi;
    unsigned long vinitrd_start;
    unsigned long vphysmap_start;
    unsigned long vstartinfo_start;
    unsigned long vstoreinfo_start;
    unsigned long vconsole_start;
    unsigned long vsharedinfo_start = 0; /* XXX gcc */
    unsigned long vstack_start;
    unsigned long vstack_end;
    unsigned long vpt_start;
    unsigned long vpt_end;
    unsigned long v_end;
    unsigned long guest_store_mfn, guest_console_mfn, guest_shared_info_mfn;
    unsigned long shadow_mode_enabled;
    uint32_t supported_features[XENFEAT_NR_SUBMAPS] = { 0, };

    rc = probeimageformat(image, image_size, &load_funcs);
    if ( rc != 0 )
        goto error_out;

    memset(&dsi, 0, sizeof(struct domain_setup_info));

    rc = (load_funcs.parseimage)(image, image_size, &dsi);
    if ( rc != 0 )
        goto error_out;

    if ( (dsi.v_start & (PAGE_SIZE-1)) != 0 )
    {
        PERROR("Guest OS must load to a page boundary.");
        goto error_out;
    }

    if (!compat_check(xc_handle, &dsi))
        goto error_out;

    /* Parse and validate kernel features. */
    p = strstr(dsi.xen_guest_string, "FEATURES=");
    if ( p != NULL )
    {
        if ( !parse_features(p + strlen("FEATURES="),
                             supported_features,
                             required_features) )
        {
            ERROR("Failed to parse guest kernel features.");
            goto error_out;
        }

        printf("Supported features  = { %08x }.\n", supported_features[0]);
        printf("Required features   = { %08x }.\n", required_features[0]);
    }

    for ( i = 0; i < XENFEAT_NR_SUBMAPS; i++ )
    {
        if ( (supported_features[i]&required_features[i]) != required_features[i] )
        {
            ERROR("Guest kernel does not support a required feature.");
            goto error_out;
        }
    }

    shadow_mode_enabled = test_feature_bit(XENFEAT_auto_translated_physmap,
                                           required_features);

    /*
     * Why do we need this? The number of page-table frames depends on the 
     * size of the bootstrap address space. But the size of the address space 
     * depends on the number of page-table frames (since each one is mapped 
     * read-only). We have a pair of simultaneous equations in two unknowns, 
     * which we solve by exhaustive search.
     */
    v_end = round_pgup(dsi.v_end);
    vinitrd_start = v_end;
    v_end += round_pgup(initrd->len);
    vphysmap_start = v_end;
    v_end += round_pgup(nr_pages * sizeof(unsigned long));
    vstartinfo_start = v_end;
    v_end += PAGE_SIZE;
    vstoreinfo_start = v_end;
    v_end += PAGE_SIZE;
    vconsole_start = v_end;
    v_end += PAGE_SIZE;
    if ( shadow_mode_enabled ) {
        vsharedinfo_start = v_end;
        v_end += PAGE_SIZE;
    }
    vpt_start = v_end;

    for ( nr_pt_pages = 2; ; nr_pt_pages++ )
    {
        vpt_end          = vpt_start + (nr_pt_pages * PAGE_SIZE);
        vstack_start     = vpt_end;
        vstack_end       = vstack_start + PAGE_SIZE;
        v_end            = (vstack_end + (1UL<<22)-1) & ~((1UL<<22)-1);
        if ( (v_end - vstack_end) < (512UL << 10) )
            v_end += 1UL << 22; /* Add extra 4MB to get >= 512kB padding. */
#if defined(__i386__)
        if ( dsi.pae_kernel )
        {
            /* FIXME: assumes one L2 pgtable @ 0xc0000000 */
            if ( (((v_end - dsi.v_start + ((1<<L2_PAGETABLE_SHIFT_PAE)-1)) >> 
                   L2_PAGETABLE_SHIFT_PAE) + 2) <= nr_pt_pages )
                break;
        }
        else
        {
            if ( (((v_end - dsi.v_start + ((1<<L2_PAGETABLE_SHIFT)-1)) >> 
                   L2_PAGETABLE_SHIFT) + 1) <= nr_pt_pages )
                break;
        }
#endif
#if defined(__x86_64__)
#define NR(_l,_h,_s) \
    (((((_h) + ((1UL<<(_s))-1)) & ~((1UL<<(_s))-1)) - \
    ((_l) & ~((1UL<<(_s))-1))) >> (_s))
        if ( (1 + /* # L4 */
              NR(dsi.v_start, v_end, L4_PAGETABLE_SHIFT) + /* # L3 */
              NR(dsi.v_start, v_end, L3_PAGETABLE_SHIFT) + /* # L2 */
              NR(dsi.v_start, v_end, L2_PAGETABLE_SHIFT))  /* # L1 */
             <= nr_pt_pages )
            break;
#endif
    }

#define _p(a) ((void *) (a))

    printf("VIRTUAL MEMORY ARRANGEMENT:\n");
    printf(" Loaded kernel:    %p->%p\n", _p(dsi.v_kernstart),
           _p(dsi.v_kernend));
    if ( initrd->len )
        printf(" Initial ramdisk:  %p->%p\n", _p(vinitrd_start),
               _p(vinitrd_start + initrd->len));
    printf(" Phys-Mach map:    %p\n", _p(vphysmap_start));
    printf(" Start info:       %p\n", _p(vstartinfo_start));
    printf(" Store page:       %p\n", _p(vstoreinfo_start));
    printf(" Console page:     %p\n", _p(vconsole_start));
    if ( shadow_mode_enabled )
        printf(" Shared Info page: %p\n", _p(vsharedinfo_start));
    printf(" Page tables:      %p\n", _p(vpt_start));
    printf(" Boot stack:       %p\n", _p(vstack_start));
    printf(" TOTAL:            %p->%p\n", _p(dsi.v_start), _p(v_end));
    printf(" ENTRY ADDRESS:    %p\n", _p(dsi.v_kernentry));

    if ( ((v_end - dsi.v_start)>>PAGE_SHIFT) > nr_pages )
    {
        PERROR("Initial guest OS requires too much space\n"
               "(%luMB is greater than %luMB limit)\n",
               (v_end-dsi.v_start)>>20, nr_pages>>(20-PAGE_SHIFT));
        goto error_out;
    }

    if ( (page_array = malloc(nr_pages * sizeof(unsigned long))) == NULL )
    {
        PERROR("Could not allocate memory");
        goto error_out;
    }

    if ( xc_get_pfn_list(xc_handle, dom, page_array, nr_pages) != nr_pages )
    {
        PERROR("Could not get the page frame list");
        goto error_out;
    }

    (load_funcs.loadimage)(image, image_size,
                           xc_handle, dom, page_array,
                           &dsi);

    if ( load_initrd(xc_handle, dom, initrd,
                     vinitrd_start - dsi.v_start, page_array) )
        goto error_out;

    /* setup page tables */
#if defined(__i386__)
    if (dsi.pae_kernel)
        rc = setup_pg_tables_pae(xc_handle, dom, ctxt,
                                 dsi.v_start, v_end,
                                 page_array, vpt_start, vpt_end,
                                 shadow_mode_enabled);
    else
        rc = setup_pg_tables(xc_handle, dom, ctxt,
                             dsi.v_start, v_end,
                             page_array, vpt_start, vpt_end,
                             shadow_mode_enabled);
#endif
#if defined(__x86_64__)
    rc = setup_pg_tables_64(xc_handle, dom, ctxt,
                            dsi.v_start, v_end,
                            page_array, vpt_start, vpt_end,
                            shadow_mode_enabled);
#endif
    if (0 != rc)
        goto error_out;

#if defined(__i386__)
    /*
     * Pin down l2tab addr as page dir page - causes hypervisor to provide
     * correct protection for the page
     */
    if ( !shadow_mode_enabled )
    {
        if ( dsi.pae_kernel )
        {
            if ( pin_table(xc_handle, MMUEXT_PIN_L3_TABLE,
                           ctxt->ctrlreg[3] >> PAGE_SHIFT, dom) )
                goto error_out;
        }
        else
        {
            if ( pin_table(xc_handle, MMUEXT_PIN_L2_TABLE,
                           ctxt->ctrlreg[3] >> PAGE_SHIFT, dom) )
                goto error_out;
        }
    }
#endif

#if defined(__x86_64__)
    /*
     * Pin down l4tab addr as page dir page - causes hypervisor to  provide
     * correct protection for the page
     */
    if ( pin_table(xc_handle, MMUEXT_PIN_L4_TABLE,
                   ctxt->ctrlreg[3] >> PAGE_SHIFT, dom) )
        goto error_out;
#endif

    if ( (mmu = xc_init_mmu_updates(xc_handle, dom)) == NULL )
        goto error_out;

    /* Write the phys->machine and machine->phys table entries. */
    physmap_pfn = (vphysmap_start - dsi.v_start) >> PAGE_SHIFT;
    physmap = physmap_e = xc_map_foreign_range(
        xc_handle, dom, PAGE_SIZE, PROT_READ|PROT_WRITE,
        page_array[physmap_pfn++]);

    for ( count = 0; count < nr_pages; count++ )
    {
        if ( xc_add_mmu_update(
            xc_handle, mmu,
            ((uint64_t)page_array[count] << PAGE_SHIFT) | MMU_MACHPHYS_UPDATE,
            count) )
        {
            fprintf(stderr,"m2p update failure p=%lx m=%lx\n",
                    count, page_array[count]); 
            munmap(physmap, PAGE_SIZE);
            goto error_out;
        }
        *physmap_e++ = page_array[count];
        if ( ((unsigned long)physmap_e & (PAGE_SIZE-1)) == 0 )
        {
            munmap(physmap, PAGE_SIZE);
            physmap = physmap_e = xc_map_foreign_range(
                xc_handle, dom, PAGE_SIZE, PROT_READ|PROT_WRITE,
                page_array[physmap_pfn++]);
        }
    }
    munmap(physmap, PAGE_SIZE);

    /* Send the page update requests down to the hypervisor. */
    if ( xc_finish_mmu_updates(xc_handle, mmu) )
        goto error_out;

    if ( shadow_mode_enabled )
    {
        struct xen_add_to_physmap xatp;

        /* Enable shadow translate mode */
        if ( xc_shadow_control(xc_handle, dom,
                               DOM0_SHADOW_CONTROL_OP_ENABLE_TRANSLATE,
                               NULL, 0, NULL) < 0 )
        {
            PERROR("Could not enable translation mode");
            goto error_out;
        }

        guest_shared_info_mfn = (vsharedinfo_start-dsi.v_start) >> PAGE_SHIFT;

        /* Map shared info frame into guest physmap. */
        xatp.domid = dom;
        xatp.space = XENMAPSPACE_shared_info;
        xatp.idx   = 0;
        xatp.gpfn  = guest_shared_info_mfn;
        rc = xc_memory_op(xc_handle, XENMEM_add_to_physmap, &xatp);
        if ( rc != 0 )
        {
            PERROR("Cannot map shared info pfn");
            goto error_out;
        }

        /* Map grant table frames into guest physmap. */
        for ( i = 0; ; i++ )
        {
            xatp.domid = dom;
            xatp.space = XENMAPSPACE_grant_table;
            xatp.idx   = i;
            xatp.gpfn  = nr_pages + i;
            rc = xc_memory_op(xc_handle, XENMEM_add_to_physmap, &xatp);
            if ( rc != 0 )
            {
                if ( errno == EINVAL )
                    break; /* done all grant tables */
                PERROR("Cannot map grant table pfn");
                goto error_out;
            }
        }
    }
    else
    {
        guest_shared_info_mfn = shared_info_frame;
    }

    *store_mfn = page_array[(vstoreinfo_start-dsi.v_start) >> PAGE_SHIFT];
    *console_mfn = page_array[(vconsole_start-dsi.v_start) >> PAGE_SHIFT];
    if ( xc_clear_domain_page(xc_handle, dom, *store_mfn) ||
         xc_clear_domain_page(xc_handle, dom, *console_mfn) )
        goto error_out;
    if ( shadow_mode_enabled )
    {
        guest_store_mfn = (vstoreinfo_start-dsi.v_start) >> PAGE_SHIFT;
        guest_console_mfn = (vconsole_start-dsi.v_start) >> PAGE_SHIFT;
    }
    else
    {
        guest_store_mfn = *store_mfn;
        guest_console_mfn = *console_mfn;
    }

    start_info = xc_map_foreign_range(
        xc_handle, dom, PAGE_SIZE, PROT_READ|PROT_WRITE,
        page_array[(vstartinfo_start-dsi.v_start)>>PAGE_SHIFT]);
    /*shared_info, start_info */
    memset(start_info, 0, sizeof(*start_info));
    rc = xc_version(xc_handle, XENVER_version, NULL);
    sprintf(start_info->magic, "xen-%i.%i-x86_%d%s",
            rc >> 16, rc & (0xFFFF), (unsigned int)sizeof(long)*8,
            dsi.pae_kernel ? "p" : "");
    start_info->nr_pages     = nr_pages;
    start_info->shared_info  = guest_shared_info_mfn << PAGE_SHIFT;
    start_info->flags        = flags;
    start_info->pt_base      = vpt_start;
    start_info->nr_pt_frames = nr_pt_pages;
    start_info->mfn_list     = vphysmap_start;
    start_info->store_mfn    = guest_store_mfn;
    start_info->store_evtchn = store_evtchn;
    start_info->console_mfn   = guest_console_mfn;
    start_info->console_evtchn = console_evtchn;
    if ( initrd->len != 0 )
    {
        start_info->mod_start    = vinitrd_start;
        start_info->mod_len      = initrd->len;
    }
    if ( cmdline != NULL )
    { 
        strncpy((char *)start_info->cmd_line, cmdline, MAX_GUEST_CMDLINE);
        start_info->cmd_line[MAX_GUEST_CMDLINE-1] = '\0';
    }
    munmap(start_info, PAGE_SIZE);

    /* shared_info page starts its life empty. */
    shared_info = xc_map_foreign_range(
        xc_handle, dom, PAGE_SIZE, PROT_READ|PROT_WRITE, shared_info_frame);
    memset(shared_info, 0, sizeof(shared_info_t));
    /* Mask all upcalls... */
    for ( i = 0; i < MAX_VIRT_CPUS; i++ )
        shared_info->vcpu_info[i].evtchn_upcall_mask = 1;

    munmap(shared_info, PAGE_SIZE);

    /* Send the page update requests down to the hypervisor. */
    if ( xc_finish_mmu_updates(xc_handle, mmu) )
        goto error_out;

    p = strstr(dsi.xen_guest_string, "HYPERCALL_PAGE=");
    if ( p != NULL )
    {
        p += strlen("HYPERCALL_PAGE=");
        hypercall_pfn = strtoul(p, NULL, 16);
        if ( hypercall_pfn >= nr_pages )
            goto error_out;
        op.u.hypercall_init.domain = (domid_t)dom;
        op.u.hypercall_init.mfn    = page_array[hypercall_pfn];
        op.cmd = DOM0_HYPERCALL_INIT;
        if ( xc_dom0_op(xc_handle, &op) )
            goto error_out;
    }

    free(mmu);
    free(page_array);

    *pvsi = vstartinfo_start;
    *pvss = vstack_start;
    *pvke = dsi.v_kernentry;

    return 0;

 error_out:
    free(mmu);
    free(page_array);
    return -1;
}
#endif
#endif

#if 0
static int xc_linux_build_internal(int xc_handle,
                                   uint32_t domid,
                                   char *image,
                                   unsigned long image_size,
                                   struct initrd_info *initrd,
                                   const char *cmdline,
                                   const char *features,
                                   unsigned long flags,
                                   unsigned int store_evtchn,
                                   unsigned long *store_mfn,
                                   unsigned int console_evtchn,
                                   unsigned long *console_mfn)
{
    dom0_op_t launch_op;
    DECLARE_DOM0_OP;
    int rc, i;
    vcpu_guest_context_t st_ctxt, *ctxt = &st_ctxt;
    unsigned long nr_pages;
    unsigned long vstartinfo_start, vkern_entry, vstack_start;
    uint32_t      features_bitmap[XENFEAT_NR_SUBMAPS] = { 0, };

    if ( features != NULL )
    {
        if ( !parse_features(features, features_bitmap, NULL) )
        {
            PERROR("Failed to parse configured features\n");
            goto error_out;
        }
    }

    if ( (nr_pages = get_tot_pages(xc_handle, domid)) < 0 )
    {
        PERROR("Could not find total pages for domain");
        goto error_out;
    }

#ifdef VALGRIND
    memset(&st_ctxt, 0, sizeof(st_ctxt));
#endif

    if ( mlock(&st_ctxt, sizeof(st_ctxt) ) )
    {   
        PERROR("%s: ctxt mlock failed", __func__);
        return 1;
    }

    op.cmd = DOM0_GETDOMAININFO;
    op.u.getdomaininfo.domain = (domid_t)domid;
    if ( (xc_dom0_op(xc_handle, &op) < 0) || 
         ((uint16_t)op.u.getdomaininfo.domain != domid) )
    {
        PERROR("Could not get info on domain");
        goto error_out;
    }

    memset(ctxt, 0, sizeof(*ctxt));

    if ( setup_guest(xc_handle, domid, image, image_size, 
                     initrd,
                     nr_pages, 
                     &vstartinfo_start, &vkern_entry,
                     &vstack_start, ctxt, cmdline,
                     op.u.getdomaininfo.shared_info_frame,
                     flags, store_evtchn, store_mfn,
                     console_evtchn, console_mfn,
                     features_bitmap) < 0 )
    {
        ERROR("Error constructing guest OS");
        goto error_out;
    }

#ifdef __ia64__
    /* based on new_thread in xen/arch/ia64/domain.c */
    ctxt->flags = 0;
    ctxt->shared.flags = flags;
    ctxt->shared.start_info_pfn = nr_pages - 3; /* metaphysical */
    ctxt->regs.cr_ipsr = 0; /* all necessary bits filled by hypervisor */
    ctxt->regs.cr_iip = vkern_entry;
    ctxt->regs.cr_ifs = 1UL << 63;
    ctxt->regs.ar_fpsr = xc_ia64_fpsr_default();
    /* currently done by hypervisor, should move here */
    /* ctxt->regs.r28 = dom_fw_setup(); */
    ctxt->vcpu.privregs = 0;
    ctxt->sys_pgnr = 3;
    i = 0; /* silence unused variable warning */
#elif __arm__
    /*to be implement -kcr */
i = 0; /* nothing special, prevent the warning from not used value- kcr */


#else /* x86 */
    /*
     * Initial register values:
     *  DS,ES,FS,GS = FLAT_KERNEL_DS
     *       CS:EIP = FLAT_KERNEL_CS:start_pc
     *       SS:ESP = FLAT_KERNEL_DS:start_stack
     *          ESI = start_info
     *  [EAX,EBX,ECX,EDX,EDI,EBP are zero]
     *       EFLAGS = IF | 2 (bit 1 is reserved and should always be 1)
     */
    ctxt->user_regs.ds = FLAT_KERNEL_DS;
    ctxt->user_regs.es = FLAT_KERNEL_DS;
    ctxt->user_regs.fs = FLAT_KERNEL_DS;
    ctxt->user_regs.gs = FLAT_KERNEL_DS;
    ctxt->user_regs.ss = FLAT_KERNEL_SS;
    ctxt->user_regs.cs = FLAT_KERNEL_CS;
    ctxt->user_regs.eip = vkern_entry;
    ctxt->user_regs.esp = vstack_start + PAGE_SIZE;
    ctxt->user_regs.esi = vstartinfo_start;
    ctxt->user_regs.eflags = 1 << 9; /* Interrupt Enable */

    ctxt->flags = VGCF_IN_KERNEL;

    /* FPU is set up to default initial state. */
    memset(&ctxt->fpu_ctxt, 0, sizeof(ctxt->fpu_ctxt));

    /* Virtual IDT is empty at start-of-day. */
    for ( i = 0; i < 256; i++ )
    {
        ctxt->trap_ctxt[i].vector = i;
        ctxt->trap_ctxt[i].cs     = FLAT_KERNEL_CS;
    }

    /* No LDT. */
    ctxt->ldt_ents = 0;
    
    /* Use the default Xen-provided GDT. */
    ctxt->gdt_ents = 0;

    /* Ring 1 stack is the initial stack. */
    ctxt->kernel_ss = FLAT_KERNEL_SS;
    ctxt->kernel_sp = vstack_start + PAGE_SIZE;

    /* No debugging. */
    memset(ctxt->debugreg, 0, sizeof(ctxt->debugreg));

    /* No callback handlers. */
#if defined(__i386__)
    ctxt->event_callback_cs     = FLAT_KERNEL_CS;
    ctxt->event_callback_eip    = 0;
    ctxt->failsafe_callback_cs  = FLAT_KERNEL_CS;
    ctxt->failsafe_callback_eip = 0;
#elif defined(__x86_64__)
    ctxt->event_callback_eip    = 0;
    ctxt->failsafe_callback_eip = 0;
    ctxt->syscall_callback_eip  = 0;
#endif
#endif /* x86 */

    memset( &launch_op, 0, sizeof(launch_op) );

    launch_op.u.setvcpucontext.domain = (domid_t)domid;
    launch_op.u.setvcpucontext.vcpu   = 0;
    launch_op.u.setvcpucontext.ctxt   = ctxt;

    launch_op.cmd = DOM0_SETVCPUCONTEXT;
    rc = xc_dom0_op(xc_handle, &launch_op);
    
    return rc;

 error_out:
    return -1;
}
#endif

#if 0
int xc_linux_build_mem(int xc_handle,
                       uint32_t domid,
                       const char *image_buffer,
                       unsigned long image_size,
                       const char *initrd,
                       unsigned long initrd_len,
                       const char *cmdline,
                       const char *features,
                       unsigned long flags,
                       unsigned int store_evtchn,
                       unsigned long *store_mfn,
                       unsigned int console_evtchn,
                       unsigned long *console_mfn)
{
    int            sts;
    char          *img_buf;
    unsigned long  img_len;
    struct initrd_info initrd_info = { .type = INITRD_none };

    /* A kernel buffer is required */
    if ( (image_buffer == NULL) || (image_size == 0) )
    {
        ERROR("kernel image buffer not present");
        return -1;
    }

    /* If it's gzipped, inflate it;  otherwise, use as is */
    /* xc_inflate_buffer may return the same buffer pointer if */
    /* the buffer is already inflated */
    img_buf = xc_inflate_buffer(image_buffer, image_size, &img_len);
    if ( img_buf == NULL )
    {
        ERROR("unable to inflate kernel image buffer");
        return -1;
    }

    /* RAM disks are optional; if we get one, inflate it */
    if ( initrd != NULL )
    {
        initrd_info.type = INITRD_mem;
        initrd_info.u.mem_addr = xc_inflate_buffer(
            initrd, initrd_len, &initrd_info.len);
        if ( initrd_info.u.mem_addr == NULL )
        {
            ERROR("unable to inflate ram disk buffer");
            sts = -1;
            goto out;
        }
    }

    sts = xc_linux_build_internal(xc_handle, domid, img_buf, img_len,
                                  &initrd_info, cmdline, features, flags,
                                  store_evtchn, store_mfn,
                                  console_evtchn, console_mfn);

 out:
    /* The inflation routines may pass back the same buffer so be */
    /* sure that we have a buffer and that it's not the one passed in. */
    /* Don't unnecessarily annoy/surprise/confound the caller */
    if ( (img_buf != NULL) && (img_buf != image_buffer) )
        free(img_buf);
    if ( (initrd_info.u.mem_addr != NULL) &&
         (initrd_info.u.mem_addr != initrd) )
        free(initrd_info.u.mem_addr);

    return sts;
}
#endif

int xc_linux_build(int xc_handle,
                   uint32_t domid,
                   const char *image_name,
                   const char *initrd_name,
                   const char *cmdline,
                   const char *features,
                   unsigned long flags,
                   unsigned int store_evtchn,
                   unsigned long *store_mfn,
                   //unsigned long *store_mfn)
                   unsigned int console_evtchn,
                   unsigned long *console_mfn)
{
#if 0
    char *image = NULL;
    unsigned long image_size;
    struct initrd_info initrd_info = { .type = INITRD_none };
    int fd = -1, sts = -1;

    if ( (image_name == NULL) ||
         ((image = xc_read_image(image_name, &image_size)) == NULL ))
        return -1;

    if ( (initrd_name != NULL) && (strlen(initrd_name) != 0) )
    {
        initrd_info.type = INITRD_file;

        if ( (fd = open(initrd_name, O_RDONLY)) < 0 )
        {
            PERROR("Could not open the initial ramdisk image");
            goto error_out;
        }

        initrd_info.len = xc_get_filesz(fd);
        if ( (initrd_info.u.file_handle = gzdopen(fd, "rb")) == NULL )
        {
            PERROR("Could not allocate decompression state for initrd");
            goto error_out;
        }
    }

    sts = xc_linux_build_internal(xc_handle, domid, image, image_size,
                                  &initrd_info, cmdline, features, flags,
                                  store_evtchn, store_mfn,
                                  console_evtchn, console_mfn);

 error_out:
    free(image);
    if ( fd >= 0 )
        close(fd);
    if ( initrd_info.u.file_handle )
        gzclose(initrd_info.u.file_handle);

    return sts;
#endif

    int fd_domU, ret;
    unsigned long guest_pa_address, guest_pa_size;
    unsigned long * va_guest_address=NULL;
    struct stat stat_buf;
    DECLARE_DOM0_OP;

	printf("xc_linux_build called\n");	
	
    op.cmd = DOM0_GUEST_IMAGE_CTL;
    op.u.guest_image_info.sub_cmd = CMD_GUEST_COPY;
    op.u.guest_image_info.domain = domid;

    if((ret = do_dom0_op(xc_handle, &op)) != 0){
		printf("xc_linux_build: CMD_GUEST_COPY failed\n");	
        return ret;
	}
	
    guest_pa_address = op.u.guest_image_info.guest_image_address;
    guest_pa_size    = op.u.guest_image_info.guest_image_size;

    /* mmap address page size has to be aligned w/ page */
    if(!guest_pa_address) {
        ret = -1;
		printf("xc_linux_build: guest_pa_address is NULL!!\n");	
        return ret;
    }

    if((va_guest_address = (unsigned long *) xc_map_foreign_range(xc_handle,
                                                                  domid,
                                                                  guest_pa_size,
                                                                  PROT_READ,
                                                                  guest_pa_address>>PAGE_SHIFT)) == NULL) {
        ret = -1;
		printf("xc_linux_build: xc_map_foreign_range failed\n");	
        return ret;
    }

    if((fd_domU = open(image_name, O_RDONLY)) < 0) {
        ret = -1;
		printf("xc_linux_build: image open failed\n");	
        return ret;
    }

    /* get the size of domU image */
    stat(image_name, &stat_buf);

    /* copy domU image from src to dst */
    if((ret = read(fd_domU, va_guest_address, stat_buf.st_size)) != stat_buf.st_size) {
        ret = -1;
		printf("xc_linux_build: image read failed\n");	
        return ret;
    }

    close(fd_domU);

    munmap(va_guest_address, guest_pa_size);

    /* passing guest image application virtual adderss to VMM */
    op.cmd = DOM0_GUEST_IMAGE_CTL;
    op.u.guest_image_info.sub_cmd = CMD_GUEST_CREATE;
    op.u.guest_image_info.guest_image_address = (unsigned long) va_guest_address;
    op.u.guest_image_info.guest_image_size = stat_buf.st_size;
    op.u.guest_image_info.store_port = store_evtchn;
	op.u.guest_image_info.console_port = console_evtchn; 
	
    /* create/construct/execute dom1 */
    if((ret = do_dom0_op(xc_handle, &op)) != 0){
		printf("xc_linux_build: CMD_GUEST_CREATE failed\n");	
        return ret;
	}

    *store_mfn = op.u.guest_image_info.store_mfn;
	*console_mfn = op.u.guest_image_info.console_mfn;

    return 0;
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
