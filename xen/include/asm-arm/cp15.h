/*
 * cp15.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          Jaemin Ryu      <jm77.ryu@samsung.com>
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

#ifndef __ARM_CP15_H__
#define __ARM_CP15_H__

/*
 * Co-Processor Access Register
 */
#define CPAR_BIT_CP0	(1 << 0)
#define CPAR_BIT_CP1	(1 << 1)
#define CPAR_BIT_CP2	(1 << 2)
#define CPAR_BIT_CP3	(1 << 3)
#define CPAR_BIT_CP4	(1 << 4)
#define CPAR_BIT_CP5	(1 << 5)
#define CPAR_BIT_CP6	(1 << 6)
#define CPAR_BIT_CP7	(1 << 7)
#define CPAR_BIT_CP8	(1 << 8)
#define CPAR_BIT_CP9	(1 << 9)
#define CPAR_BIT_CP10	(1 << 10)
#define CPAR_BIT_CP11	(1 << 11)
#define CPAR_BIT_CP12	(1 << 12)
#define CPAR_BIT_CP13	(1 << 13)

/* CRn=c0, OP1=0, CRm=c0 */
#define CP15_ID_MIDR		0	/* Main ID Register */
#define CP15_ID_CTR		1	/* Cache Type Register */
#define CP15_ID_TCMTR		2	/* TCM Type Register : IMPLEMENTATION DEFINED*/
#define CP15_ID_TLBTR		3	/* TLB Type Register : IMPLEMENTATION DEFINED */
#define CP15_ID_MPIDR		4	/* Multiprocessor Affinity Register */
#define CP15_ID_CPUIDR		5	/* IMPLEMENTATION DEFINED */

/* crN=c0, OP1=0, CRm=c1 */
#define CP15_ID_PFR0		0	/* Processor Feature Register 0 */
#define CP15_ID_PFR1		1	/* Processor Feature Register 1 */
#define CP15_ID_DFR0		2	/* Debug Feature Register 0 */
#define CP15_ID_ADR0		3	/* Auxiliary Feature Register 0 */
#define CP15_ID_MMFR0		4	/* Memory Model Feature Register 0 */
#define CP15_ID_MMFR1		5	/* Memory Model Feature Register 1 */
#define CP15_ID_MMFR2		6	/* Memory Model Feature Register 2 */
#define CP15_ID_MMFR3		7	/* Memory Model Feature Register 3 */

/* CRn=c0, OP1=0, CRm=c2 */
#define CP15_ID_ISAR0		0	/* ISA Feature Register 0 */
#define CP15_ID_ISAR1		1	/* ISA Feature Register 1 */
#define CP15_ID_ISAR2		2	/* ISA Feature Register 2 */
#define CP15_ID_ISAR3		3	/* ISA Feature Register 3 */
#define CP15_ID_ISAR4		4	/* ISA Feature Register 4 */
#define CP15_ID_ISAR5		5	/* ISA Feature Register 5 */

/* CRn=c0, OP1=1, CRm=c0 */
#define CP15_ID_CCSIDR		0	/* Cache Size ID Register */
#define CP15_ID_CLIDR		1	/* Cache Level ID Register */
#define CP15_ID_AIDR		7	/* Auxiliary ID Register : IMPLEMENTATION DEFINED */

/* CRn=c0, OP1=2, CRm=c0 */
#define CP15_ID_CSSELR		0	/* Cache Size Selection  Register */

/* CRN=c1, OP1=0, CRm=c0 */
#define CP15_SCTLR		0	/* Control Register */
#define CP15_ACTLR		1	/* Auxiliary Control Register : IMPLEMENTATION DEFINED */
#define CP15_CPACR		2	/* Coprocessor Access Register */


/* CRN=c1, OP1=0, CRm=c1 */
#define CP15_SCR		0	/* Secure Configuration Register */
#define CP15_SDER		1	/* Secure Debug Enable Register */
#define CP15_NSACR		2	/* Non-secure Access Control Register */


#endif /* __ARM_CP15_H__ */

