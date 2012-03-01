#ifndef __ZYNQ_TTC_TIMER_H__
#define __ZYNQ_TTC_TIMER_H__

// TTC(Triple Timer Counter)

/*
 * Timer Register Offset Definitions of Timer 1, Increment base address by 4
 * and use same offsets for Timer 2
 */
#define XTTCPSS_CLK_CNTRL_OFFSET        0x00 /* Clock Control Reg, RW */
#define XTTCPSS_CNT_CNTRL_OFFSET        0x0C /* Counter Control Reg, RW */
#define XTTCPSS_COUNT_VAL_OFFSET        0x18 /* Counter Value Reg, RO */
#define XTTCPSS_INTR_VAL_OFFSET         0x24 /* Interval Count Reg, RW */
#define XTTCPSS_MATCH_1_OFFSET          0x30 /* Match 1 Value Reg, RW */
#define XTTCPSS_MATCH_2_OFFSET          0x3C /* Match 2 Value Reg, RW */
#define XTTCPSS_MATCH_3_OFFSET          0x48 /* Match 3 Value Reg, RW */
#define XTTCPSS_ISR_OFFSET              0x54 /* Interrupt Status Reg, RO */
#define XTTCPSS_IER_OFFSET              0x60 /* Interrupt Enable Reg, RW */

#define XTTCPSS_CNT_CNTRL_ENABLE_MASK   0xFFFFFFFE


#endif /* __ZYNQ_TTC_TIMER_H__ */
