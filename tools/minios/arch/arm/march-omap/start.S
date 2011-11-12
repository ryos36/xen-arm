#include <os/config.h>

.section __xen_guest
	.ascii	"GUEST_OS=Mini-OS"
	.ascii	",XEN_VER=xen-3.0"
	.ascii	",LOADER=generic"
	.ascii	",PT_MODE_WRITABLE"

	.text
ENTRY(_start)

	/*
	 * Stack Setup for User Mode
	 * This code will cause abort exception
	 */
	ldr	sp, __init_sp 
	
	mov	r0, r12

	b	start_kernel

__init_sp:
	.long __stack_end + 0x1000

        .org 0x1000
ENTRY(shared_info)
        .fill 0x8000,1,0    @ 32KB

.section ".bss.stack_aligned","w"
__stack_end:
        .fill 0x1000,1,0  @ 8KB 

