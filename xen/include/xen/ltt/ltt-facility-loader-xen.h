#ifndef _LTT_FACILITY_LOADER_XEN_H_
#define _LTT_FACILITY_LOADER_XEN_H_

#ifdef CONFIG_LTT

#include <public/ltt-facilities.h>
#include <ltt/ltt-facility-id-xen.h>

ltt_facility_t	ltt_facility_xen;
ltt_facility_t	ltt_facility_xen_C454D23D;

#define LTT_FACILITY_SYMBOL		ltt_facility_xen
#define LTT_FACILITY_CHECKSUM_SYMBOL	ltt_facility_xen_C454D23D
#define LTT_FACILITY_CHECKSUM		0xC454D23D
#define LTT_FACILITY_NAME		"xen"
#define LTT_FACILITY_NUM_EVENTS	facility_xen_num_events

#endif //CONFIG_LTT

#endif //_LTT_FACILITY_LOADER_XEN_H_
