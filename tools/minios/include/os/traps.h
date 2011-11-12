#ifndef __TRAPS_H__
#define __TRAPS_H__

#include <os/os.h>
#include <os/traps.h>
struct pt_regs {
        unsigned long r0;
        unsigned long r1;
        unsigned long r2;
        unsigned long r4;
        unsigned long r5;
        unsigned long r6;
        unsigned long r7;
        unsigned long r8;
        unsigned long r9;
        unsigned long r10;
        unsigned long r11;
        unsigned long r12;
        unsigned long sp;
        unsigned long lr;
        unsigned long pc;
        unsigned long cpsr;
        unsigned long spsr;
};

void trap_init(void);

#endif
