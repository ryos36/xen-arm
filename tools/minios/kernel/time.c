/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*-
 ****************************************************************************
 * (C) 2003 - Rolf Neugebauer - Intel Research Cambridge
 * (C) 2002-2003 - Keir Fraser - University of Cambridge 
 * (C) 2005 - Grzegorz Milos - Intel Research Cambridge
 ****************************************************************************
 *
 *        File: time.c
 *      Author: Rolf Neugebauer and Keir Fraser
 *     Changes: Grzegorz Milos
 *
 * Description: Simple time and timer functions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */


#include <os/os.h>
#include <os/traps.h>
#include <os/types.h>
#include <os/hypervisor.h>
#include <os/evtchn.h>
#include <os/time.h>
#include <os/lib.h>
#include <os/irq.h>
#include <os/sched.h>

#if 0

/************************************************************************
 * Time functions
 *************************************************************************/

/* These are peridically updated in shared_info, and then copied here. */

struct shadow_time_info {
	u64 tsc_timestamp;     // TSC at last update of time vals.
	u64 system_timestamp;  // Time, in nanosecs, since boot.
	u32 tsc_to_nsec_mul;
	u32 tsc_to_usec_mul;
	int tsc_shift;
	u32 version;
};

static struct timespec shadow_ts;
static u32 shadow_ts_version;

static struct shadow_time_info shadow;

#define HANDLE_USEC_OVERFLOW(_tv)          \
    do {                                   \
        while ( (_tv)->tv_usec >= 1000000 ) \
        {                                  \
            (_tv)->tv_usec -= 1000000;      \
            (_tv)->tv_sec++;                \
        }                                  \
    } while ( 0 )
/*
static inline int time_values_up_to_date(void)
{
	struct vcpu_time_info *src = &HYPERVISOR_shared_info->vcpu_info[0].time; 

	return (shadow.version == src->version);
}



// Scale a 64-bit delta by scaling and multiplying by a 32-bit fraction,
// yielding a 64-bit result.

static inline u64 scale_delta(u64 delta, u32 mul_frac, int shift)
{
	u64 product;
#ifdef __i386__
	u32 tmp1, tmp2;
#endif

	if ( shift < 0 )
		delta >>= -shift;
	else
		delta <<= shift;

#ifdef __i386__
	__asm__ (
		"mul  %5       ; "
		"mov  %4,%%eax ; "
		"mov  %%edx,%4 ; "
		"mul  %5       ; "
		"add  %4,%%eax ; "
		"xor  %5,%5    ; "
		"adc  %5,%%edx ; "
		: "=A" (product), "=r" (tmp1), "=r" (tmp2)
		: "a" ((u32)delta), "1" ((u32)(delta >> 32)), "2" (mul_frac) );
#else
	__asm__ (
		"mul %%rdx ; shrd $32,%%rdx,%%rax"
		: "=a" (product) : "0" (delta), "d" ((u64)mul_frac) );
#endif

	return product;
}

*/
/*
static unsigned long get_nsec_offset(void)
{
	u64 now, delta;
	
	rdtscll(now);
	
	delta = now - shadow.tsc_timestamp;
	return scale_delta(delta, shadow.tsc_to_nsec_mul, shadow.tsc_shift);
}
*/

void get_time_values_from_xen(void)
{
	struct vcpu_time_info *src = &HYPERVISOR_shared_info->vcpu_info[0].time;

 	do {
		shadow.version = src->version;
		#if 0
		rmb();
		shadow.tsc_timestamp     = src->tsc_timestamp;
		shadow.system_timestamp  = src->system_time;
		shadow.tsc_to_nsec_mul   = src->tsc_to_system_mul;
		shadow.tsc_shift         = src->tsc_shift;
		#endif
		rmb();
	}
	while ((src->version & 1) | (shadow.version ^ src->version));

#if 0
	shadow.tsc_to_usec_mul = shadow.tsc_to_nsec_mul / 1000;
#endif
}


//--------------------This fuction is not add monotonic_clock, just return 0

/* monotonic_clock(): returns # of nanoseconds passed since time_init()
 *		Note: This function is required to return accurate
 *		time even in the absence of multiple timer ticks.
 */
u64 monotonic_clock(void)
{
	/*
	u64 time;
	u32 local_time_version;

	do {
		local_time_version = shadow.version;
		rmb();
		time = shadow.system_timestamp + get_nsec_offset();
        if (!time_values_up_to_date())
			get_time_values_from_xen();
		rmb();
	} while (local_time_version != shadow.version);
*/
	return 0;//time;
}

void update_wallclock(void)
{
	shared_info_t *s = HYPERVISOR_shared_info;

	do {
		shadow_ts_version = s->wc_version;
		rmb();
		shadow_ts.ts_sec = s->wc_sec;
		shadow_ts.ts_nsec = s->wc_nsec;
		rmb();
	} while((s->wc_version & 1) | (shadow_ts_version ^ s->wc_version));
}


void gettimeofday(struct timeval *tv)
{
    u64 nsec = monotonic_clock();
    nsec += shadow_ts.ts_nsec;
    
    update_wallclock();
    
    tv->tv_sec = shadow_ts.ts_sec;
    tv->tv_sec += NSEC_TO_SEC(shadow_ts.ts_nsec);
    tv->tv_usec = NSEC_TO_USEC(shadow_ts.ts_nsec % 1000000000UL);
}

void print_current_time(void)
{
    struct timeval tv;    

    gettimeofday(&tv);
    printk("T(s=%ld us=%ld)\n", tv.tv_sec, tv.tv_usec);
}

void block_domain(u32 millisecs)
{
    struct timeval tv;
    gettimeofday(&tv);
    HYPERVISOR_set_timer_op(monotonic_clock() + 1000000LL * (s64) millisecs);
    HYPERVISOR_sched_op(SCHEDOP_block, 0);
}


/*
 * Just a dummy 
 */
static void timer_handler(int ev, struct pt_regs *regs)
{

	/*
    static int i;

    get_time_values_from_xen();
    update_wallclock();
    
    i++;
    if (i >= 1000) {
        print_current_time();
        i = 0;
    }
    */
	//printk("Timer event.\n");
	unmask_evtchn(VIRQ_TIMER);
	local_irq_enable();
}

static void dom_timer_handler(int ev, struct pt_regs *regs)
{
	printk("Dom Timer\n");
}


void time_init(void)
{
    printk("Initialising timer interface\n");
    bind_virq(VIRQ_TIMER, &timer_handler);
    bind_virq(VIRQ_DOM_TIMER, &dom_timer_handler);
}
#endif
