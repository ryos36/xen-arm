/*
 * platform.c
 *
 * Copyright (C) 2008, 2009 Samsung Electronics 
 *         Sang-bum Suh <sbuk.suh@samsung.com>
 *         JaeMin Ryu   <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of 
 * Xen on ARM and the associated access control.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/serial.h>
#include <xen/errno.h>
#include <xen/smp.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <asm/arch/scu.h>

#define UARTA_LSR_THRE	(0x20)
#define UARTA_LSR_TMTY	(0x40)

#define UARTA_THR_DLAB(x) (*((volatile u32 *)IO_ADDRESS((0x0 + (x)))))   /* Transmitter Register */
#define UARTA_LSR(x) (*((volatile u32 *)IO_ADDRESS((0x14 + (x)))))   /* Transmitter Register */
#define UARTA_LCR(x) (*((volatile u32 *)IO_ADDRESS((0xc + (x)))))   /* Transmitter Register */
#define UARTA_FCR(x) (*((volatile u32 *)IO_ADDRESS((0x8 + (x)))))   /* Transmitter Register */


// Register APBDMA_IRQ_MASK_CLR_0  
#define APBDMA_IRQ_MASK_CLR_0                   _MK_ADDR_CONST(0x24)
#define APBDMA_IRQ_MASK_CLR_0_SECURE                    0x0
#define APBDMA_IRQ_MASK_CLR_0_WORD_COUNT                        0x1
#define APBDMA_IRQ_MASK_CLR_0_RESET_VAL                         _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_CLR_0_RESET_MASK                        _MK_MASK_CONST(0xffff)
#define APBDMA_IRQ_MASK_CLR_0_SW_DEFAULT_VAL                    _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_CLR_0_SW_DEFAULT_MASK                   _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_CLR_0_READ_MASK                         _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_CLR_0_WRITE_MASK                        _MK_MASK_CONST(0xffff)


// Register APBDMA_IRQ_MASK_SET_0  
#define APBDMA_IRQ_MASK_SET_0                   _MK_ADDR_CONST(0x20)
#define APBDMA_IRQ_MASK_SET_0_SECURE                    0x0
#define APBDMA_IRQ_MASK_SET_0_WORD_COUNT                        0x1
#define APBDMA_IRQ_MASK_SET_0_RESET_VAL                         _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_SET_0_RESET_MASK                        _MK_MASK_CONST(0xffff)
#define APBDMA_IRQ_MASK_SET_0_SW_DEFAULT_VAL                    _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_SET_0_SW_DEFAULT_MASK                   _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_SET_0_READ_MASK                         _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_MASK_SET_0_WRITE_MASK                        _MK_MASK_CONST(0xffff)

// Register APBDMA_IRQ_STA_CPU_0  
#define APBDMA_IRQ_STA_CPU_0                    _MK_ADDR_CONST(0x14)
#define APBDMA_IRQ_STA_CPU_0_SECURE                     0x0
#define APBDMA_IRQ_STA_CPU_0_WORD_COUNT                         0x1
#define APBDMA_IRQ_STA_CPU_0_RESET_VAL                  _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_STA_CPU_0_RESET_MASK                         _MK_MASK_CONST(0xffff)
#define APBDMA_IRQ_STA_CPU_0_SW_DEFAULT_VAL                     _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_STA_CPU_0_SW_DEFAULT_MASK                    _MK_MASK_CONST(0x0)
#define APBDMA_IRQ_STA_CPU_0_READ_MASK                  _MK_MASK_CONST(0xffff)
#define APBDMA_IRQ_STA_CPU_0_WRITE_MASK                         _MK_MASK_CONST(0x0)

void *gic_cpu_base_addr = 0;

static void tegra250_platform_halt(int mode)
{
}
 
DECLARE_PLATFORM_OP(platform_halt, tegra250_platform_halt);

static void tegra250_memory_init(void)
{
	register_memory_bank(PHYS_OFFSET, 0x40000000);
}

