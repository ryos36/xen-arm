#ifndef __ZYNQ_IRQS_H__
#define __ZYNQ_IRQS_H__

#define NR_IRQS (32 + 64)

/* 
 * SGI(Software Generated Interrupt)  0..15
 * PPI(Private Peripheral Interrupt) 16..31
 * SPI(Shared Peripheral Interrupt)  32.. max 1020 */

#define INT_PPI_0_GTIMER        27
#define INT_PPI_1_nFIQ          28
#define INT_PPI_2_PTIMER        29
#define INT_PPI_3_WDT           30
#define INT_PPI_4_nIRQ          31

#if 0
#define INT_PPI_0_GTIMER_ICDICFR b11 // rising edge
#define INT_PPI_1_nFIQ_ICDICFR   b01 // active low
#define INT_PPI_2_PTIMER_ICDICFR b11 // rising edge
#define INT_PPI_3_WDT_ICDICFR    b11 // rising edge
#define INT_PPI_4_nIRQ_ICDICFR   b01 // active low
#endif

#define INT_SPI_BASE		32

#define INT_UART0		(INT_SPI_BASE + 27)
#define INT_UART1		(INT_SPI_BASE + 50)

#define INT_TTC0_0              (INT_SPI_BASE + 10)
#define INT_TTC0_1              (INT_SPI_BASE + 11)
#define INT_TTC0_2              (INT_SPI_BASE + 12)

#define INT_TTC1_0              (INT_SPI_BASE + 37)
#define INT_TTC1_1              (INT_SPI_BASE + 38)
#define INT_TTC1_2              (INT_SPI_BASE + 39)

#define INT_GPIO                (INT_SPI_BASE + 20)

#define INT_KEYBOARD            (INT_SPI_BASE + 28)
#define INT_MOUSE               (INT_SPI_BASE + 29)

#define INT_EHCI                (INT_SPI_BASE + 44)

#define INT_GEM0                (INT_SPI_BASE + 54 - 32)

#endif /* __ZYNQ_IRQS_H__ */
