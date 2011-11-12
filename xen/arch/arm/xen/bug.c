/*
 * bug.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh   <sbuk.suh@samsung.com>
 *          Jaemin Ryu     <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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

#include <xen/stdarg.h>
#include <xen/config.h>
#include <xen/version.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/event.h>
#include <xen/spinlock.h>
#include <xen/console.h>
#include <xen/serial.h>
#include <xen/softirq.h>
#include <xen/keyhandler.h>
#include <xen/mm.h>
#include <xen/delay.h>
#include <xen/guest_access.h>
#include <xen/shutdown.h>
#include <asm/current.h>
#include <asm/debugger.h>

void panic(const char *fmt, ...)
{
    va_list args;
    unsigned long flags;
    static char buf[128];
    
    va_start(args, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printk("\n****************************************\n");
    printk("Panic on CPU %d:\n", smp_processor_id());
    printk("%s", buf);
    printk("****************************************\n\n");

    while(1);

}

void __bug(char *file, int line)
{
    printk("Xen BUG at %s:%d\n", file, line);
    panic("Xen BUG at %s:%d\n", file, line);
    for ( ; ; ) ;
}

void __warn(char *file, int line)
{
    printk("Xen WARN at %s:%d\n", file, line);
}

