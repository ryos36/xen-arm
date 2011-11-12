#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef NDEBUG
#define DEBUG(x...)	printf( "[DOM_CREATE]" x)
#else
#define DEBUG(x...) 
#endif

#define PANIC		printf
#endif

