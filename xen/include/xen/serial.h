/******************************************************************************
 * serial.h
 * 
 * Framework for serial device drivers.
 * 
 * Copyright (c) 2003-2005, K A Fraser
 */

#ifndef __XEN_SERIAL_H__
#define __XEN_SERIAL_H__

struct cpu_user_regs;

/* Register a character-receive hook on the specified COM port. */
typedef void (*serial_rx_fn)(char, struct cpu_user_regs *);
void serial_set_rx_handler(int handle, serial_rx_fn fn);

/* Number of characters we buffer for a polling receiver. */
#define SERIAL_RXBUFSZ 32
#define MASK_SERIAL_RXBUF_IDX(_i) ((_i)&(SERIAL_RXBUFSZ-1))

/* Number of characters we buffer for an interrupt-driven transmitter. */
#define SERIAL_TXBUFSZ 16384
#define MASK_SERIAL_TXBUF_IDX(_i) ((_i)&(SERIAL_TXBUFSZ-1))

struct uart_driver;

struct serial_port {
    /* Uart-driver parameters. */
    struct uart_driver *driver;
    void               *uart;
    /* Number of characters the port can hold for transmit. */
    int                 tx_fifo_size;
    /* Transmit data buffer (interrupt-driven uart). */
    char               *txbuf;
    unsigned int        txbufp, txbufc;
    /* Force synchronous transmit. */
    int                 sync;
    /* Receiver callback functions (asynchronous receivers). */
    serial_rx_fn        rx_lo, rx_hi, rx;
    /* Receive data buffer (polling receivers). */
    char                rxbuf[SERIAL_RXBUFSZ];
    unsigned int        rxbufp, rxbufc;
    /* Serial I/O is concurrency-safe. */
    spinlock_t          rx_lock, tx_lock;
};

struct uart_driver {
    /* Driver initialisation (pre- and post-IRQ subsystem setup). */
    void (*init_preirq)(struct serial_port *);
    void (*init_postirq)(struct serial_port *);
    /* Hook to clean up after Xen bootstrap (before domain 0 runs). */
    void (*endboot)(struct serial_port *);
    /* Transmit FIFO ready to receive up to @tx_fifo_size characters? */
    int  (*tx_empty)(struct serial_port *);
    /* Put a character onto the serial line. */
    void (*putc)(struct serial_port *, char);
    /* Get a character from the serial line: returns 0 if none available. */
    int  (*getc)(struct serial_port *, char *);
};

/* 'Serial handles' are composed from the following fields. */
#define SERHND_IDX      (1<<0) /* COM1 or COM2?                           */
#define SERHND_HI       (1<<1) /* Mux/demux each transferred char by MSB. */
#define SERHND_LO       (1<<2) /* Ditto, except that the MSB is cleared.  */
#define SERHND_COOKED   (1<<3) /* Newline/carriage-return translation?    */

/* Two-stage initialisation (before/after IRQ-subsystem initialisation). */
void serial_init_preirq(void);
void serial_init_postirq(void);

/* Clean-up hook before domain 0 runs. */
void serial_endboot(void);

/* Takes a config string and creates a numeric handle on the COM port. */
int serial_parse_handle(char *conf);

/* Transmit a single character via the specified COM port. */
void serial_putc(int handle, char c);

/* Transmit a NULL-terminated string via the specified COM port. */
void serial_puts(int handle, const char *s);

/*
 * An alternative to registering a character-receive hook. This function
 * will not return until a character is available. It can safely be
 * called with interrupts disabled.
 */
char serial_getc(int handle);

/* Forcibly prevent serial lockup when the system is in a bad way. */
/* (NB. This also forces an implicit serial_start_sync()). */
void serial_force_unlock(int handle);

/* Start/end a synchronous region (temporarily disable interrupt-driven tx). */
void serial_start_sync(int handle);
void serial_end_sync(int handle);

/* Return number of bytes headroom in transmit buffer. */
int serial_tx_space(int handle);

/*
 * Initialisation and helper functions for uart drivers.
 */
/* Register a uart on serial port @idx (e.g., @idx==0 is COM1). */
void serial_register_uart(int idx, struct uart_driver *driver, void *uart);
/* Place the serial port into asynchronous transmit mode. */
void serial_async_transmit(struct serial_port *port);
/* Process work in interrupt context. */
void serial_rx_interrupt(struct serial_port *port, struct cpu_user_regs *regs);
void serial_tx_interrupt(struct serial_port *port, struct cpu_user_regs *regs);

/*
 * Initialisers for individual uart drivers.
 */
/* NB. Any default value can be 0 if it is unknown and must be specified. */
struct ns16550_defaults {
    int baud;      /* default baud rate; BAUD_AUTO == pre-configured */
    int data_bits; /* default data bits (5, 6, 7 or 8) */
    int parity;    /* default parity (n, o, e, m or s) */
    int stop_bits; /* default stop bits (1 or 2) */
    int irq;       /* default irq */
    unsigned long io_base; /* default io_base address */
};
void ns16550_init(int index, struct ns16550_defaults *defaults);

/* Baud rate was pre-configured before invoking the UART driver. */
#define BAUD_AUTO (-1)

#endif /* __XEN_SERIAL_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
