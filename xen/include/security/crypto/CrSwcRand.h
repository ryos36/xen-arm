/*  If you use getRandomBytes(CrUINT8 *out, CrUINT32 outLen), 

*	include this file.	

*	1998.10.31 created by Jung Kyung Im

*

*/



#ifndef HEADER_SWC_RAND_H

#define HEADER_SWC_RAND_H



#ifdef  __cplusplus

extern "C" {

#endif



#ifndef NOPROTO

CrINT32 getRandomBytes(CrUINT8* out, CrUINT32 outLen);

#else

CrINT32 getRandomBytes( );

#endif



#ifdef  __cplusplus

}

#endif



#endif