static void tegra250_uart_putc(struct serial_port *port, char c)
{
#if 0
	while((UARTA_LSR(TEGRA_UARTA_BASE) & (UARTA_LSR_TMTY|UARTA_LSR_THRE)) != (UARTA_LSR_TMTY|UARTA_LSR_THRE));

	UARTA_THR_DLAB(TEGRA_UARTA_BASE) = c;

	if(c == '\n') {
		tegra250_uart_putc(port, '\r');
	}
#endif
}

static int tegra250_uart_getc(struct serial_port *port, char *pc)
{
	return 1;
}

static struct ns16550_defaults tegra250_uart_params = {
	.baud      = BAUD_AUTO,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

static struct uart_driver tegra250_uart_driver = {
	.putc = tegra250_uart_putc,
	.getc = tegra250_uart_getc
};

static void tegra250_uart_init(void)
{
  //  UARTA_LCR(TEGRA_UARTA_BASE) = 0xb;
  //  UARTA_FCR(TEGRA_UARTA_BASE) = 0x81;

	serial_register_uart(0, &tegra250_uart_driver, &tegra250_uart_params);	
}


struct tegra_irq_chip {
	unsigned int irq_start;
	void  *mmio;
	/* context save/restore data for interrupts */
};

static struct tegra_irq_chip tegra_chip[(INT_SYS_NR+INT_SYS_SZ-1)/INT_SYS_SZ];


#define TEGRA250_GIC_CPU_BASE          0x50040100

static void tegra_mask(unsigned int irq)
{
	struct tegra_irq_chip *chip;
	gic_mask_irq(irq);
	irq -= INT_PRI_BASE;
	chip = &tegra_chip[irq/INT_SYS_SZ];
	mmio_writel(1<<(irq&31), chip->mmio + ICTLR_CPU_IER_CLR_0);
}

static void tegra_unmask(unsigned int irq)
{
	struct tegra_irq_chip *chip;
	gic_unmask_irq(irq);
	irq -= INT_PRI_BASE;
	chip = &tegra_chip[irq/INT_SYS_SZ];
	mmio_writel(1<<(irq&31), chip->mmio + ICTLR_CPU_IER_SET_0);
}

static void tegra_ack(unsigned int irq)
{
	gic_ack_irq(irq);
}

extern void gic_set_cpu(unsigned int irq, cpumask_t mask_val);

static struct irqchip tegra_irq = {
	.trigger_type		= "level",
	.mask		= tegra_mask,
	.unmask		= tegra_unmask,
	.ack		= tegra_ack,
#ifdef CONFIG_SMP
	.set_affinity	= gic_set_cpu,
#endif

};

struct apbdma_irq_chip {
	unsigned int irq_start;
	void  *mmio;
	spinlock_t lock;
};
static struct apbdma_irq_chip apbdma_chip;


static void apbdma_ack(unsigned int irq) { }

static void apbdma_mask(unsigned int irq)
{
	struct apbdma_irq_chip *chip = get_irq_chip_data(irq);
	irq -= chip->irq_start;
	mmio_writel(1<<irq, chip->mmio + APBDMA_IRQ_MASK_CLR_0);
}

static void apbdma_unmask(unsigned int irq)
{
	struct apbdma_irq_chip *chip = get_irq_chip_data(irq);
	irq -= chip->irq_start;
	mmio_writel(1<<irq, chip->mmio + APBDMA_IRQ_MASK_SET_0);
}

static  unsigned long __fls(unsigned long word)
{
	int num = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
	if (!(word & (~0ul << 32))) {
		num -= 32;
		word <<= 32;
	}
#endif
	if (!(word & (~0ul << (BITS_PER_LONG-16)))) {
		num -= 16;
		word <<= 16;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-8)))) {
		num -= 8;
		word <<= 8;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-4)))) {
		num -= 4;
		word <<= 4;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-2)))) {
		num -= 2;
		word <<= 2;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-1))))
		num -= 1;
	return num;
}

static inline void generic_handle_irq_desc(unsigned int irq, struct irqdesc *desc)
{
	desc->handle(irq, desc,NULL);
}


static inline void generic_handle_irq(unsigned int irq)
{
	generic_handle_irq_desc(irq, get_irq_descriptor(irq));
}


