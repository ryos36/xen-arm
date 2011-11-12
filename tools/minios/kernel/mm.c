

#include <os/os.h>
#include <os/hypervisor.h>
#include <os/mm.h>
#include <os/types.h>
#include <os/lib.h>

#ifdef MM_DEBUG
#define DEBUG(_f, _a...) \
    printk("MINI_OS(file=mm.c, line=%d) " _f "\n", __LINE__, ## _a)
#else
#define DEBUG(_f, _a...)    ((void)0)
#endif

unsigned long *phys_to_machine_mapping;
extern char *stack;
extern void page_walk(unsigned long virt_addr);
extern unsigned long _text, _etext, _edata;

extern int gnttab_init(void);


/*********************
 * ALLOCATION BITMAP
 *  One bit per page of memory. Bit set => page is allocated.
 */

static unsigned long *alloc_bitmap;
#define PAGES_PER_MAPWORD (sizeof(unsigned long) * 8)

#define allocated_in_map(_pn) \
(alloc_bitmap[(_pn)/PAGES_PER_MAPWORD] & (1<<((_pn)&(PAGES_PER_MAPWORD-1))))

/*
 * Hint regarding bitwise arithmetic in map_{alloc,free}:
 *  -(1<<n)  sets all bits >= n. 
 *  (1<<n)-1 sets all bits <  n.
 * Variable names in map_{alloc,free}:
 *  *_idx == Index into `alloc_bitmap' array.
 *  *_off == Bit offset within an element of the `alloc_bitmap' array.
 */

static void map_alloc(unsigned long first_page, unsigned long nr_pages)
{
    unsigned long start_off, end_off, curr_idx, end_idx;

    curr_idx  = first_page / PAGES_PER_MAPWORD;
    start_off = first_page & (PAGES_PER_MAPWORD-1);
    end_idx   = (first_page + nr_pages) / PAGES_PER_MAPWORD;
    end_off   = (first_page + nr_pages) & (PAGES_PER_MAPWORD-1);

    if ( curr_idx == end_idx )
    {
        alloc_bitmap[curr_idx] |= ((1<<end_off)-1) & -(1<<start_off);
    }
    else 
    {
        alloc_bitmap[curr_idx] |= -(1<<start_off);
        while ( ++curr_idx < end_idx ) alloc_bitmap[curr_idx] = ~0L;
        alloc_bitmap[curr_idx] |= (1<<end_off)-1;
    }
}


static void map_free(unsigned long first_page, unsigned long nr_pages)
{
    unsigned long start_off, end_off, curr_idx, end_idx;

    curr_idx = first_page / PAGES_PER_MAPWORD;
    start_off = first_page & (PAGES_PER_MAPWORD-1);
    end_idx   = (first_page + nr_pages) / PAGES_PER_MAPWORD;
    end_off   = (first_page + nr_pages) & (PAGES_PER_MAPWORD-1);

    if ( curr_idx == end_idx )
    {
        alloc_bitmap[curr_idx] &= -(1<<end_off) | ((1<<start_off)-1);
    }
    else 
    {
        alloc_bitmap[curr_idx] &= (1<<start_off)-1;
        while ( ++curr_idx != end_idx ) alloc_bitmap[curr_idx] = 0;
        alloc_bitmap[curr_idx] &= -(1<<end_off);
    }
}



/*************************
 * BINARY BUDDY ALLOCATOR
 */

typedef struct chunk_head_st chunk_head_t;
typedef struct chunk_tail_st chunk_tail_t;

struct chunk_head_st {
    chunk_head_t  *next;
    chunk_head_t **pprev;
    int            level;
};

struct chunk_tail_st {
    int level;
};

/* Linked lists of free chunks of different powers-of-two in size. */
#define FREELIST_SIZE ((sizeof(void*)<<3)-PAGE_SHIFT)
static chunk_head_t *free_head[FREELIST_SIZE];
static chunk_head_t  free_tail[FREELIST_SIZE];
#define FREELIST_EMPTY(_l) ((_l)->next == NULL)

#define round_pgdown(_p)  ((_p)&PAGE_MASK)
#define round_pgup(_p)    (((_p)+(PAGE_SIZE-1))&PAGE_MASK)

#ifdef MM_DEBUG
/*
 * Prints allocation[0/1] for @nr_pages, starting at @start
 * address (virtual).
 */
static void print_allocation(void *start, int nr_pages)
{
    unsigned long pfn_start = virt_to_pfn(start);
    int count;
    for(count = 0; count < nr_pages; count++)
        if(allocated_in_map(pfn_start + count)) printk("1");
        else printk("0");
        
    printk("\n");        
}

/*
 * Prints chunks (making them with letters) for @nr_pages starting
 * at @start (virtual).
 */
static void print_chunks(void *start, int nr_pages)
{
    char chunks[1001], current='A';
    int order, count;
    chunk_head_t *head;
    unsigned long pfn_start = virt_to_pfn(start);
   
    memset(chunks, (int)'_', 1000);
    if(nr_pages > 1000) 
    {
        DEBUG("Can only pring 1000 pages. Increase buffer size.");
    }
    
    for(order=0; order < FREELIST_SIZE; order++)
    {
        head = free_head[order];
        while(!FREELIST_EMPTY(head))
        {
            for(count = 0; count < 1<< head->level; count++)
            {
                if(count + virt_to_pfn(head) - pfn_start < 1000)
                    chunks[count + virt_to_pfn(head) - pfn_start] = current;
            }
            head = head->next;
            current++;
        }
    }
    chunks[nr_pages] = '\0';
    printk("%s\n", chunks);
}
#endif


/*
 * Initialise allocator, placing addresses [@min,@max] in free pool.
 * @min and @max are PHYSICAL addresses.
 */
static void init_page_allocator(unsigned long min, unsigned long max)
{
    int i;
    unsigned long range, bitmap_size;
    chunk_head_t *ch;
    chunk_tail_t *ct;

	printf("[init_page_allocator] start: min=%d, max=%d\n", min, max);

    for ( i = 0; i < FREELIST_SIZE; i++ )
    {
        free_head[i]       = &free_tail[i];
        free_tail[i].pprev = &free_head[i];
        free_tail[i].next  = NULL;
    }

    min = round_pgup  (min);
    max = round_pgdown(max);

    /* Allocate space for the allocation bitmap. */
    bitmap_size  = (max+1) >> (PAGE_SHIFT+3);
    bitmap_size  = round_pgup(bitmap_size);
    alloc_bitmap = (unsigned long *)to_virt(min);
    min         += bitmap_size;
    range        = max - min;

    /* All allocated by default. */
    memset(alloc_bitmap, ~0, bitmap_size);
    /* Free up the memory we've been given to play with. */
    map_free(min>>PAGE_SHIFT, range>>PAGE_SHIFT);

	printf("[init_page_allocator] The buddy lists are addressed in high memory\n");

    /* The buddy lists are addressed in high memory. */
    min += VIRT_START;
    max += VIRT_START;

    while ( range != 0 )
    {
        /*
         * Next chunk is limited by alignment of min, but also
         * must not be bigger than remaining range.
         */
        for ( i = PAGE_SHIFT; (1<<(i+1)) <= range; i++ )
            if ( min & (1<<i) ) break;
	
        ch = (chunk_head_t *)min;
        min   += (1<<i);
        range -= (1<<i);
        ct = (chunk_tail_t *)min-1;

		printf("[init_page_allocator] ch=0x%lx, ct=0x%lx\n", (unsigned long) ch, (unsigned long) ct);

        i -= PAGE_SHIFT;
        ch->level       = i;
        ch->next        = free_head[i];
        ch->pprev       = &free_head[i];
        ch->next->pprev = &ch->next;
        free_head[i]    = ch;
        ct->level       = i;
    }
	printf("[init_page_allocator] end.\n");

}

/* Allocate 2^@order contiguous pages. Returns a VIRTUAL address. */
unsigned long alloc_pages(int order)
{
    int i;
    chunk_head_t *alloc_ch, *spare_ch;
    chunk_tail_t            *spare_ct;


    /* Find smallest order which can satisfy the request. */
    for ( i = order; i < FREELIST_SIZE; i++ ) {
	if ( !FREELIST_EMPTY(free_head[i]) ) 
	    break;
    }

    if ( i == FREELIST_SIZE ) goto no_memory;
 
    /* Unlink a chunk. */
    alloc_ch = free_head[i];
    free_head[i] = alloc_ch->next;
    alloc_ch->next->pprev = alloc_ch->pprev;

    /* We may have to break the chunk a number of times. */
    while ( i != order )
    {
        /* Split into two equal parts. */
        i--;
        spare_ch = (chunk_head_t *)((char *)alloc_ch + (1<<(i+PAGE_SHIFT)));
        spare_ct = (chunk_tail_t *)((char *)spare_ch + (1<<(i+PAGE_SHIFT)))-1;

        /* Create new header for spare chunk. */
        spare_ch->level = i;
        spare_ch->next  = free_head[i];
        spare_ch->pprev = &free_head[i];
        spare_ct->level = i;

        /* Link in the spare chunk. */
        spare_ch->next->pprev = &spare_ch->next;
        free_head[i] = spare_ch;
    }
    
    map_alloc(to_phys(alloc_ch)>>PAGE_SHIFT, 1<<order);


    return((unsigned long)alloc_ch);

 no_memory:

    printk("Cannot handle page request order %d!\n", order);

    return 0;
}

void free_pages(void *pointer, int order)
{
    chunk_head_t *freed_ch, *to_merge_ch;
    chunk_tail_t *freed_ct;
    unsigned long mask;
	
    
    /* First free the chunk */
    map_free(virt_to_pfn(pointer), 1 << order);
    
    /* Create free chunk */
    freed_ch = (chunk_head_t *)pointer;
    freed_ct = (chunk_tail_t *)((char *)pointer + (1<<(order + PAGE_SHIFT)))-1;
    
    /* Now, possibly we can conseal chunks together */
    while(order < FREELIST_SIZE)
    {
        mask = 1 << (order + PAGE_SHIFT);
        if((unsigned long)freed_ch & mask) 
        {
            to_merge_ch = (chunk_head_t *)((char *)freed_ch - mask);
            if(allocated_in_map(virt_to_pfn(to_merge_ch)) ||
                    to_merge_ch->level != order)
                break;
            
            /* Merge with predecessor */
            freed_ch = to_merge_ch;   
        }
        else 
        {
            to_merge_ch = (chunk_head_t *)((char *)freed_ch + mask);
            if(allocated_in_map(virt_to_pfn(to_merge_ch)) ||
                    to_merge_ch->level != order)
                break;
            
            /* Merge with successor */
            freed_ct = (chunk_tail_t *)((char *)to_merge_ch + mask);
        }
        
        /* We are commited to merging, unlink the chunk */
        *(to_merge_ch->pprev) = to_merge_ch->next;
        to_merge_ch->next->pprev = to_merge_ch->pprev;
        
        order++;
    }

    /* Link the new chunk */
    freed_ch->level = order;
    freed_ch->next  = free_head[order];
    freed_ch->pprev = &free_head[order];
    freed_ct->level = order;
    
    freed_ch->next->pprev = &freed_ch->next;
    free_head[order] = freed_ch;   
   
}


void new_pt_frame(unsigned long *pt_pfn, unsigned long prev_l_mfn, 
                                unsigned long offset, unsigned long level)
{   
    unsigned long *tab = (unsigned long *)start_info.pt_base;
    unsigned long pt_page = (unsigned long)pfn_to_virt(*pt_pfn); 
    unsigned long prot_e=0, prot_t=0, pincmd=0;
    mmu_update_t mmu_updates[1];
    struct mmuext_op pin_request;
	
    //printf("Allocating new L%d pt frame for pt_pfn=%lx, "
    //       "prev_l_mfn=%lx, offset=%lx\n", 
	//		 level, *pt_pfn, prev_l_mfn, offset);

    /* We need to clear the page, otherwise we might fail to map it
       as a page table page */
    memset((unsigned long*)pfn_to_virt(*pt_pfn), 0, PAGE_SIZE);  
 
    if (level == L1_FRAME)
    {
		prot_e = __L1_PAGE_USER_SMALL_RO;
		prot_t = __L2_PAGE_USER_TABLE;
		pincmd = MMUEXT_PIN_L1_TABLE;
    }

    else
    {
         printk("new_pt_frame() called with invalid level number %d\n", level);
	*((unsigned long *)0)=0;
    }    

    /* Update the entry */
	//printf("update the page table entry\n");
    mmu_updates[0].ptr = (tab[l2_table_offset(pt_page)] & PAGE_MASK) + 
                         sizeof(void *)* l1_table_offset(pt_page);
    mmu_updates[0].val = (pfn_to_mfn(*pt_pfn) << PAGE_SHIFT) | prot_e;

    if(HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF) < 0)
    {
         printk("PTE for new page table page could not be updated\n");
         *((unsigned long *)0) = 0;
    }
                        
    /* Pin the page to provide correct protection */
	//printf("pin the page\n");
    pin_request.cmd = pincmd;
    pin_request.arg1.mfn = pfn_to_mfn(*pt_pfn);

// before this hypercall, new page table page should set with correct page type
//    if(HYPERVISOR_mmuext_op(&pin_request, 1, NULL, DOMID_SELF) < 0)
//    {
//        printk("ERROR: pinning failed\n");
//        *((unsigned long *)0) = 0;
//    }

    /* Now fill the new page table page with entries.
       Update the page directory as well. */
	//printf("fill the new page table page\n");
    mmu_updates[0].ptr = (prev_l_mfn << PAGE_SHIFT) + sizeof(void *) * offset;
    mmu_updates[0].val = pfn_to_mfn(*pt_pfn) << PAGE_SHIFT | prot_t;
    if(HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF) < 0) 
    {            
       printk("ERROR: mmu_update failed\n");
       *((unsigned long *)0) = 0;
    }

    *pt_pfn += 1;
}

