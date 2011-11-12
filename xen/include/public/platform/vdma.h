#ifndef __XEN_PUBLIC_VDMA_H__
#define __XEN_PUBLIC_VDMA_H__

#define MAX_VDMA_CHANNEL	32
#define DMAOP_ENABLE		0
#define DMAOP_DISABLE		1
#define DMAOP_START		2
#define DMAOP_ACK		3
#define DMAOP_REQUEST		4
#define DMAOP_RELEASE		5
#define DMAOP_CONFIGURE		6
#define DMAOP_BIND		7

#ifndef __ASSEMBLY__

typedef enum {
	VDMA_PRIORITY_HIGH   = 0,
	VDMA_PRIORITY_MEDIUM = 1,
	VDMA_PRIORITY_LOW    = 2
}vdma_priority;

typedef struct dma_op {
	unsigned int cmd;
	union {
		unsigned long	port;
		unsigned long	channel;
		vdma_priority	priority;
	}param;
}dma_op_t;

#endif
#endif

