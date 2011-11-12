/******************************************************************************
 * omap1610_serial.c
 * 
 * Copyright (c) 2006-2008, J Y Hwang
 */

#include <xen/types.h>

#include <linux/types.h>
#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <asm/io.h>
#include <linux/serial_reg.h>
#include <asm/termbits.h>
#include <xen/config.h>
#include <xen/spinlock.h>
#include <xen/serial.h> // struct serial_port
#include <asm/processor.h>

//#include <xen/irq.h> // struct irqaction
//#include <xen/sched.h> // struct timer
//#include <xen/init.h>
//#include <xen/iocap.h>


// OMAP UART registers' memory base
#define IOMEM_BASE 0xfffb0000
#define IOMEM_REG_SHIFT 0x2
/*
 * Configure serial port with a string <baud>,DPS,<io-base>,<irq>.
 * The tail of the string can be omitted if platform defaults are sufficient.
 * If the baud rate is pre-configured, perhaps by a bootloader, then 'auto'
 * can be specified in place of a numeric baud rate.
 */
static char opt_com1[30] = "", opt_com2[30] = "";
string_param("com1", opt_com1);
string_param("com2", opt_com2);

static struct omap1610_uart{
    int baud, data_bits, parity, stop_bits, irq;
    unsigned long io_base;   /* I/O port or memory-mapped I/O address. */
    char *remapped_io_base;  /* Remapped virtual address of mmap I/O.  */ 
    /* UART with IRQ line: interrupt-driven I/O. */
    // struct irqaction irqaction;
    /* UART with no IRQ line: periodically-polled I/O. */
    // struct timer timer;
    unsigned int timeout_ms;
} omap1610_uart_com[2] = { { 0 } };

#if 0
// NS16550 registers
/* Register offsets */
#define RBR             0x00    /* receive buffer       */
#define THR             0x00    /* transmit holding     */
#define IER             0x01    /* interrupt enable     */
#define IIR             0x02    /* interrupt identity   */
#define FCR             0x02    /* FIFO control         */
#define LCR             0x03    /* line control         */
#define MCR             0x04    /* Modem control        */
#define LSR             0x05    /* line status          */
#define MSR             0x06    /* Modem status         */
#define DLL             0x00    /* divisor latch (ls) (DLAB=1) */
#define DLM             0x01    /* divisor latch (ms) (DLAB=1) */

/* Interrupt Enable Register */
#define IER_ERDAI       0x01    /* rx data recv'd       */
#define IER_ETHREI      0x02    /* tx reg. empty        */
#define IER_ELSI        0x04    /* rx line status       */
#define IER_EMSI        0x08    /* MODEM status         */

/* Interrupt Identification Register */
#define IIR_NOINT       0x01    /* no interrupt pending */
#define IIR_IMASK       0x06    /* interrupt identity:  */
#define IIR_LSI         0x06    /*  - rx line status    */
#define IIR_RDAI        0x04    /*  - rx data recv'd    */
#define IIR_THREI       0x02    /*  - tx reg. empty     */
#define IIR_MSI         0x00    /*  - MODEM status      */

/* FIFO Control Register */
#define FCR_ENABLE      0x01    /* enable FIFO          */
#define FCR_CLRX        0x02    /* clear Rx FIFO        */
#define FCR_CLTX        0x04    /* clear Tx FIFO        */
#define FCR_DMA         0x10    /* enter DMA mode       */
#define FCR_TRG1        0x00    /* Rx FIFO trig lev 1   */
#define FCR_TRG4        0x40    /* Rx FIFO trig lev 4   */
#define FCR_TRG8        0x80    /* Rx FIFO trig lev 8   */
#define FCR_TRG14       0xc0    /* Rx FIFO trig lev 14  */

/* Line Control Register */
#define LCR_DLAB        0x80    /* Divisor Latch Access */

/* Modem Control Register */
#define MCR_DTR         0x01    /* Data Terminal Ready  */
#define MCR_RTS         0x02    /* Request to Send      */
#define MCR_OUT2        0x08    /* OUT2: interrupt mask */

