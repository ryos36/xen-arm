#ifndef __SCHED_H__
#define __SCHED_H__

#include <os/list.h>

struct thread
{
    char				*name;
    char				*stack;
    unsigned long		eps;
    unsigned long		eip;
    struct list_head 	thread_list;
    u32 flags;
};

#define switch_threads(prev, next)							\
do{															\
	__asm__ __volatile__("stmdb	r13!, {r0 - r14}\t\n"		\
						 "str	r13,[%0]\t\n"				\
						 "adr	r14, 1f\t\n"				\
						 "str	r14, [%1]\t\n"				\
						 "ldr	sp, [%2]\t\n"				\
						 "ldr	pc, [%3]\t\n"				\
						 "1:\t\n"							\
						 "ldmia	sp!, {r0 - r14}\t\n"		\
						 :									\
						 : "r"(&prev->eps), "r"(&prev->eip),\
						   "r"(&next->eps), "r"(&next->eip)	\
						 : "memory");						\
}while(0)
						 
static inline struct thread* get_current(void)
{
    struct thread **current;
    
    __asm__("and %0, %%r13, %1" : "=%r" (current) : "r" (~8191UL));

    return *current;
}
          
#define current get_current()

void sched_init(void);
void run_idle_thread(void);
struct thread* create_thread(char *name, void (*function)(void *), void *data);
void schedule(void);
void wake(struct thread *thread);
void block(struct thread *thread);

#endif /* __SCHED_H__ */
