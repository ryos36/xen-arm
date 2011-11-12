#include <os/os.h>
#include <os/hypervisor.h>
#include <os/time.h>
#include <os/mm.h>
#include <os/types.h>
#include <os/lib.h>
#include <os/xmalloc.h>
#include <os/list.h>
#include <os/irq.h>
#include <os/sched.h>

#define RUNNABLE_FLAG   0x00000001

#define is_runnable(_thread)    (_thread->flags & RUNNABLE_FLAG)
#define set_runnable(_thread)   (_thread->flags |= RUNNABLE_FLAG)
#define clear_runnable(_thread) (_thread->flags &= ~RUNNABLE_FLAG)

struct thread *idle_thread;
LIST_HEAD(exited_threads);

void inline print_runqueue(void)
{
    struct list_head *it;
    struct thread *th;
    list_for_each(it, &idle_thread->thread_list)
    {
        th = list_entry(it, struct thread, thread_list);
        printk("   Thread \"%s\", runnable=%d\n", th->name, is_runnable(th));
    }
    printk("\n");
}

void schedule(void)
{
    struct thread *prev, *next, *thread;
    struct list_head *iterator;
    unsigned long flags;
    prev = current;
    
    //__cli();
    local_irq_save(flags); 
    list_for_each(iterator, &exited_threads)
    {
        thread = list_entry(iterator, struct thread, thread_list);
        if(thread != prev)
        {
            list_del(&thread->thread_list);
            free_pages(thread->stack, 1);
            xfree(thread);
            printk("Thread Desctroyed\n");
        }
    }
    next = idle_thread;    
    /* Thread list needs to be protected */
    list_for_each(iterator, &idle_thread->thread_list)
    {
        thread = list_entry(iterator, struct thread, thread_list);
        if(is_runnable(thread)) 
        {
            next = thread;
            /* Put this thread on the end of the list */
            list_del(&thread->thread_list);
            list_add_tail(&thread->thread_list, &idle_thread->thread_list);
            break;
        }
    }
    //__sti();
    
    local_irq_restore(flags);
    /* Interrupting the switch is equivalent to having the next thread
       inturrupted at the return instruction. And therefore at safe point. */
/* The thread switching only works for i386 at the moment */    
	
    if(prev != next) {
	    switch_threads(prev, next);
    }
}


void exit_thread(struct thread *thread)
{
    unsigned long flags;
    printk("[1] Thread \"%s\" exited.\n", thread->name);
    local_irq_save(flags);
    /* Remove from the thread list */
    list_del(&thread->thread_list);
    clear_runnable(thread);
    /* Put onto exited list */
    list_add(&thread->thread_list, &exited_threads);
    local_irq_restore(flags);
    /* Schedule will free the resources */
    schedule();
}

struct thread* create_thread(char *name, void (*function)(void *), void *data)
{
    struct thread *thread;
    unsigned long flags;
    
    thread = xmalloc(struct thread);
    /* Allocate 2 pages for stack, stack will be 2pages aligned */
    thread->stack = (char *)alloc_pages(1);
    thread->name = name;
    printk("[1] Thread \"%s\": pointer: 0x%lx, stack: 0x%lx\n", name, thread, 
            thread->stack);
    
    thread->eps = (unsigned long)thread->stack + 4096 * 2 - 4;
    /* Save pointer to the thread on the stack, used by current macro */
    *((unsigned long *)thread->stack) = (unsigned long)thread;
    *((unsigned long *)thread->eps) = (unsigned long)thread;
    thread->eps -= 4; 
    *((unsigned long *)thread->eps) = (unsigned long)data;
    
    /* No return address */
    thread->eps -= 4;
    *((unsigned long *)thread->eps) = (unsigned long)exit_thread;
    
    thread->eip = (unsigned long)function;
     
    /* Not runable, not exited */ 
    thread->flags = 0;
    set_runnable(thread);
    
    local_irq_save(flags);
    if(idle_thread != NULL)
        list_add_tail(&thread->thread_list, &idle_thread->thread_list); 
    local_irq_restore(flags);

    return thread;
}

void idle_thread_fn(void *unused)
{
	int i=0;
	
	while(1) {
		for(i=0;i<10000000;i++);
		schedule();
	}
}

void th_f3(void *unused)
{
	int i=0;
	
	while(1) {
		printk("[1] 3 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("[1] After 3 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f4(void *unused)
{
	int i=0;
	
	while(1) {
		printk("[1] 4 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("[1] After 4 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f5(void *unused)
{
	int i=0;
	
	while(1) {
		printk("[1] 5 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("[1] After 5 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f6(void *unused)
{
	int i=0;
	
	while(1) {
		printk("[1] 6 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("[1] After 6 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

/*
void th_f7(void *unused)
{
	int i=0;
	
	while(1) {
		printk("7 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 7 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f8(void *unused)
{
	int i=0;
	
	while(1) {
		printk("8 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 8 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f9(void *unused)
{
	int i=0;
	
	while(1) {
		printk("9 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 9 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f10(void *unused)
{
	int i=0;
	
	while(1) {
		printk("10 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 10 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f11(void *unused)
{
	int i=0;
	
	while(1) {
		printk("11 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 11 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f12(void *unused)
{
	int i=0;
	
	while(1) {
		printk("12 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 12 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f13(void *unused)
{
	int i=0;
	
	while(1) {
		printk("13 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 13 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

void th_f14(void *unused)
{
	int i=0;
	
	while(1) {
		printk("14 Thread\n"); 
		for(i=0;i<10000000;i++);
		schedule();
		printk("After 14 Thread\n");
		for(i=0;i<10000000;i++);
	}
}

*/

void run_idle_thread(void)
{							
	__asm__ __volatile__(	"ldr	sp, [%0]\t\n"	\
				"ldr	pc, [%1]\t\n" 	\
				:\
				: "r"(&idle_thread->eps), "r"(&idle_thread->eip) \
				: "memory");
}

void sched_init(void)
{
    printk("Initialising scheduler\n");
       
    idle_thread = create_thread("Idle", idle_thread_fn, NULL);
    INIT_LIST_HEAD(&idle_thread->thread_list);

    create_thread("3", th_f3, (void *)0x1234);    
    create_thread("4", th_f4, NULL);
    create_thread("5", th_f5, (void *)0x1234);    
    create_thread("6", th_f6, NULL);
/*
    create_thread("7", th_f7, (void *)0x1234);    
    create_thread("8", th_f8, NULL);
    create_thread("9", th_f9, (void *)0x1234);    
    create_thread("10", th_f10, NULL);
    create_thread("11", th_f11, (void *)0x1234);    
    create_thread("12", th_f12, NULL);
    create_thread("13", th_f13, (void *)0x1234);    
    create_thread("14", th_f14, NULL);
*/
}