static void apbdma_cascade(unsigned int irq, struct irqdesc *desc)
{
	struct irqchip *pri = get_irq_chip(irq);
	struct apbdma_irq_chip *chip = get_irq_chip_data(irq);
	u32 reg, ch=0;

	pri->ack(irq);
	spin_lock(&chip->lock);
	reg = mmio_readl(chip->mmio + APBDMA_IRQ_STA_CPU_0);
	if (reg) {
		reg = __fls(reg);
		mmio_writel(1<<reg, chip->mmio + APBDMA_IRQ_STA_CPU_0);
		ch = chip->irq_start + reg;
	}
	spin_unlock(&chip->lock);
	if (ch)	generic_handle_irq(ch);
	pri->unmask(irq);
}

static struct irqchip apbdma_irq = {
	.trigger_type		= "level",
	.ack	= apbdma_ack,
	.mask	= apbdma_mask,
	.unmask	= apbdma_unmask,
};


#define GPIO_BANK(x)        ((x) >> 5)
#define GPIO_PORT(x)        (((x) >> 3) & 0x3)
#define GPIO_BIT(x)         ((x) & 0x7)


static unsigned long add_gpio_base = 0;
#define GPIO_REG(x)   ((add_gpio_base + GPIO_BANK(x)*0x80) +  GPIO_PORT(x)*4)

#define GPIO_CNF(x)     (GPIO_REG(x) + 0x00)
#define GPIO_OE(x)      (GPIO_REG(x) + 0x10)
#define GPIO_OUT(x)     (GPIO_REG(x) + 0X20)
#define GPIO_IN(x)      (GPIO_REG(x) + 0x30)

#define GPIO_INT_STA(x)     (GPIO_REG(x) + 0x40)
#define GPIO_INT_ENB(x)     (GPIO_REG(x) + 0x50)
#define GPIO_INT_LVL(x)     (GPIO_REG(x) + 0x60)
#define GPIO_INT_CLR(x)     (GPIO_REG(x) + 0x70)

#define GPIO_MSK_CNF(x)     (GPIO_REG(x) + 0x800)
#define GPIO_MSK_OE(x)      (GPIO_REG(x) + 0x810)
#define GPIO_MSK_OUT(x)     (GPIO_REG(x) + 0X820)
#define GPIO_MSK_INT_STA(x) (GPIO_REG(x) + 0x840)
#define GPIO_MSK_INT_ENB(x) (GPIO_REG(x) + 0x850)
#define GPIO_MSK_INT_LVL(x) (GPIO_REG(x) + 0x860)

#define GPIO_INT_LVL_MASK           0x010101
#define GPIO_INT_LVL_EDGE_RISING    0x000101
#define GPIO_INT_LVL_EDGE_FALLING   0x000100
#define GPIO_INT_LVL_EDGE_BOTH      0x010100
#define GPIO_INT_LVL_LEVEL_HIGH     0x000001
#define GPIO_INT_LVL_LEVEL_LOW      0x000000
#define MAX_GPIO_INSTANCES  10


#define LEVEL_STRING	"level"
#define EDGE_STRING	"edge"


struct tegra_gpio_bank {
	int bank;
	int irq;
	spinlock_t lvl_lock[4];
};

static struct tegra_gpio_bank tegra_gpio_banks[] = {
	{.bank = 0, .irq = INT_GPIO1},
	{.bank = 1, .irq = INT_GPIO2},
	{.bank = 2, .irq = INT_GPIO3},
	{.bank = 3, .irq = INT_GPIO4},
	{.bank = 4, .irq = INT_GPIO5},
	{.bank = 5, .irq = INT_GPIO6},
	{.bank = 6, .irq = INT_GPIO7},
};


static void tegra_gpio_mask_write(u32 reg, int gpio, int value)
{
	u32 val;

	val = 0x100 << GPIO_BIT(gpio);
	if (value)
		val |= 1 << GPIO_BIT(gpio);
	mmio_writel(val, reg);
}


static void tegra_gpio_irq_ack(unsigned int irq)
{
	int gpio = irq - INT_GPIO_BASE;

	mmio_writel(1 << GPIO_BIT(gpio), GPIO_INT_CLR(gpio));
}

