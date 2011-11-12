#ifndef __ARM_BUG_H__
#define __ARM_BUG_H__

#define BUGFRAME_dump   0
#define BUGFRAME_warn   1
#define BUGFRAME_bug    2
#define BUGFRAME_assert 3

#define BUG()							\
	do {							\
		printk("BUG at %s:%d\n", __FILE__, __LINE__);	\
		while(1);					\
	} while ( 0 )

#define PANIC(msg)						\
	do {							\
		printk("Panic at %s:%d\n", __FILE__, __LINE__); \
		while(1);                                                                                       \
	}while (0)

struct bug_frame {
	unsigned char ud2[2];
	unsigned char ret;
	unsigned short id; /* BUGFRAME_??? */
};


#endif /* __ARM_BUG_H__ */

