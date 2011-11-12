/*
 * io.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JooYoung Hwang <jooyoung.hwang@samsung.com>
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

#ifndef __ARM_IO_H__
#define __ARM_IO_H__

#define mmio_writeb(v,a)	(*(volatile unsigned char *)(a) = (v))
#define mmio_writew(v,a)	(*(volatile unsigned short *)(a) = (v))
#define mmio_writel(v,a)	(*(volatile unsigned int *)(a) = (v))

#define mmio_readb(a)		(*(volatile unsigned char *)(a))
#define mmio_readw(a)		(*(volatile unsigned short *)(a))
#define mmio_readl(a)		(*(volatile unsigned int *)(a))

#endif	/* __ARM_IO_H__ */
