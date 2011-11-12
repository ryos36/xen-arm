#ifndef 	_ISEC_CONFIG_HEADER_

#define 	_ISEC_CONFIG_HEADER_



#ifdef	__cplusplus

extern "C" {

#endif











/**

 *	###########################################################################

 *								TYPE CONTROL

 *	###########################################################################

 */



#ifdef WIN32	/* by Sangsin Jung */
#include <stdio.h>
#else	/* OSV_XEN */
#include <xen/types.h>
#endif







/**

 *	###########################################################################

 *								CIPHER SELECTION

 *	###########################################################################

 */

#define ISEC_API_ASYMMETRIC_CIPHER

#define ISEC_API_SYMMETRIC_CIPHER

#define ISEC_API_HASH

//#define ISEC_API_PRF

#define ISEC_API_SWC_RANDOM_NUMBER







// ######################## ASYMMETRIC CIPHER SELECTION. ######################

#ifdef ISEC_API_ASYMMETRIC_CIPHER

//#	define ISEC_API_DH

#	define ISEC_API_RSA

//#	define ISEC_API_ECDSA

//#	define ISEC_API_ECES

//#	define ISEC_API_ECDH

#endif







// ######################## SYMMETRIC CIPHER SELECTION. #######################

#ifdef ISEC_API_SYMMETRIC_CIPHER

#	define ISEC_API_BLOCK_CIPHER

//#	define ISEC_API_STREAM_CIPHER

#endif

//			BLOCK CIPHER

#ifdef ISEC_API_BLOCK_CIPHER

//#	define ISEC_API_DES

//#	define ISEC_API_RC2

//#	define ISEC_API_RC5

//#	define ISEC_API_SEED

#	define ISEC_API_AES

//#	define ISEC_API_IDEA

#endif

//			STREAM CIPHER

#ifdef ISEC_API_STREAM_CIPHER

#	define ISEC_API_RC4

#endif









// ######################### HASH ALGORITHM SELECTION. ########################

#ifdef ISEC_API_HASH

#	define ISEC_API_SHA1

//#	define ISEC_API_MD5

//#	define ISEC_API_MD2

//#	define ISEC_API_SHA256

//#	define ISEC_API_SHA384

//#	define ISEC_API_SHA512

#	define ISEC_API_HMAC

#endif











#ifdef  __cplusplus

}

#endif



#endif // end of #ifndef 	_ISEC_CONFIG_HEADER_







