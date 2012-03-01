#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/serial.h>
#include <asm/io.h>

#include "xilinx_ps_uart.h"

//----------------------------------------------------------------
// already spin locked
static
void zynq_uart_putc(struct serial_port *port, char c)
{
	struct ns16550_defaults *private_data;

	private_data = (struct ns16550_defaults *)port->uart;

	while (( mmio_readl(private_data->io_base + XUARTPS_SR_OFFSET) & XUARTPS_SR_TXFULL ) == XUARTPS_SR_TXFULL ) {
		// wait for room
	}

	mmio_writel(c, private_data->io_base + XUARTPS_FIFO_OFFSET);
}

static
int zynq_uart_getc(struct serial_port *port, char *pc)
{
	struct ns16550_defaults *private_data;

	private_data = (struct ns16550_defaults *)port->uart;

	if ((mmio_readl(private_data->io_base + XUARTPS_SR_OFFSET) & XUARTPS_SR_RXEMPTY ) == XUARTPS_SR_RXEMPTY ) {
		return 0;
	}

	*pc = mmio_readl(private_data->io_base + XUARTPS_FIFO_OFFSET);

	return 1;
}

static
struct ns16550_defaults zynq_uart_params = {
	.baud      = 115200,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

static
struct uart_driver zynq_uart_driver = {
	.putc = zynq_uart_putc,
	.getc = zynq_uart_getc
};

void zynq_uart_init(void)
{
	uint32_t uart_cr;

	// TODO use dts
	zynq_uart_params.io_base = ZYNQ_UART0_BASE_PHY_ADDR;

	uart_cr = zynq_uart_params.io_base + XUARTPS_CR_OFFSET;

	// disable TX and RX
	mmio_writel(XUARTPS_CR_TX_DIS | XUARTPS_CR_RX_DIS, uart_cr);

	// reset TX and RX
	mmio_writel(XUARTPS_CR_TXRST | XUARTPS_CR_RXRST, uart_cr);

	// enabled TX and RX
	mmio_writel(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN, uart_cr);

	// keep default boudrate, since FPGA
#ifdef TEST
	// why AT? please refer history of AT cmd
	mmio_writel('A', zynq_uart_params.io_base + XUARTPS_FIFO_OFFSET);
	mmio_writel('T', zynq_uart_params.io_base + XUARTPS_FIFO_OFFSET);
#endif

#ifdef TEST
{
	struct serial_port port;
	int i;

	port.uart = &zynq_uart_params;
	for( i = 0 ; i < 10 ; ++i ) {
		char c;
		while ( zynq_uart_getc( &port , &c) == 0 ) ;
		zynq_uart_putc( &port, c );
		zynq_uart_putc( &port, '-' );
		zynq_uart_putc( &port, c + 1 );
		zynq_uart_putc( &port, '\r' );
		zynq_uart_putc( &port, '\n' );
	}
	zynq_uart_putc( &port, '\r' );
	zynq_uart_putc( &port, '\n' );
}
#endif

	serial_register_uart(0, &zynq_uart_driver, &zynq_uart_params);	
}

