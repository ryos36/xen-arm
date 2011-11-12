#ifndef _TYPES_H_
#define _TYPES_H_

#include <xen/interface/types.h>
#include <asm/types.h>

#define BITS_TO_LONGS(bits) \
	(((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)

#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

#ifndef NULL
#define NULL ((void *)0)
#endif

#define INT_MAX         ((int)(~0U>>1))
#define INT_MIN         (-INT_MAX - 1)
#define UINT_MAX        (~0U)
#define LONG_MAX        ((long)(~0UL>>1))
#define LONG_MIN        (-LONG_MAX - 1)
#define ULONG_MAX       (~0UL)

typedef unsigned char           u_char;
typedef unsigned short          u_short;
typedef unsigned int            u_int;
typedef unsigned long           u_long;

/* sysv */
typedef unsigned char           unchar;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;

typedef		__u8		u8;
typedef         __u8            uint8_t;
typedef         __u8            u_int8_t;
typedef		__s8		s8;
typedef         __s8            int8_t;

typedef		__u16		u16;
typedef         __u16           uint16_t;
typedef         __u16           u_int16_t;
typedef		__s16		s16;
typedef         __s16           int16_t;

typedef		__u32		u32;
typedef         __u32           uint32_t;
typedef         __u32           u_int32_t;
typedef		__s32		s32;
typedef         __s32           int32_t;

typedef		__u64		u64;
typedef         __u64           uint64_t;
typedef         __u64           u_int64_t;
typedef		__s64		s64;
typedef         __s64           int64_t;


typedef long long		quad_t;
typedef unsigned long long	u_quad_t;
typedef unsigned int		uintptr_t;

typedef struct { 
	unsigned long pte_low; 
} pte_t;



#endif /* _TYPES_H_ */
