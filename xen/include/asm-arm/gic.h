#ifndef __GIC_H__
#define __GIC_H__

/* Distributor Register Map */
#define ICDDCR          0x000  /* Distributor Control Register */
#define ICDICTR         0x004  /* Interrupt Controller Type Register */
#define ICDIIDR         0x008  /* Distributor Implementer Id Register */
#define ICDISR0         0x080  /* Interrupt Security Register */
#define ICDISR1         0x084  /* Interrupt Security Register */
#define ICDISR2         0x088  /* Interrupt Security Register */
#define ICDISR3         0x08c  /* Interrupt Security Register */
#define ICDISER         0x100  /* Interrupt Set-Enable Register */
#define ICDICER         0x180  /* Interrupt Clear-Enable Register */
#define ICDISPR         0x200  /* Interrupt Set-Pending Register */
#define ICDICPR         0x280  /* Interrupt Clear-Pending Register */
#define ICDABR          0x300  /* Active Bit Registers */
#define ICDIPR          0x400  /* Interrupt Priority Register */
#define ICDIPTR         0x800  /* Interrupt Processor Targets Registers */
#define ICDICFR         0xC00  /* Interrupt Configuration Registers */
#define ICDSGIR         0xF00  /* Software Generated Interrupt Register */

/* CPU Interface Register Map */
#define ICCICR          0x000  /* CPU Interface Control Register */
#define ICCPMR          0x004  /* Interrupt Priority Mask Register */
#define ICCBPR          0x008  /* Binrary Point Register */
#define ICCIAR          0x00C  /* Interrupt Acknowledge Register */
#define ICCEOIR         0x010  /* End of Interrupt Register */
#define ICCRPR          0x014  /* Running Priority Register */
#define ICCHPIR         0x018  /* HIghest Pending Interrupt Register */
#define ICCABPR         0x01C  /* Aliased Binary Point Register */
#define ICCIIDR         0x0FC  /* CPU Interface Id Register */

#ifndef __ASSEMBLY__
void gic_dist_init(unsigned int gic_nr, void *base, unsigned int irq_start);
void gic_cpu_init(unsigned int gic_nr, void *base);
void gic_cascade_irq(unsigned int gic_nr, unsigned int irq);
void gic_raise_softirq(cpumask_t cpumask, unsigned int irq);
void gic_init_irq(void);

extern void *gic_cpu_base_addr;
extern void *gic_dist_base_addr;
#endif

#endif