int  update_pagetable(unsigned long virtaddr, unsigned long physaddr, int count)
{
    mmu_update_t mmu_updates[100];
    unsigned long *l2tab, *l1tab;
    unsigned long l1tab_mfn;
    unsigned long l2pte, offset;
	unsigned long vaddr, paddr;
	int			  i;

	//printf("[update_pagetable] virtaddr=0x%lx, physaddr=0x%lx, count=%d\n",
	//	   virtaddr, physaddr, count);
	
	if( count > 100 )
		return -1;

	l2tab = (unsigned long *)start_info.pt_base;

	for( i = 0; i < count; i++ )
	{
		vaddr = virtaddr + i*PAGE_SIZE;
		paddr = physaddr + i*PAGE_SIZE;

		offset = l2_table_offset(vaddr);        
		l2pte = l2tab[offset];
		l1tab_mfn = pte_to_mfn(l2pte);

		/* Need new L1 pt frame */
		if( l1tab_mfn == 0 ) {

			//printf("[update_pagetable] Need new L1 pt frame\n");

			l1tab_mfn = pfn_to_mfn(virt_to_pfn(alloc_pages(0)));

			//printf("[update_pagetable] l1tab_mfn = %ld\n", l1tab_mfn);

			memset((unsigned long*)mfn_to_virt(l1tab_mfn), 0, PAGE_SIZE);  
			
			mmu_updates[0].ptr = (pfn_to_mfn(virt_to_pfn((unsigned long) l2tab)) << PAGE_SHIFT) + sizeof(void *) * offset;
			mmu_updates[0].val = (l1tab_mfn << PAGE_SHIFT) | __L2_PAGE_USER_TABLE;
			
			if(HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF) < 0)
			{
				printk("PTE for new page table page could not be updated\n");
				*((unsigned long *)0) = 0;
			}
			//printf("[update_pagetable] L1 pt configured\n");

		}
		
		l1tab = (unsigned long *) (l1tab_mfn << PAGE_SHIFT);
		offset = l1_table_offset(vaddr);
		
		mmu_updates[i].ptr = (unsigned long) &l1tab[offset];
		mmu_updates[i].val = paddr | __L1_PAGE_USER_SMALL_RO;
	}

	if(HYPERVISOR_mmu_update(mmu_updates, count, NULL, DOMID_SELF) < 0)
	{
		printk("PTE could not be updated\n");
		*((unsigned long *)0) = 0;
	}
	//printf("[update_pagetable] page table updated\n");
		
	return 1;
}

