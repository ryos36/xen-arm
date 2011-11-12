/******************************************************************************
 * os.h
 * 
 * random collection of macros and definition
 */

#ifndef __OS_H__
#define __OS_H__

#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

#include <os/types.h>
#include <os/traps.h>
#include <os/evtchn.h>
#include <os/hypervisor.h>
#include <os/errno-base.h>

#include <xen/interface/types.h>
#include <xen/interface/xen.h>
#include <xen/interface/dom0_ops.h>
#include <xen/interface/sched.h>

#define smp_processor_id() 0

extern start_info_t 	start_info;
extern shared_info_t 	*HYPERVISOR_shared_info;

#endif /* __OS_H__ */
