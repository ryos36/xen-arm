#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <asm/types.h>

#define MAX_LOG_ENTRIES				8192

#define LOG_ORIGIN_RESET_ENTRY		0
#define LOG_ORIGIN_RESET_EXIT		1
#define LOG_ORIGIN_UND_ENTRY		2
#define LOG_ORIGIN_DABT_ENTRY		3
#define LOG_ORIGIN_DABT_EXIT		4
#define LOG_ORIGIN_PABT_ENTRY		5
#define LOG_ORIGIN_PABT_EXIT		6
#define LOG_ORIGIN_IRQ_ENTRY		7
#define LOG_ORIGIN_IRQ_EXIT			8
#define LOG_ORIGIN_FIQ_ENTRY		9
#define LOG_ORIGIN_FIQ_EXIT			10
#define LOG_ORIGIN_SYSCALL_ENTRY	11
#define LOG_ORIGIN_HYPERCALL_ENTRY	12
#define LOG_ORIGIN_HYPERCALL_EXIT	13
#define LOG_ORIGIN_UPCALL_ENTRY		14
#define LOG_ORIGIN_UPCALL_EXIT		15
#define LOG_ORIGIN_VMM_EXIT			16
#define LOG_ORIGIN_SCHEDULER_ENTRY	17
#define LOG_ORIGIN_SCHEDULER_EXIT	18
#define LOG_ORIGIN_SOFTIRQ_ENTRY	19
#define LOG_ORIGIN_SOFTIRQ_EXIT		20

#ifndef __ASSEMBLY__
struct log_entry {
	u32		timestamp;
	u32		origin;
	u32		event;
	u32		dummy;
};

extern u32 idx;
extern struct log_entry profile_data[MAX_LOG_ENTRIES];
void event_logging(u32 origin, u32 event);

#endif

#endif

