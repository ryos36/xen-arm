/******************************************************************************
 * printf.c
 * 
 */

#include <stdarg.h>
#include <xen/config.h>
#include <xen/compile.h>
#include <xen/linkage.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <xen/serial.h>
#include <xen/foreground.h>
#include <asm/io.h>
#include <public/xen.h>
#include <xen/sched.h>
#include <xen/console.h>
#include <asm/current.h>

#ifndef va_arg

#ifndef _VALIST
#define _VALIST
typedef char *va_list;
#endif /* _VALIST */

/*
 * Storage alignment properties
 */
#define  _AUPBND                (sizeof (acpi_native_int) - 1)
#define  _ADNBND                (sizeof (acpi_native_int) - 1)

/*
 * Variable argument list macro definitions
 */ 
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (void) 0
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#endif /* va_arg */
//------------------------------------------------------------------------------------ 


extern int vsnprintf(char *, size_t, const char *, va_list);
extern char * strchr(const char *, int);
void __putstr(const char *str);


asmlinkage long do_console_io(int cmd, int count, char *buffer)
{
	long rc;
	
	switch ( cmd ) {
		case CONSOLEIO_write:
			if (current->domain->domain_id == foreground_domain){
				buffer[count] = '\0';
				__putstr(buffer);
				rc = 0;
			} else {
				buffer[count] = '\0';
				__putstr(buffer);
				rc = 0;
			}
			break;

		case CONSOLEIO_read:
			rc = 0;
			break;
		default:
			rc = -ENOSYS;
			break;
	}

	return rc;
}

static char printk_prefix[16] = "";
static int sercon_handle = -1;

spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

/*
 * *****************************************************
 * *************** GENERIC CONSOLE I/O *****************
 * *****************************************************
 */
extern void serial_puts(int, const char *);
void __putstr(const char *str)
{
    serial_puts(sercon_handle, str);
}

void printf(const char *fmt, ...)
{
    static char   buf[1024];
    static int    start_of_line = 1;

    va_list       args;
    char         *p, *q;
    unsigned long flags;

    spin_lock_irqsave(&console_lock, flags);

    va_start(args, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);        

    p = buf;
    while ( (q = strchr(p, '\n')) != NULL )
    {
        *q = '\0';
        if ( start_of_line )
            __putstr(printk_prefix);
        __putstr(p);
        __putstr("\n");
        start_of_line = 1;
        p = q + 1;
    }

    if ( *p != '\0' )
    {
        if ( start_of_line )
            __putstr(printk_prefix);
        __putstr(p);
        start_of_line = 0;
    }

    spin_unlock_irqrestore(&console_lock, flags);
}

extern char * strcpy(char *,const char *);
void set_printk_prefix(const char *prefix)
{
    strcpy(printk_prefix, prefix);
}

void init_console(void)
{
	sercon_handle = 0; // com 1

    set_printk_prefix("[XEN] ");
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