/* Line Status Register */
#define LSR_DR          0x01    /* Data ready           */
#define LSR_OE          0x02    /* Overrun              */
#define LSR_PE          0x04    /* Parity error         */
#define LSR_FE          0x08    /* Framing error        */
#define LSR_BI          0x10    /* Break                */
#define LSR_THRE        0x20    /* Xmit hold reg empty  */
#define LSR_TEMT        0x40    /* Xmitter empty        */
#define LSR_ERR         0x80    /* Error                */
#endif

/* These parity settings can be ORed directly into the LCR. */
#define PARITY_NONE     (0<<3)
#define PARITY_ODD      (1<<3)
#define PARITY_EVEN     (3<<3)
#define PARITY_MARK     (5<<3)
#define PARITY_SPACE    (7<<3)

/* Frequency of external clock source. This definition assumes PC platform. */
#define UART_CLOCK_HZ   1843200

#if 0
static char ns_read_reg(struct omap1610_uart *uart, int reg)
{
    if ( uart->remapped_io_base == NULL )
        return inb(uart->io_base + reg);
    return readb(uart->remapped_io_base + reg);
}

static void ns_write_reg(struct omap1610_uart *uart, int reg, char c)
{
    if ( uart->remapped_io_base == NULL )
        return outb(c, uart->io_base + reg);
    writeb(c, uart->remapped_io_base + reg);
}
#endif

#if 0
// required for post irq
static void omap1610_uart_interrupt(
    int irq, void *dev_id, struct cpu_user_regs *regs)
{
    struct serial_port *port = dev_id;
    struct omap1610_uart *uart = port->uart;

    while ( !(ns_read_reg(uart, IIR) & IIR_NOINT) )
    {
        char lsr = ns_read_reg(uart, LSR);
        if ( lsr & LSR_THRE )
            serial_tx_interrupt(port, regs);
        if ( lsr & LSR_DR )
            serial_rx_interrupt(port, regs);
    }
}
#endif

#if 0
// required for post irq
static void omap1610_uart_poll(void *data)
{
    struct serial_port *port = data;
    struct omap1610_uart *uart = port->uart;
    struct cpu_user_regs *regs = guest_cpu_user_regs();

    while ( ns_read_reg(uart, LSR) & LSR_DR )
        serial_rx_interrupt(port, regs);

    if ( ns_read_reg(uart, LSR) & LSR_THRE )
        serial_tx_interrupt(port, regs);

    set_timer(&uart->timer, NOW() + MILLISECS(uart->timeout_ms));
}
#endif

static unsigned int serial_in(int offset)
{
	return readb((unsigned long) IOMEM_BASE+ \
					(offset<<IOMEM_REG_SHIFT));
}

static void serial_out(int offset, int value)
{
       
    writeb(value, (unsigned long)IOMEM_BASE + (offset<<IOMEM_REG_SHIFT));
    //writel(value, (unsigned long) info->iomem_base + ((offset)<<info->iomem_reg_shift));
    // outb(value, info->port+offset);
}
static int omap1610_uart_tx_empty(struct serial_port *port)
{
#if 0
    struct omap1610_uart *uart = port->uart;
    return !!(ns_read_reg(uart, LSR) & LSR_THRE);
#else
	return 1;
#endif
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)
static int lsr_break_flag;

static inline void wait_for_xmitr(void)
{
    unsigned int status, tmout = 1000000;

    do {
        status = serial_in(UART_LSR);

        if (status & UART_LSR_BI)
            lsr_break_flag = UART_LSR_BI;
        
        if (--tmout == 0)
            break;
    } while((status & BOTH_EMPTY) != BOTH_EMPTY);

#if 0
    /* Wait for flow control if necessary */
    if (info->flags & ASYNC_CONS_FLOW) {
        tmout = 1000000;
        while (--tmout &&
               ((serial_in(info, UART_MSR) & UART_MSR_CTS) == 0));
    }
#endif
   
}

