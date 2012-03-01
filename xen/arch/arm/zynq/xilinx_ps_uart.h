#ifndef __ZYNQ_XILINX_PS_UART_H__
#define __ZYNQ_XILINX_PS_UART_H__

#define ZYNQ_UART0_BASE_PHY_ADDR 0xE0000000
#define ZYNQ_UART1_BASE_PHY_ADDR 0xE0001000

/********************************Register Map********************************/
/** UART
 *
 * Register offsets for the UART.
 *
 */
#define XUARTPS_CR_OFFSET       0x00  /* Control Register [8:0] */
#define XUARTPS_MR_OFFSET       0x04  /* Mode Register [10:0] */
#define XUARTPS_IER_OFFSET      0x08  /* Interrupt Enable [10:0] */
#define XUARTPS_IDR_OFFSET      0x0C  /* Interrupt Disable [10:0] */
#define XUARTPS_IMR_OFFSET      0x10  /* Interrupt Mask [10:0] */
#define XUARTPS_ISR_OFFSET      0x14  /* Interrupt Status [10:0]*/
#define XUARTPS_BAUDGEN_OFFSET  0x18  /* Baud Rate Generator [15:0] */
#define XUARTPS_RXTOUT_OFFSET   0x1C  /* RX Timeout [7:0] */
#define XUARTPS_RXWM_OFFSET     0x20  /* RX FIFO Trigger Level [5:0] */
#define XUARTPS_MODEMCR_OFFSET  0x24  /* Modem Control [5:0] */
#define XUARTPS_MODEMSR_OFFSET  0x28  /* Modem Status [8:0] */
#define XUARTPS_SR_OFFSET       0x2C  /* Channel Status [11:0] */
#define XUARTPS_FIFO_OFFSET     0x30  /* FIFO [15:0] or [7:0] */
#define XUARTPS_BAUDDIV_OFFSET  0x34  /* Baud Rate Divider [7:0] */
#define XUARTPS_FLOWDEL_OFFSET  0x38  /* Flow Delay [15:0] */
#define XUARTPS_IRRX_PWIDTH_OFFSET 0x3C /* IR Minimum Received Pulse
                                                Width [15:0] */
#define XUARTPS_IRTX_PWIDTH_OFFSET 0x40 /* IR Transmitted pulse


/** Control Register
 *
 * The Control register (CR) controls the major functions of the device.
 *
 * Control Register Bit Definitions
 */
#define XUARTPS_CR_STOPBRK      0x00000100  /* Stop TX break */
#define XUARTPS_CR_STARTBRK     0x00000080  /* Set TX break */
#define XUARTPS_CR_TX_DIS       0x00000020  /* TX disabled. */
#define XUARTPS_CR_TX_EN        0x00000010  /* TX enabled */
#define XUARTPS_CR_RX_DIS       0x00000008  /* RX disabled. */
#define XUARTPS_CR_RX_EN        0x00000004  /* RX enabled */
#define XUARTPS_CR_TXRST        0x00000002  /* TX logic reset */
#define XUARTPS_CR_RXRST        0x00000001  /* RX logic reset */
#define XUARTPS_CR_RST_TO       0x00000040  /* Restart Timeout Counter */

/** Channel Status Register
 *
 * The channel status register (CSR) is provided to enable the control logic
 * to monitor the status of bits in the channel interrupt status register,
 * even if these are masked out by the interrupt mask register.
 */
#define XUARTPS_SR_RXEMPTY      0x00000002 /* RX FIFO empty */
#define XUARTPS_SR_TXEMPTY      0x00000008 /* TX FIFO empty */
#define XUARTPS_SR_TXFULL       0x00000010 /* TX FIFO full */
#define XUARTPS_SR_RXTRIG       0x00000001 /* Rx Trigger */

#endif /* __ZYNQ_XILINX_PS_UART_H__ */
