
#ifndef __TE_COMPARISON_H
#define __TE_COMPARISON_H

unsigned long shift_request(unsigned long request, unsigned int object_group);
int te_evtchn_equal(void *x, void *y);
int te_evtchn_less(void *x, void *y);
int te_evtchn_greater(void *x, void *y);
int te_gnttab_equal(void *x, void *y);
int te_gnttab_less(void *x, void *y);
int te_gnttab_greater(void *x, void *y);
int te_device_equal(void *x, void *y);
int te_device_less(void *x, void *y);
int te_device_greater(void *x, void *y);
int te_smpres_equal(void *x, void *y);
int te_smpres_less(void *x, void *y);
int te_smpres_greater(void *x, void *y);

typedef struct _comparison_ops{
	int (* equal)(void *x, void *y);
	int (* greater)(void *x, void *y);
	int (* less)(void *x, void *y);
}ComparisonOP;

#endif