static void omap1610_uart_putc(struct serial_port *port, char c)
{
#if 0
    struct omap1610_uart *uart = port->uart;
    ns_write_reg(uart, THR, c);
#else
    int ier;

	/*
 	 *  First save the IER then disable the interrupts
	 */
	ier = serial_in(UART_IER);
	serial_out(UART_IER, 0x00);
	wait_for_xmitr();
	/* 
	 *  Send the character out.
	 *  If a LF, also do CR...
	 */
	serial_out(UART_TX, c);
	if (c == 10) {
		wait_for_xmitr();
		serial_out(UART_TX, 13);
	}

    /*
     *  Finally, Wait for transmitter & holding register to empty
     *  and restore the IER
     */
    wait_for_xmitr();
    serial_out(UART_IER, ier);
#endif
}

static int omap1610_uart_getc(struct serial_port *port, char *pc)
{
#if 0
    struct omap1610_uart *uart = port->uart;

    if ( !(ns_read_reg(uart, LSR) & LSR_DR) )
        return 0;

    *pc = ns_read_reg(uart, RBR);
    return 1;
#else
	return 1;
#endif
}




static void omap1610_uart_init_preirq(struct serial_port *port)
{
    int cflag = CREAD | HUPCL | CLOCAL;
    unsigned cval;
	int quot;
    cflag |= B115200;
    cflag |= CS8;
	quot = 3000000 / 115200;

	cval = cflag & (CSIZE | CSTOPB);
	cval >>= 4;
#if 0
    if (cflag & PARENB)
        cval |= UART_LCR_PARITY;
    if (!(cflag & PARODD))
        cval |= UART_LCR_EPAR;
#endif

	serial_out(UART_OMAP_MDR1, 0x07); /* disable UART */
	serial_out(UART_LCR, 0xBF);   /* select EFR */
	serial_out(UART_EFR, UART_EFR_ECB);
	serial_out(UART_LCR, cval | UART_LCR_DLAB); /* set DLAB */
	serial_out(UART_DLL, 0x00);
	serial_out(UART_DLM, 0x00);
	serial_out(UART_LCR, cval);   /* reset DLAB */
	serial_out(UART_OMAP_SCR, 0x00);
	serial_out(UART_IER, 0x00);
	serial_out(UART_FCR, UART_FCR_ENABLE_FIFO |
					UART_FCR_CLEAR_RCVR |
					UART_FCR_CLEAR_XMIT);
	serial_out(UART_MCR, 0x40);   /* enable TCR/TLR */
	serial_out(UART_OMAP_TCR, 0x0F);
	serial_out(UART_OMAP_TLR, 0x00);
	serial_out(UART_MCR, 0x00);
	serial_out(UART_LCR, 0xBF);   /* select EFR */
	serial_out(UART_EFR, 0x00);
	serial_out(UART_LCR, cval);   /* reset DLAB */
	serial_out(UART_OMAP_MDR1, 0x00); /* enable UART */

	serial_out(UART_LCR, cval | UART_LCR_DLAB);   /* set DLAB */
	serial_out(UART_DLL, quot & 0xff);    /* LS of divisor */
	serial_out(UART_DLM, quot >> 8);      /* MS of divisor */
	serial_out(UART_LCR, cval);       /* reset DLAB */
	serial_out(UART_IER, 0);
	serial_out(UART_MCR, UART_MCR_DTR | UART_MCR_RTS);
}

static void omap1610_uart_init_postirq(struct serial_port *port)
{
#if 0
    struct omap1610_uart *uart = port->uart;
    int rc, bits;

    if ( uart->irq < 0 )
        return;

    serial_async_transmit(port);

    if ( uart->irq == 0 )
    {
        /* Polled mode. Calculate time to fill RX FIFO and/or empty TX FIFO. */
        bits = uart->data_bits + uart->stop_bits + !!uart->parity;
        uart->timeout_ms = max_t(
            unsigned int, 1, (bits * port->tx_fifo_size * 1000) / uart->baud);
        init_timer(&uart->timer, omap1610_uart_poll, port, 0);
        set_timer(&uart->timer, NOW() + MILLISECS(uart->timeout_ms));
    }
    else
    {
        uart->irqaction.handler = omap1610_uart_interrupt;
        uart->irqaction.name    = "omap1610_uart";
        uart->irqaction.dev_id  = port;
        if ( (rc = setup_irq(uart->irq, &uart->irqaction)) != 0 )
            printk("ERROR: Failed to allocate na16550 IRQ %d\n", uart->irq);

        /* Master interrupt enable; also keep DTR/RTS asserted. */
        ns_write_reg(uart, MCR, MCR_OUT2 | MCR_DTR | MCR_RTS);

        /* Enable receive and transmit interrupts. */
        ns_write_reg(uart, IER, IER_ERDAI | IER_ETHREI);
    }
#endif
}
#define omap1610_uart_endboot NULL
static struct uart_driver omap1610_uart_driver = {
    .init_preirq  = omap1610_uart_init_preirq,
    .init_postirq = omap1610_uart_init_postirq,
    .endboot      = omap1610_uart_endboot,
    .tx_empty     = omap1610_uart_tx_empty,
    .putc         = omap1610_uart_putc,
    .getc         = omap1610_uart_getc
};

