#ifndef __PROP_H
#define __PROP_H

#define PROP_MAGIC_NUMBER		0xff4dc58a
#define PROP_STRING_SIZE 		32


enum prop_memusage_mode{
	HEXADECIMAL,
	PERCENTILE
};

typedef struct prop_memory_usage{
	unsigned int mode;	// Hex or Percentage
	unsigned int max_memory;
}prop_memory_usage_t;

typedef struct prop_cpu_usage{
	unsigned int max_usage;
	unsigned int interval;
}prop_cpu_usage_t;

enum prop_cpuschedule_mode{
	PRIORITY,
	CPU_TIME
};

typedef struct prop_battery_usage{
	unsigned int reschedule_threshold;
	unsigned int mode;   // Priority or CPU usage
	union{
		unsigned int priority;
		prop_cpu_usage_t cpu;
	}u;
	unsigned int shutdown_threshold;
}prop_battery_usage_t;

typedef struct prop_evtchn_usage{
	unsigned int max_per_dom;
	unsigned int max_per_dom_use;
}prop_evtchn_usage_t;

typedef struct prop_domain{
	prop_memory_usage_t memory_usage;
	prop_cpu_usage_t cpu_usage;
	prop_battery_usage_t battery_usage;
	prop_evtchn_usage_t evtchn_usage;
}prop_domain;

struct prop_policy{
	int nr_prop_domain;
	int vm_label_offset;
	int nr_vm_label;
	int vm_to_domain_size;
	int vm_label_size;
	int prop_policy_offset;
};

struct prop_domain_name{
	char name[PROP_STRING_SIZE];
	__u32 index;
};

struct prop_type_name{
	char name[PROP_STRING_SIZE];
	__u32 index;
};

struct VM_Label{
	__u32 nr_domains;
	__u32 domains_offset;
	domid_t vmid;
	char vm_name[PROP_STRING_SIZE];
};


#endif
