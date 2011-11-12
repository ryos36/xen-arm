/**

 *	Samsung Software Center Common Crypto Module

 *	THIS FILE DEFINES THE COMMON CRYPTO LIBRARY CONFIGURATION. IF YOU USE THIS

 *	CRYPTO LIBRARY THE OTHER SECURITY LIBRARY, THEN MODIFY THIS FILE DEFINITIONS.

 *	ORIGINALLY, THE FOLLOWING CONFIGURATION PART WAS PUT IN EVP.H, BUT CREATED 

 *	FOR THE CONVENIENCE OF CONFIGURE BY THE PROJECT.

 *

 *	PLEASE READ FOLLOWING COMMENTS

 *

 *	@author		Tae-Sung Kim (crypto89@samsung.com)

 */

#ifndef 	_CRYPTO_CONFIG_H

#define 	_CRYPTO_CONFIG_H



#ifdef	__cplusplus

extern "C" {

#endif



#ifdef WIN32

#include <stdio.h> // for NULL

#endif





/**

 *	2001.07.09 Commented by Taesung Kim.

 *	

 *	AS YOUR PROJECT, YOU ONLY HAVE TO DEFINE A PART OF THE

 *	FOLLOWING DEFINITIONS.

 *

 *	###########################################################

 *	HASH ALGORITHMS

 *		ISEC_API_SHA1

 *		ISEC_API_MD5

 *	###########################################################	

 *	SYMETRIC ALGORITHMS

 *		ISEC_API_DES

 *		ISEC_API_RC5

 *	###########################################################	

 *	ASYMETRIC ALGORITHMS

 *		ISEC_API_RSA

 *		ISEC_API_ECC

 *	###########################################################	

 *	KEY EXCHANGE ALGORITHMS

 *		ISEC_API_DH

 *		ISEC_API_ECDH

 *	###########################################################	

 *	HMAC ALGORITHMS

 *		ISEC_API_HMAC

 *	###########################################################	

 *	ISEC_API_ECC or ISEC_API_ECDH is defined.

 *		ISEC_CRAPI_EC_CURVE_1

 *		...

 *		ISEC_CRAPI_EC_CURVE_35

 *

 *	FOR EXAMPLE, the following  header file <ISECConfig.h> has 

 *	above definitions.

 */



#include <ISECConfig.h>

/**

 *	2001.07.09 Commented by Taesung Kim.

 *

 *	AS ABOVE COMMENT, BEFORE DEFINITION, YOU MAY DEFINE THE

 *	DEFINITIONS AND INCLUDE HEADER FILES WHICH IS SPECFIC YOUR PROJECT

 *

 *	FOR EXAMPLE,

 *	In <ISECConfig.h> which is for SHP project, you include

 *	<shp.h> and <shpconfig.h> and definition

 *	#if (defined(_SHP_PROTO_WAP) && defined(_SHP_PROTO_WTLS)) || (defined(_SHP_PROTO_INTERNET) && defined(_SHP_PROTO_SSL))

 *	If the above definitions are not defined, then no crypto module is contained in your module.

 */





#define CRYPTO_LITTLE_ENDIAN

/**
 *	######################################
 *            Standard library
 *             depends on OS
 *  ######################################
 *
 *	2007.02.23 Added by Sangsin Jung
 */
#define UNIX
#define XEN


/**

 *	######################################

 *				TYPE CONTROL

 *	######################################

 *	2001.07.09 Commented by Taesung Kim.

 *

 *	If you want your type definition, then you undef

 *	ISEC_TYPE_DEFINITION  and define your type defintion

 *	in <ISECConfig.h>

 *

 */

#ifndef ISEC_TYPE_DEFINITION

#define ISEC_TYPE_DEFINITION



typedef 	unsigned char		CrUINT8;

typedef 	signed char			CrINT8;

typedef 	unsigned short		CrUINT16;

typedef 	short				CrINT16;

typedef 	unsigned int		CrUINT32;

typedef 	int					CrINT32;

typedef 	unsigned long		CrULONG32;

typedef 	long				CrLONG32;



#ifdef WIN32



typedef 	unsigned _int64		CrULONG64;

typedef 	_int64				CrLONG64;



#else



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



#endif // #ifdef ISEC_TYPE_DEFINITION





#ifndef NOPROTO

#define P_CC_CC			const void *,const void *

#define P_I_I			CrINT32,CrINT32 

#define P_I_I_P			CrINT32,CrINT32,CrINT8 *

#define P_I_I_P_I		CrINT32,CrINT32,CrINT8 *,CrINT32

#define P_IP_I_I_P_I	CrINT32 *,CrINT32,CrINT32,CrINT8 *,CrINT32

#define P_V				void 

#else

#define P_CC_CC

#define P_I_I

#define P_I_I_P

#define P_IP_I_I_P_I

#define P_I_I_P_I

#define P_V

#endif





/**

 *##########################################

 *				ERROR CONTROL

 *##########################################

 */

//#define SSLEAY_ERROR

#ifndef NO_SSLEAY_ERROR

#define NO_SSLEAY_ERROR

#endif // #ifndef NO_SSLEAY_ERROR



/**

 *##########################################

 *				RANDOM CONTROL

 *##########################################

 */

#ifdef ISEC_API_SWC_RANDOM_NUMBER

#define SWC_RANDOM_NUMBER

/**

 *	If you have the other random like random generation algorithm

 *	by using RC5, then you UNDEFINITION following DES_RANDOM.

 */

//#define DES_RANDOM

//#define RC5_RANDOM

#define AES_RANDOM



#endif // #ifndef ISEC_API_SWC_RANDOM_NUMBER







#define CR_UINT8_SIZE		sizeof(CrINT8)

#define CR_UINT16_SIZE		sizeof(CrINT16)

#define CR_UINT32_SIZE		sizeof(CrINT32)



typedef CrINT16				CrResult;

typedef CrUINT16			CrValueSizeType;

typedef struct tagCrOctet

{

	CrValueSizeType			valueSize;

	CrUINT8*				pValue;

}CrOctet;



/* ============= HASH ALGORITHM ============= */

#ifdef ISEC_API_SHA1

#	define CR_SHA

#	define CR_SHA1

#endif

#ifdef ISEC_API_MD5

#	define CR_MD5

#endif

#ifdef ISEC_API_MD2

#	define CR_MD2

#endif

#ifdef ISEC_API_SHA256

#	define CR_SHA256

#endif

#ifdef ISEC_API_SHA384

#	define CR_SHA384

#endif

#ifdef ISEC_API_SHA512

#	define CR_SHA512

#endif







#if defined(ISEC_API_ECDSA) || defined(ISEC_API_ECES) || defined(ISEC_API_ECDH)

#define CR_ECC

#endif





/* ========= KEY EXCHANGE ALGORITHM ========= */

#ifdef ISEC_API_DH

#define CR_DH

#endif

#ifdef ISEC_API_ECDH

#define CR_ECDH

#endif





/* ========== ASYMETRIC ALGORITHM ========== */

#ifdef ISEC_API_RSA

#	define CR_RSA

#endif

#ifdef ISEC_API_ECDSA

#	define CR_ECDSA // ECC curve에 대한 선택은 CrECConfig.h

#endif

#ifdef ISEC_API_ECES

#	define CR_ECES

#	define CR_ECAES

#endif





/* =========== SYMETRIC ALGORITHM =========== */

#ifdef ISEC_API_DES

#	define CR_DES

#endif

#ifdef ISEC_API_RC4

#	define CR_RC4 // added RC4 2001.11.19

#endif

#ifdef ISEC_API_RC5

#	define CR_RC5

#endif

#ifdef ISEC_API_SEED

#	define CR_SEED

#endif

#ifdef ISEC_API_AES

#	define CR_AES

#endif

#ifdef ISEC_API_IDEA

#	define CR_IDEA

#endif

#ifdef ISEC_API_RC2

#	define CR_RC2

#endif







/* ========== MAC ALGORITHM ========== */

#ifdef ISEC_API_HMAC

#	define CR_HMAC

#	ifdef CR_HMAC

#		define CR_HMAC_SHA1

//#		define CR_HMAC_MD5

#	endif

#endif







#ifdef  __cplusplus

}

#endif



#endif //#ifndef _CRYPTO_CONFIG_H