#if 0
void build_pagetable(unsigned long *start_pfn, unsigned long *max_pfn)
{
    unsigned long start_address, end_address;
    unsigned long pfn_to_map, pt_pfn = *start_pfn;
    static mmu_update_t mmu_updates[L1_PAGETABLE_ENTRIES + 1];
    unsigned long *tab = (unsigned long *)start_info.pt_base;
    unsigned long mfn = pfn_to_mfn(virt_to_pfn(start_info.pt_base));
    unsigned long page, offset;
    int count = 0;


    //pfn_to_map = (start_info.nr_pt_frames - 1) * L1_PAGETABLE_ENTRIES;
	pfn_to_map = 0x100;

    start_address = (unsigned long)pfn_to_virt(pfn_to_map);
    end_address = (unsigned long)pfn_to_virt(*max_pfn);

	//printf("[MM_INIT] pfn_to_map=0x%lx, start_address=0x%lx, end_address=0x%lx\n",
	//	   pfn_to_map, start_address, end_address);
    

    while(start_address < end_address)
    {
        tab = (unsigned long *)start_info.pt_base;
        mfn = pfn_to_mfn(virt_to_pfn(start_info.pt_base));


        offset = l2_table_offset(start_address);        
        /* Need new L1 pt frame */
        if(!(start_address & L1_MASK)) 
            new_pt_frame(&pt_pfn, mfn, offset, L1_FRAME);
       
		//printf("PTE update for start_address: pfn=0x%lx, mfn=0x%lx", 
		//	   pfn_to_map, pfn_to_mfn(pfn_to_map));

        page = tab[offset];
        mfn = pte_to_mfn(page);
        offset = l1_table_offset(start_address);

        mmu_updates[count].ptr = (mfn << PAGE_SHIFT) + sizeof(void *) * offset;
        mmu_updates[count].val = 
            (pfn_to_mfn(pfn_to_map++) << PAGE_SHIFT) | __L1_PAGE_USER_SMALL;
        count++;
        if (count == L1_PAGETABLE_ENTRIES || pfn_to_map == *max_pfn)
        {
            if(HYPERVISOR_mmu_update(mmu_updates, count, NULL, DOMID_SELF) < 0)
            {
                printk("PTE could not be updated\n");
       		*((unsigned long *)0) = 0;
            }
            count = 0;
        }
        start_address += PAGE_SIZE;
    }

    *start_pfn = pt_pfn;
}
#endif

