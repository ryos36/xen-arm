/******************************************************************************
 * sysctl.c
 * 
 * System management operations. For use by node control stack.
 * 
 * Copyright (c) 2002-2006, K Fraser
 */

#include <xen/types.h>
#include <xen/event.h>
#include <public/xen.h>
#include <public/sysctl.h>

long do_sysctl(GUEST_HANDLE(xen_sysctl_t) u_sysctl)
{
	while(1);

	return 0;
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