static void tegra_gpio_irq_mask(unsigned int irq)
{
	int gpio = irq - INT_GPIO_BASE;

	tegra_gpio_mask_write(GPIO_MSK_INT_ENB(gpio), gpio, 0);
}

static void tegra_gpio_irq_unmask(unsigned int irq)
{
	int gpio = irq - INT_GPIO_BASE;

	tegra_gpio_mask_write(GPIO_MSK_INT_ENB(gpio), gpio, 1);
}

#define IRQ_TYPE_NONE		0x00000000	/* Default, unspecified type */
#define IRQ_TYPE_EDGE_RISING	0x00000001	/* Edge rising type */
#define IRQ_TYPE_EDGE_FALLING	0x00000002	/* Edge falling type */
#define IRQ_TYPE_EDGE_BOTH (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH	0x00000004	/* Level high type */
#define IRQ_TYPE_LEVEL_LOW	0x00000008	/* Level low type */
#define IRQ_TYPE_SENSE_MASK	0x0000000f	/* Mask of the above */
#define IRQ_TYPE_PROBE		0x00000010	/* Probing in progress */

#define ACK_TYPE_NONE	0
#define ACK_TYPE_UNMASK	1

static inline void __set_irq_handler_unlocked(int irq,
					      irq_handler_t handler, int level)
{
	struct irqdesc *desc;
	irq_guest_action_t  *action;
	unsigned long       flags;
	
	desc = get_irq_descriptor(irq);

	//spin_lock_irqsave(&desc->lock, flags);
	desc->handle = handler;


	if(level){
		desc->chip->trigger_type=LEVEL_STRING;
		action = (irq_guest_action_t *)desc->action;
		if(action)
			action->ack_type= ACK_TYPE_UNMASK;
	}
	else{
		desc->chip->trigger_type=EDGE_STRING;
		action = (irq_guest_action_t *)desc->action;
		if(action)
			action->ack_type= ACK_TYPE_NONE;
	}
	//spin_unlock_irqrestore(&desc->lock, flags);
	
}

static int tegra_gpio_irq_set_type(unsigned int irq, unsigned int type)
{
#if 1
	int gpio = irq - INT_GPIO_BASE;
	struct tegra_gpio_bank *bank = get_irq_chip_data(irq);
	int port = GPIO_PORT(gpio);
	int lvl_type;
	int val;
	unsigned long flags;

	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		lvl_type = GPIO_INT_LVL_EDGE_RISING;
		break;

	case IRQ_TYPE_EDGE_FALLING:
		lvl_type = GPIO_INT_LVL_EDGE_FALLING;
		break;

	case IRQ_TYPE_EDGE_BOTH:
		lvl_type = GPIO_INT_LVL_EDGE_BOTH;
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		lvl_type = GPIO_INT_LVL_LEVEL_HIGH;
		break;

	case IRQ_TYPE_LEVEL_LOW:
		lvl_type = GPIO_INT_LVL_LEVEL_LOW;
		break;

	default:
		return -EINVAL;
	}

	spin_lock_irqsave(&bank->lvl_lock[port], flags);

	val = mmio_readl(GPIO_INT_LVL(gpio));
	val &= ~(GPIO_INT_LVL_MASK << GPIO_BIT(gpio));
	val |= lvl_type << GPIO_BIT(gpio);
	mmio_writel( val, GPIO_INT_LVL(gpio));

	spin_unlock_irqrestore(&bank->lvl_lock[port], flags);

	if (type & (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH))
		__set_irq_handler_unlocked(irq, level_irq_handler,1);
	else if (type & (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING))
		__set_irq_handler_unlocked(irq, edge_irq_handler,0);
#endif
	return 0;
}

#define ARCH_NR_GPIOS		INT_GPIO_NR

static struct irqchip tegra_gpio_irq_chip = {
	.trigger_type		= "level",
	.ack        = tegra_gpio_irq_ack,
	.mask       = tegra_gpio_irq_mask,
	.unmask     = tegra_gpio_irq_unmask,
	.set_type   = tegra_gpio_irq_set_type,
};



