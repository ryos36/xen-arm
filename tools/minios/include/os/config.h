#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifdef __ASSEMBLY__
#  ifndef __ALIGN
#    define __ALIGN     .align 4,0x90
#    define __ALIGN_STR ".align 4,0x90"
#  endif

#  define ALIGN __ALIGN
#  define ALIGN_STR __ALIGN_STR

#  define ENTRY(name) \
	.globl name; \
	ALIGN; \
	name:
#endif
#endif
