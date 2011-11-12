#define UNRSV	0xFF

#if defined(CONFIG_MACHINE_IMX21)
#define DEV_UNIT_ADDR_SHIFT	12
#define DEV_UNIT_ADDR_SIZE		(0x1<<DEV_UNIT_ADDR_SHIFT)

#define IMX21_AIPI1			0x0
#define IMX21_DMA 			0x1
#define IMX21_WDOG 			0x2
#define IMX21_GPT1 			0x3
#define IMX21_GPT2			0x4
#define IMX21_GPT3 			0x5
#define IMX21_PWM 			0x6
#define IMX21_RTC 			0x7
#define IMX21_KPP 			0x8
#define IMX21_OWIRE 			0x9
#define IMX21_UART1			0xA
#define IMX21_UART2			0xB
#define IMX21_UART3			0xC
#define IMX21_UART4			0xD
#define IMX21_CSPI1			0xE
#define IMX21_CSPI2			0xF
#define IMX21_SSI1			0x10
#define IMX21_SSI2			0x11
#define IMX21_I2C				0x12
#define IMX21_SDHC1 			0x13
#define IMX21_SDHC2			0x14
#define IMX21_GPIO			0x15
#define IMX21_AUDMUX			0x16
#define IMX21_CSPI3			0x17
#define IMX21_AIPI2			0x20
#define IMX21_LCD 			0x21
#define IMX21_SLCD			0x22
#define IMX21_USBOTG1		0x24
#define IMX21_USBOTG2		0x25
#define IMX21_eMMA 			0x26
#define IMX21_CRM  			0x27
#define IMX21_FIRI 			0x28 
#define IMX21_RNGA   		0x29
#define IMX21_RTIC			0x2A
#define IMX21_JAM   			0x3E 
#define IMX21_MAX     		0x3F
#define IMX21_CS1				0xCC
#define IMX21_CHIPSELECT	0xD1
#define IMX21_PCMCIA			0xD2
#define IMX21_NANDFC			0xD3
#define IMX21_FLASH_MEM		0x50

#elif defined(CONFIG_MACHINE_XSCALE)

#define DEV_UNIT_ADDR_SHIFT	20
#define DEV_UNIT_ADDR_SIZE		(0x1<<DEV_UNIT_ADDR_SHIFT)

#define MIRAGE_DMAC				0x400	//(DCSR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_FFUART			0x401	//(FFUART	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_BTUART			0x402	//(BTUART	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_I2C				0x403	//(IBMR		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_AC97				0x405	//(POCR		>> DEV_UNIT_ADDR_SIZE)		// Audio Codec 97 & WM97(touchscreen)
#define MIRAGE_UDC				0x406	//(UDCCR	>> DEV_UNIT_ADDR_SIZE)		//USB
#define MIRAGE_STUART			0x407	//(STUART	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_RTC				0x409	//(RCNR		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_OSTIMER			0x40A	//(OSMR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_PWM0N2			0x40B	//(PWM_CTRL0 >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_PWM1N3			0x40C	//(PWM_CTRL1 >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_INTC				0x40D	//(ICIP		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_GPIO				0x40E	//(GPLR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_PM					0x40F	//(PMCR		>> DEV_UNIT_ADDR_SIZE)		// Power management
#define MIRAGE_SSP1				0x410	//(SSCR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_MMC_SD_SDIO_1	0x411	//(MMC_STRPCL >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_CLKC				0x413	//(ACCR		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_KEYPAD			0x415	//(KPC		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_USIM1				0x416
#define MIRAGE_SSP2				0x417	//(SSCR0_P2 >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_SSP3				0x419	//(SSCR0_P3 >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_SSP4				0x41A
#define MIRAGE_ONEWIRE			0x41B	//(W1CMDR	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_CIF				0x41D											// Consumer Infrared
#define MIRAGE_MMC_SD_SDIO_2 	0x420	//(MMC_STRPCL >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_USIM2				0x421
#define MIRAGE_MMC_SD_SDIO_3	0x425
#define MIRAGE_DFC				0x431	//(NDCR		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_LCDC				0x440	//(LCCR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_MLCD_PADDR		0x46000000	//(MLCCR0)							// 0x46000000XX
#define MIRAGE_MLCD				0x460	//(MLCCR0)							// 0x46000000XX
#define MIRAGE_ISBA_PMD_PADDR	0x4600FE00	// Handle in supervisor mode. Should not assigned to virtual machines. 
#define MIRAGE_ISBA_PMD			0x461	// Handle in supervisor mode. Should not assigned to virtual machines. 
#define MIRAGE_DMEMC				0x481	//0x(MDCNFG	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_SMEMC				0x4A0	//(MSC0		>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_USBH11			0x4C0	//(UHC_BASE_PHYS >> DEV_UNIT_ADDR_SIZE)	// USB host 1.1
#define MIRAGE_CAMERA			0x500	//(CICR0	>> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_GRPHICS			0x540									// 2D Graphics
#define MIRAGE_USBH20			0x541	//(U2DCR	>> DEV_UNIT_ADDR_SIZE)		// USB host 2.0
#define MIRAGE_ISRAM				0x580	//(SRAM_MEM_PHYS >> DEV_UNIT_ADDR_SIZE)
#define MIRAGE_BOOTROM			0x5E0

#define PXA_CS0					0x1
#define PXA_CS1					0x2
#define PXA_CS2					0x3
#define PXA_CS3					0x4
#define PXA_PCMCIA				0x5
#define PXA_DDR_CS0				0x6
#define PXA_DDR_CS1				0x7
#define PXA_CS0_PADDR			0x00000000 // for both small and large same start
#define PXA_CS0_SIZE				0x10000000
#define PXA_CS1_PADDR			0x30000000 // Large partition start address (256MB)
#define PXA_CS1_SIZE				0x10000000 //256 MB	
#define PXA_CS2_PADDR			0x10000000 // (64MB)
#define PXA_CS2_SIZE				0x4000000
#define PXA_CS3_PADDR			0x14000000 // (64MB)
#define PXA_CS3_SIZE				0x4000000
#define PXA_PCMCIA_PADDR		0x20000000 // (256MB)
#define PXA_PCMCIA_SIZE			0x10000000 //256 MB	

#define PXA_DDR_CS0_PADDR		0x80000000
#define PXA_DDR_CS0_SIZE		0x50000000
#define PXA_DDR_CS1_PADDR		0xC0000000
#define PXA_DDR_CS1_SIZE		0x3C000000

#else
/* Below is default */
#define DEV_UNIT_ADDR_SHIFT	12
#define DEV_UNIT_ADDR_SIZE		(0x1<<DEV_UNIT_ADDR_SHIFT)
#endif

