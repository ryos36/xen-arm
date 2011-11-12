#ifndef __DEVICE_UART_H__
#define __DEVICE_UART_H__

/* These parity settings can be ORed directly into the LCR. */
#define PARITY_NONE     (0<<3)
#define PARITY_ODD      (1<<3)
#define PARITY_EVEN     (3<<3)
#define PARITY_MARK     (5<<3)
#define PARITY_SPACE    (7<<3)

struct uart {
	int	baud;
        int	data_bits;
        int	parity;
        int	stop_bits;
        int	irq;
};

#endif

