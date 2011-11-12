#ifndef __XEN_COMPILER_H__
#define __XEN_COMPILER_H__

#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#define __attribute_used__ __attribute__((__used__))
#else
#define __attribute_used__ __attribute__((__unused__))
#endif

#endif /* __XEN_COMPILER_H__ */
