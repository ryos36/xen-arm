#ifndef __XEN_VDMA_H__
#define __XEN_VDMA_H__

#include <asm/arch/config.h>

#define MAX_DMA_CHANNEL		ARCH_MAX_DMA_CHANNEL
#define DMA_CHANNEL_ALLOCATED	1	/* Allocated to guest domain */

typedef enum {
	DMA_PRIORITY_HIGH   = 0,
	DMA_PRIORITY_MEDIUM = 1,
	DMA_PRIORITY_LOW    = 2
}dma_priority;

struct dma_ops {
	int	(*request)(unsigned long channel);
	void	(*release)(unsigned long channel);
	void	(*enable)(unsigned long channel);
	void	(*disable)(unsigned long channel);
};

struct vcpu;

struct dma_channel {
	char		*name;
	unsigned long	state;
	struct vcpu	*owner;
	struct dma_ops	*ops;
};

extern struct dma_channel dma_channels[];
#endif