/*
void mem_test(unsigned long *start_add, unsigned long *end_add)
{
    unsigned long mask = 0x10000;
    unsigned long *pointer;

    for(pointer = start_add; pointer < end_add; pointer++)
    {
        if(!(((unsigned long)pointer) & 0xfffff))
        {
            printk("Writing to %lx\n", pointer);
            page_walk((unsigned long)pointer);
        }
        *pointer = (unsigned long)pointer & ~mask;
    }

    for(pointer = start_add; pointer < end_add; pointer++)
    {
        if(((unsigned long)pointer & ~mask) != *pointer)
            printk("Read error at 0x%lx. Read: 0x%lx, should read 0x%lx\n",
                (unsigned long)pointer, 
                *pointer, 
                ((unsigned long)pointer & ~mask));
    }

}
*/

void mm_init(void)
{

	unsigned long start_pfn, max_pfn;

	printk("MM: Init\n");

	/* set up minimal memory infos */
	phys_to_machine_mapping = (unsigned long *)start_info.mfn_list;
   
	/* First page follows page table pages and 3 more pages (store page etc) */
	start_pfn = PFN_UP(to_phys(start_info.pt_base)) + start_info.nr_pt_frames + 3;
	max_pfn = start_info.nr_pages;
   
	min_mfn = start_info.min_mfn;


	/*
	 * now we can initialise the page allocator
	 */
	printk("MM: Initializing page allocator for memory %lx(%lx)-%lx(%lx)\n",
		(u_long)to_virt(PFN_PHYS(start_pfn)), PFN_PHYS(start_pfn), 
		(u_long)to_virt(PFN_PHYS(max_pfn)), PFN_PHYS(max_pfn));

	printk("MM: initializing page allocator\n");
	
	init_page_allocator(PFN_PHYS(start_pfn), PFN_PHYS(max_pfn));

	printk("MM: initializing grant table\n");

	gnttab_init();

	printk("MM: done\n");

}