# define lockdep_set_class(lock, key)		do { (void)(key); } while (0)
struct lock_class_key { };
static struct lock_class_key gpio_lock_class;

static int tegra_gpio_compose(int bank, int port, int bit)
{
	return (bank << 5) | ((port & 0x3) << 3) | (bit & 0x7);
}

static inline int gpio_to_irq(unsigned int gpio)
{
	if (gpio < ARCH_NR_GPIOS)
		return INT_GPIO_BASE + gpio;
	return -EINVAL;
}


static void tegra_gpio_irq_handler(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs * reg)
{
	struct tegra_gpio_bank *bank;
	int port;
	int pin;
	int unmasked = 0;

	desc->chip->ack(irq);

	bank = get_irq_data(irq);

	for (port = 0; port < 4; port++) {
		int gpio = tegra_gpio_compose(bank->bank, port, 0);
		u8 sta = mmio_readl(GPIO_INT_STA(gpio)) &
			mmio_readl(GPIO_INT_ENB(gpio));
		u32 lvl = mmio_readl(GPIO_INT_LVL(gpio));

		for (pin = 0; pin < 8; pin++) {
			if (sta & (1 << pin)) {
				mmio_writel(1 << pin,
						 GPIO_INT_CLR(gpio));

				/* if gpio is edge triggered, clear condition
				 * before executing the hander so that we don't
				 * miss edges
				 */
				if (lvl & (0x100 << pin)) {
					unmasked = 1;
					desc->chip->unmask(irq);
				}

				generic_handle_irq(gpio_to_irq(gpio + pin));
			}
		}
	}
	if (!unmasked)
		desc->chip->unmask(irq);

}

static void tegra250_irq_init()
{
	struct tegra_gpio_bank *bank;
	int i;
	int j;
	unsigned long phys;
	
	gic_cpu_base_addr = IO_ADDRESS(TEGRA250_GIC_CPU_BASE);
	for (i=0; i<ARRAY_SIZE(tegra_chip); i++) {
		tegra_chip[i].irq_start = INT_PRI_BASE + INT_SYS_SZ*i;
		tegra_chip[i].mmio = IO_ADDRESS(INT_PPI_ADDRESS(i));
		mmio_writel(~0ul, tegra_chip[i].mmio + ICTLR_CPU_IER_CLR_0);
		mmio_writel(0, tegra_chip[i].mmio + ICTLR_CPU_IEP_CLASS_0);
	}
	gic_dist_init(0, IO_ADDRESS(GIC_DIST_BASE), 29);
	gic_cpu_init(0, IO_ADDRESS(GIC_PROC_IF_BASE));

	for (i=INT_PRI_BASE; i<INT_GPIO_BASE; i++) {
		set_irq_chip(i, &tegra_irq);
		set_irq_handler(i, level_irq_handler);
		set_irq_flags(i, IRQF_VALID);
	}

	apbdma_chip.mmio = IO_ADDRESS(INT_APBDMA_ADDRESS);
	spin_lock_init(&apbdma_chip.lock);
	apbdma_chip.irq_start = INT_APBDMA_BASE;

	for (i=INT_APBDMA_BASE; i<INT_APBDMA_NR+INT_APBDMA_BASE; i++) {
		set_irq_chip(i, &apbdma_irq);
		set_irq_chip_data(i, &apbdma_chip);
		set_irq_handler(i, level_irq_handler);
		set_irq_flags(i, IRQF_VALID);
	}

	set_irq_data(INT_APB_DMA, &apbdma_chip);
	set_irq_chained_handler(INT_APB_DMA, apbdma_cascade);
	
	add_gpio_base = (unsigned long)IO_ADDRESS(0x6000d000);

	for (i = 0; i < ARRAY_SIZE(tegra_gpio_banks); i++) {
		for (j = 0; j < 4; j++) {
			int gpio = tegra_gpio_compose(i, j, 0);
			mmio_writel(0x00, GPIO_INT_ENB(gpio));
		}
	}

	for (i = INT_GPIO_BASE; i < (INT_GPIO_BASE + ARCH_NR_GPIOS); i++) {
		bank = &tegra_gpio_banks[GPIO_BANK(i-INT_GPIO_BASE)];
		set_irq_chip(i, &tegra_gpio_irq_chip);
		set_irq_handler(i, level_irq_handler);
		set_irq_chip_data(i, bank);
		set_irq_flags(i, IRQF_VALID);
	}	

	for (i = 0; i < ARRAY_SIZE(tegra_gpio_banks); i++) {
		bank = &tegra_gpio_banks[i];

		set_irq_chained_handler(bank->irq, tegra_gpio_irq_handler);
		set_irq_data(bank->irq, bank);

		spin_lock_init(&bank->lvl_lock[0]);
		spin_lock_init(&bank->lvl_lock[1]);
		spin_lock_init(&bank->lvl_lock[2]);
		spin_lock_init(&bank->lvl_lock[3]);
	}
}

