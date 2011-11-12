#ifndef __ARM_COPROCESSOR_H__
#define __ARM_COPROCESSOR_H__

#define p14     14
#define p15     15
#define c0      0
#define c1      1
#define c2      2
#define c3      3
#define c4      4
#define c5      5
#define c6      6
#define c7      7
#define c8      8
#define c9      9
#define c10     10
#define c11     11
#define c12     12
#define c13     13
#define c14     14
#define c15     15

#define MCR(cp,op1,Rd,CRn,CRm,op2)  \
	asm(" MCR " #cp",%1,%2,"#CRn","#CRm ",%5" \
	: : "i" (cp), "i" (op1), "r" (Rd), "i" (CRn), "i" (CRm), "i" (op2))

#define MRC(cp,op1,Rd,CRn,CRm,op2)  \
	asm( " MRC " #cp",%2,%0," #CRn","#CRm",%5" \
	: "=r" (Rd) : "i" (cp), "i" (op1), "i" (CRn), "i" (CRm), "i" (op2))

#endif

