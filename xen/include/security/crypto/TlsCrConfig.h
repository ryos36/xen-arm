/**

 * 프로젝트에 따른 configuration설정.

 * 

 */



//#include <Shp.h>//TAE-SUNG KIM

//#include <ShpConfig.h>

//#if (defined(_SHP_PROTO_WAP) && defined(_SHP_PROTO_WTLS)) || (defined(_SHP_PROTO_INTERNET) && defined(_SHP_PROTO_SSL))



#ifndef _TLS_CR_CONFIG_H

#define _TLS_CR_CONFIG_H



#include <ISECConfig.h>
#define WTLS





/**

 *	###########################################################################

 *								DATA TYPE CONTROL

 *	###########################################################################

 */

#ifndef ISEC_TYPE_DEFINITION

#define ISEC_TYPE_DEFINITION



typedef 	unsigned char		CrUINT8;

typedef 	char				CrINT8;

typedef 	unsigned short		CrUINT16;

typedef 	short				CrINT16;

typedef 	unsigned int		CrUINT32;

typedef 	int					CrINT32;

typedef 	unsigned long		CrULONG32;

typedef 	long				CrLONG32;



#ifdef WIN32



typedef 	unsigned _int64		CrULONG64;

typedef 	_int64				CrLONG64;



#else // #ifdef WIN32



typedef 	unsigned long long	CrULONG64;

typedef 	long long			CrLONG64;



#endif // #ifdef WIN32



typedef	CrINT32		CrBOOL;

#ifndef TRUE

#define TRUE			1

#endif

#ifndef FALSE

#define	FALSE			0

#endif



#endif // end of #ifndef ISEC_TYPE_DEFINITION



typedef CrINT16						TLSCrResult;

typedef CrUINT16					TLSModulusBitsSizeType;













/**

 *	###########################################################################

 *								CRYPTO API MODULE SETTING

 *	###########################################################################

 */

#ifdef ISEC_API_ASYMMETRIC_CIPHER

#	define TLS_API_ASYMMETRIC_CIPHER

#endif

#ifdef ISEC_API_DH

#	define TLS_API_DH

#endif

#ifdef ISEC_API_RSA

#	define TLS_API_RSA

#endif

#ifdef ISEC_API_ECDSA

#	define TLS_API_ECDSA

#endif

#ifdef ISEC_API_ECDH

#	define TLS_API_ECDH

#endif

#ifdef ISEC_API_ECES

#	define TLS_API_ECES

#endif

#if defined(TLS_API_ECDSA) || defined(TLS_API_ECDH) || defined(TLS_API_ECES)

#	define TLS_API_ECC

#endif



#ifdef ISEC_API_SYMMETRIC_CIPHER

#	define TLS_API_SYMMETRIC_CIPHER

#endif

#ifdef ISEC_API_BLOCK_CIPHER

#	define TLS_API_BLOCK_CIPHER

#endif

#ifdef ISEC_API_STREAM_CIPHER

#	define TLS_API_STREAM_CIPHER

#endif

#ifdef ISEC_API_DES

#	define TLS_API_DES

#endif

#ifdef ISEC_API_RC5

#	define TLS_API_RC5

#endif

#ifdef ISEC_API_SEED

#	define TLS_API_SEED

#endif

#ifdef ISEC_API_AES

#	define TLS_API_AES

#endif

#ifdef ISEC_API_IDEA

#	define TLS_API_IDEA

#endif



#ifdef ISEC_API_RC4

#	define TLS_API_RC4

#endif



#ifdef ISEC_API_RC2

#	define TLS_API_RC2

#endif





#ifdef ISEC_API_HASH

#	define TLS_API_HASH



#	ifdef ISEC_API_SHA1

#		define TLS_API_SHA1

#	endif

#	ifdef ISEC_API_SHA256

#		define TLS_API_SHA256

#	endif

#	ifdef ISEC_API_SHA384

#		define TLS_API_SHA384

#	endif

#	ifdef ISEC_API_SHA512

#		define TLS_API_SHA512

#	endif

#	ifdef ISEC_API_MD2

#		define TLS_API_MD2

#	endif

#	ifdef ISEC_API_MD5

#		define TLS_API_MD5

#	endif

#	ifdef ISEC_API_HMAC

#		define TLS_API_HMAC

#	endif

#endif



#ifdef ISEC_API_PRF

#	define TLS_API_PRF

#endif



//#define TLS_CR_DEBUG

#ifdef TLS_CR_DEBUG

#define TLS_CR_ECC_FIXED_RAND

#endif



#ifdef TLS_API_ECC

#define TLS_CR_ECC_UNCOMPRESSED

#endif

#ifdef TLS_API_ECDH

#define TLS_CR_ECC_PMS

#endif



#endif // #ifndef _TLS_CR_API_CONFIG_H





//#endif // #if (defined(_SHP_PROTO_WAP) && defined(_SHP_PROTO_WTLS)) || (defined(_SHP_PROTO_INTERNET) && defined(_SHP_PROTO_SSL))


