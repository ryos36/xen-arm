#ifndef __ARM_INIT_H__
#define __ARM_INIT_H__

#ifndef __ASSEMBLY__
/*
 * Mark functions and data as being only used at initialization
 * or exit time.
 */
#define __init       \
    __attribute__ ((__section__ (".init.text")))
#if 0
#define __exit       \
    __attribute_used__ __attribute__ ((__section__(".text.exit")))
#endif

#define __initdata   \
    __attribute__ ((__section__ (".init.data")))
#if 0
#define __exitdata   \
    __attribute_used__ __attribute__ ((__section__ (".data.exit")))
#endif
#define __initsetup  \
    __attribute_used__ __attribute__ ((__section__ (".init.setup")))
#define __init_call  \
    __attribute_used__ __attribute__ ((__section__ (".initcall.init")))
#define __page_aligned \
    __attribute_used__ __attribute__ ((__section__ (".bss.page_aligned")))
#if 0
#define __exit_call  \
    __attribute_used__ __attribute__ ((__section__ (".exitcall.exit")))
#endif
#endif

#endif /* _XEN_ASM_INIT_H */
