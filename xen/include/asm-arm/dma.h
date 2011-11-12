/*
 * dma.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          Jaemin Ryu <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ARM_DMA_H__
#define __ARM_DMA_H__

typedef struct dma_channel_info {
	domid_t	owner;
	int	in_use;
}dma_channel_info_t;

extern dma_channel_info_t dma_channel_map[];

#endif /* __ARM_DMA_H__ */
