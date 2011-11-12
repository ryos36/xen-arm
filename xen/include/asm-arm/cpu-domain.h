/*
 * cpu-domain.h
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

#ifndef __ARM_CPU_DOMAIN_H__
#define __ARM_CPU_DOMAIN_H__

#define DOMAIN_HYPERVISOR	15
#define DOMAIN_KERNEL		0
#define DOMAIN_SUPERVISOR	0
#define DOMAIN_IO		2
#define DOMAIN_USER		1
#define DOMAIN_METASET		3

/*
 * Domain types
 */
#define DOMAIN_NOACCESS		0
#define DOMAIN_CLIENT		1
#define DOMAIN_MANAGER		3

#define DOMAIN_VALUE(dom,type)	((type) << (2*(dom)))

#define DOMAIN_HYPERVISOR_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR,DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL, DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_IO, DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_USER,DOMAIN_CLIENT))

#define DOMAIN_SUPERVISOR_VALUE
#define DOMAIN_IO_VALUE

#define DOMAIN_KERNEL_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL, DOMAIN_MANAGER) |		\
	 DOMAIN_VALUE(DOMAIN_IO, DOMAIN_MANAGER) | 		\
	 DOMAIN_VALUE(DOMAIN_USER, DOMAIN_CLIENT))		\

#define DOMAIN_USER_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL,   DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_IO,       DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_USER,     DOMAIN_CLIENT))

#endif /* __ARM_CPU_DOMAIN_H__ */