static int parse_parity_char(int c)
{
    switch ( c )
    {
    case 'n':
        return PARITY_NONE;
    case 'o': 
        return PARITY_ODD;
    case 'e': 
        return PARITY_EVEN;
    case 'm': 
        return PARITY_MARK;
    case 's': 
        return PARITY_SPACE;
    }
    return 0;
}

#define PARSE_ERR(_f, _a...)                 \
    do {                                     \
        printk( "ERROR: " _f "\n" , ## _a ); \
        return;                              \
    } while ( 0 )

static void omap1610_uart_parse_port_config(struct omap1610_uart *uart, char *conf)
{
#if 0
    int baud;

    /* No user-specified configuration? */
    if ( (conf == NULL) || (*conf == '\0') )
    {
        /* Some platforms may automatically probe the UART configuartion. */
        if ( uart->baud != 0 )
            goto config_parsed;
        return;
    }

    if ( strncmp(conf, "auto", 4) == 0 )
    {
        uart->baud = BAUD_AUTO;
        conf += 4;
    }
    else if ( (baud = simple_strtoul(conf, &conf, 10)) != 0 )
        uart->baud = baud;

    if ( *conf != ',' )
        goto config_parsed;
    conf++;

    uart->data_bits = simple_strtoul(conf, &conf, 10);

    uart->parity = parse_parity_char(*conf);
    conf++;

    uart->stop_bits = simple_strtoul(conf, &conf, 10);

    if ( *conf == ',' )
    {
        conf++;
        uart->io_base = simple_strtoul(conf, &conf, 0);

        if ( *conf == ',' )
        {
            conf++;
            uart->irq = simple_strtoul(conf, &conf, 10);
        }
    }

 config_parsed:
    /* Sanity checks. */
    if ( (uart->baud != BAUD_AUTO) &&
         ((uart->baud < 1200) || (uart->baud > 115200)) )
        PARSE_ERR("Baud rate %d outside supported range.", uart->baud);
    if ( (uart->data_bits < 5) || (uart->data_bits > 8) )
        PARSE_ERR("%d data bits are unsupported.", uart->data_bits);
    if ( (uart->stop_bits < 1) || (uart->stop_bits > 2) )
        PARSE_ERR("%d stop bits are unsupported.", uart->stop_bits);
    if ( uart->io_base == 0 )
        PARSE_ERR("I/O base address must be specified.");
#endif

    /* Register with generic serial driver. */
    serial_register_uart(uart - omap1610_uart_com, &omap1610_uart_driver, uart);
}

void omap1610_uart_init(int index, struct omap1610_uart_defaults *defaults)
{
    struct omap1610_uart *uart = &omap1610_uart_com[index];

    if ( (index < 0) || (index > 1) )
        return;

    if ( defaults != NULL )
    {
        uart->baud      = defaults->baud;
        uart->data_bits = defaults->data_bits;
        uart->parity    = parse_parity_char(defaults->parity);
        uart->stop_bits = defaults->stop_bits;
        uart->irq       = defaults->irq;
        uart->io_base   = defaults->io_base;
    }

    omap1610_uart_parse_port_config(uart, (index == 0) ? opt_com1 : opt_com2);
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
