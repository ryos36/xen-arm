/******************************************************************************
 * imx21ads_serial.c
 * 
 * Copyright (c) 2006-2008, J Y Hwang
 */

#include <xen/types.h>
	//#include <linux/types.h>
#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/spinlock.h>
#include <xen/serial.h> // struct serial_port
#include <asm/io.h>
#include <asm/termbits.h>	 
#include <xen-asm/serial_reg.h>	// appended by whang, and modified by PCJ, added for OMAP, iMX21ADS
#include <asm/arch/imx-regs.h>

#define IOMEM_BASE 0xffc0a040	// imx21ads uart virtual base address (physical : 0x1000a000) by PCJ
#define IOMEM_REG_SHIFT 0x2	// for shift...???  by PCJ
/*
 * Configure serial port with a string <baud>,DPS,<io-base>,<irq>.
 * The tail of the string can be omitted if platform defaults are sufficient.
 * If the baud rate is pre-configured, perhaps by a bootloader, then 'auto'
 * can be specified in place of a numeric baud rate.
 */
static char opt_com1[30] = "", opt_com2[30] = "";
string_param("com1", opt_com1);
string_param("com2", opt_com2);

static struct imx21ads_uart{
    int baud, data_bits, parity, stop_bits, irq;
    unsigned long io_base;   /* I/O port or memory-mapped I/O address. */
    char *remapped_io_base;  /* Remapped virtual address of mmap I/O.  */ 
    /* UART with IRQ line: interrupt-driven I/O. */
    // struct irqaction irqaction;
    /* UART with no IRQ line: periodically-polled I/O. */
    // struct timer timer;
    unsigned int timeout_ms;
} imx21ads_uart_com[2] = { { 0 } };

/* These parity settings can be ORed directly into the LCR. */
#define PARITY_NONE     (0<<3)
#define PARITY_ODD      (1<<3)
#define PARITY_EVEN     (3<<3)
#define PARITY_MARK     (5<<3)
#define PARITY_SPACE    (7<<3)

/* Frequency of external clock source. This definition assumes PC platform. */
#define UART_CLOCK_HZ   1843200

static unsigned int serial_in(int offset)
{
	return readb((unsigned long) IOMEM_BASE+ \
					(offset<<IOMEM_REG_SHIFT));
}

static void serial_out(int offset, int value)
{
       
    writeb(value, (unsigned long)IOMEM_BASE + (offset<<IOMEM_REG_SHIFT));
}
static int imx21ads_uart_tx_empty(struct serial_port *port)
{
	return 1;
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)
static int lsr_break_flag;

static inline void wait_for_xmitr(void)
{
    unsigned int status, tmout = 1000;

    do {
        status = serial_in(UART_LSR);

        if (status & UART_LSR_BI)
            lsr_break_flag = UART_LSR_BI;
        
        if (--tmout == 0)
            break;
    } while((status & BOTH_EMPTY) != BOTH_EMPTY);

}

static void imx21ads_uart_putc(struct serial_port *port, char c)
{
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
}

static int imx21ads_uart_getc(struct serial_port *port, char *pc)
{
      return 1;
}


void imx_gpio_mode(int gpio_mode)
{
        unsigned int pin = gpio_mode & GPIO_PIN_MASK;
        unsigned int port = (gpio_mode & GPIO_PORT_MASK) >> GPIO_PORT_POS;
        unsigned int ocr = (gpio_mode & GPIO_OCR_MASK) >> GPIO_OCR_POS;
        unsigned int tmp;

        /* Pullup enable */
        if(gpio_mode & GPIO_PUEN)
                PUEN(port) |= (1<<pin);
        else
                PUEN(port) &= ~(1<<pin);

        /* Data direction */
        if(gpio_mode & GPIO_OUT)
                DDIR(port) |= 1<<pin;
        else
                DDIR(port) &= ~(1<<pin);

        /* Primary / alternate function */
        if(gpio_mode & GPIO_AF)
                GPR(port) |= (1<<pin);
        else
                GPR(port) &= ~(1<<pin);

        /* use as gpio? */
        if( ocr == 3 )
                GIUS(port) |= (1<<pin);
        else
                GIUS(port) &= ~(1<<pin);

        /* Output / input configuration */
        /* FIXME: I'm not very sure about OCR and ICONF, someone
         * should have a look over it
         */
        if(pin<16) {
                tmp = OCR1(port);
                tmp &= ~( 3<<(pin*2));
                tmp |= (ocr << (pin*2));
                OCR1(port) = tmp;

                if( gpio_mode & GPIO_AOUT )
                        ICONFA1(port) &= ~( 3<<(pin*2));
                if( gpio_mode & GPIO_BOUT )
                        ICONFB1(port) &= ~( 3<<(pin*2));
        } else {
                tmp = OCR2(port);
                tmp &= ~( 3<<((pin-16)*2));
                tmp |= (ocr << ((pin-16)*2));
                OCR2(port) = tmp;

                if( gpio_mode & GPIO_AOUT )
                        ICONFA2(port) &= ~( 3<<((pin-16)*2));
                if( gpio_mode & GPIO_BOUT )
                        ICONFB2(port) &= ~( 3<<((pin-16)*2));
        }
}




static void imx21ads_uart_init_preirq(struct serial_port *port)
{
    int cflag = CREAD | HUPCL | CLOCAL;
    unsigned cval;
	int quot;
    cflag |= B115200;
    cflag |= CS8;
	quot = 3000000 / 115200;

	cval = cflag & (CSIZE | CSTOPB);
	cval >>= 4;
    if (cflag & PARENB)
        cval |= UART_LCR_PARITY;
    if (!(cflag & PARODD))
        cval |= UART_LCR_EPAR;
}

static void imx21ads_uart_init_postirq(struct serial_port *port)
{
}
#define imx21ads_uart_endboot NULL
static struct uart_driver imx21ads_uart_driver = {
    .init_preirq  = imx21ads_uart_init_preirq,
    .init_postirq = imx21ads_uart_init_postirq,
    .endboot      = imx21ads_uart_endboot,
    .tx_empty     = imx21ads_uart_tx_empty,
    .putc         = imx21ads_uart_putc,
    .getc         = imx21ads_uart_getc
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

static void imx21ads_uart_parse_port_config(struct imx21ads_uart *uart, char *conf)
{
    /* Register with generic serial driver. */
    serial_register_uart(uart - imx21ads_uart_com, &imx21ads_uart_driver, uart);
}

void imx21ads_uart_init(int index, struct imx21ads_uart_defaults *defaults)
{
    struct imx21ads_uart *uart = &imx21ads_uart_com[index];

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

    imx21ads_uart_parse_port_config(uart, (index == 0) ? opt_com1 : opt_com2);
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