#define CLK_RST_CONTROLLER_RST_CPU_CMPLX_CLR_0  0x344
#define CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0      0x4c
#define CPU_CLK_STOP(cpu)                       (0x1<<(8+cpu))
#define CPU_RESET(cpu)                          (0x1011ul<<(cpu))

#define EVP_CPU_RESET_VECTOR_0          (0x100)
#define FLOW_CTRL_HALT_CPUx_EVENTS(cpu) ((cpu)?((cpu-1)*0x8 + 0x14) : 0x0)


volatile int tegra250_core_map = 1;

int wakeup_cpu(unsigned int cpu)
{
        tegra250_core_map |= 1 <<  cpu;

        cpu_flush_cache_all();

	send_ipi(cpumask_of_cpu(cpu), 1);

        return 0;
}

void platform_secondary_init(unsigned int cpu)
{
        gic_cpu_init(0, IO_ADDRESS(TEGRA_GIC_PROC_IF_BASE));
}


extern void secondary_startup(void);

static void tegra250_evp_init(void)
{
	unsigned long r;
	unsigned long orig_reset;
	unsigned long loop;
	unsigned long ctrl;

	/* Initialize Snoop Control Unit */
	ctrl = mmio_readl(IO_ADDRESS(TEGRA_SCU_BASE) + SCU_CONTROL_0);
	ctrl |= 1;
	mmio_writel(ctrl, IO_ADDRESS(TEGRA_SCU_BASE) + SCU_CONTROL_0);

	orig_reset = mmio_readl(IO_ADDRESS(TEGRA_EXCEPTION_VECTORS_BASE) + EVP_CPU_RESET_VECTOR_0);

	/* Set boot entry */
	mmio_writel(virt_to_phys(secondary_startup),
	IO_ADDRESS(TEGRA_EXCEPTION_VECTORS_BASE) + EVP_CPU_RESET_VECTOR_0);

	dsb();
	isb();

	/* Halt CPU */
	mmio_writel(0, IO_ADDRESS(TEGRA_FLOW_CTRL_BASE) + FLOW_CTRL_HALT_CPUx_EVENTS(1));

	dsb();
	isb();

	/* CPU Clock Stop */
	r = mmio_readl(IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0);
	r &= ~CPU_CLK_STOP(1);
	mmio_writel(r, IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0);

	dsb();
	isb();

	/* Restart Slave CPU */
	mmio_writel(CPU_RESET(1), IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_RST_CPU_CMPLX_CLR_0);

	dsb();
	isb();

#if 0
	/* Wait util the power unit is in stable */
	loop = 10000;
	while((--loop) > 0 );

	/* Restore reset vector */
	mmio_writel(orig_reset, IO_ADDRESS(TEGRA_EXCEPTION_VECTORS_BASE) + EVP_CPU_RESET_VECTOR_0);
	dsb();
	isb();
#endif
}

struct platform tegra250 = {
};

static struct platform *tegra250_platform_setup(void *param)
{
	smp_init_cpus(2);

	tegra250_evp_init();

	tegra250_memory_init();

	tegra250_uart_init();

	tegra250_irq_init();

	tegra250_timer_init();

	return &tegra250;

}

DECLARE_PLATFORM_OP(platform_setup, tegra250_platform_setup);
