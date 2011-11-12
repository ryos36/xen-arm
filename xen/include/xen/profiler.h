#ifndef __XEN_PROFILER_H__
#define __XEN_PROFILER_H__

struct profile_entry {
	unsigned long timestamp;
	unsigned long event;
};

void record_timestamp();

#endif

