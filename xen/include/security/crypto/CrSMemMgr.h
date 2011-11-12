/**

 *####################################################################

 *						MEMORY CONFIGURATION

 *####################################################################

 */



#ifndef _S_MEM_MGR_H

#define _S_MEM_MGR_H



/**

 *	1. 2001.07.06 Modified by Taesung Kim [Memory Management]

 *	You may modify this file for your memory management.

 *	In SHP project, always define "_SHP_MEM".

 *	And define "_NO_MEM_MGMT" when testing your module with your PC.

 *	

 *	2. 2001.08.22 Removed by Taesung Kim

 */



//#define _SHP_MEM		// Mem. Mgmt. in SHP Project.

//#define _NO_MEM_MGMT	// Mem. Mgmt. in Local Host.







/**

 *	2001.07.02 Modified by Taesung Kim [ISEC COMPONENT]

 *	The following header file, <MemMgr.h> is only used to 

 *	SHP Project for memory management. 

 *	If you have another memory management files, include memory

 *	management header files after following code.

 */



#ifdef SHP_PROJECT_CRYPTO

#include "MemMgr.h"

#else

// You MAY change these header files, if other project

/** WIN32 Header Files for Memory Functions.

 * memcpy	: memory.h, string.h

 * memmove	: string.h

 * memset	: memory.h, string.h

 * memcmp	: memory.h, string.h

 * 

 * malloc	: stdlib.h, malloc.h

 * realloc	: stdlib.h, malloc.h

 * calloc	: stdlib.h, malloc.h

 * free		: stdlib.h, malloc.h

 */

#ifdef WIN32 /* by Sangsin & Sung-Min */

#include <string.h>

#include <stdlib.h>

#else /* OSV_XEN */

#include <xen/string.h>

#include <xen/xmalloc.h>

#endif	/* @WIN32 */

#endif



#define MEM_CLEAR_CHAR 0x00







/**

 * ======== MEMORY UTIL (COPY, MOVE, SET, COMPARE) ==========

 */

#ifdef SHP_PROJECT_CRYPTO

#	define SMemcpy		AcMemcpy

#	define SMemmove		AcMemmove

#	define SMemset(a,b,c)		AcMemset(a,b,(CrUINT16)(c))

#	define SMemcmp		AcMemcmp

#else // NO MEMORY MANAGEMENT

#	define SMemcpy	memcpy

#	define SMemmove	memmove

#	define SMemset	memset

#	define SMemcmp	memcmp

#endif // end of SHP_PROJECT_CRYPTO



/**

 * =============== MEMORY ALLOC & FREE =================

 */

#ifdef SHP_PROJECT_CRYPTO

#	ifdef _SHP_ECOM_CRYPTO

extern EMemMgr*		pMemMgr;		



#		define SMalloc(a)	pMemMgr->Alloc((CrUINT16)(a))

#		define SRealloc		pMemMgr->ReAlloc

#		define SFree		pMemMgr->Free

#		define SCalloc		pMemMgr->Calloc

#	else // else of _SHP_MEM_ECOM_CRYPTO

#		define SMalloc(a)	MemAlloc((CrUINT16)(a))

#		define SRealloc		MemReAlloc

#		define SFree		MemFree

#		define SCalloc		MemCalloc

#	endif // end of _SHP_MEM_ECOM_CRYPTO



#else // NO MEMORY MANAGEMENT.

#ifdef WIN32

#	define SMalloc	malloc

#	define SRealloc	realloc

#	define SCalloc	calloc

#	define SFree		free

#else // Sung-Min	

#	define SMalloc		xmalloc_bytes

#	define SRealloc		xrealloc

#	define SFree		xfree

#endif



#endif // end of SHP_PROJECT_CRYPTO





CrINT16	SPtrMalloc(void **ppMem, size_t size);

void	SClearFree(void **ppMem, size_t size);





#endif // #ifndef _S_MEM_MGR_H

