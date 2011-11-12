#ifndef _TLS_CR_KEY_ST_H
#define _TLS_CR_KEY_ST_H

#include "TlsCrConfig.h"



/**
 * #######################################################
 * 		DIFFIE-HELLMAN KEY STRUCTURE DEFINITIONS 
 * #######################################################
 */
#ifdef TLS_API_DH

/* TLS DH Private Key */
typedef	CrUINT8	TLSDHESizeType;
typedef struct	tagTLSDHPrivateKey
{
	TLSDHESizeType		dheSize;
	CrUINT8*			pDHE;
}TLSDHPrivateKey;

/* TLS DH Public Key */
typedef	CrUINT16	TLSDHYSizeType; 
typedef struct	tagTLSDHPublicKey
{
#	define TLS_DHY_SIZE_LOW_BOUND		1
	TLSDHYSizeType		dhySize;
	CrUINT8*			pDHY;
}TLSDHPublicKey;

/* TLS DH PARAMETERS */
typedef CrUINT16	TLSDHPSizeType;
typedef CrUINT16	TLSDHGSizeType;
typedef struct tagTLSDHParameters
{
	TLSDHESizeType		dheSize;

	TLSDHPSizeType		dhpSize;
	CrUINT8*			pDHP;

	TLSDHGSizeType		dhgSize;
	CrUINT8*			pDHG;
}TLSDHParameters;

#endif // #ifdef TLS_API_DH


/**
 * #######################################################
 * 			RSA KEY STRUCTURE DEFINITION
 * #######################################################
 */
#ifdef TLS_API_RSA

typedef CrUINT16	TLSModulusSizeType;
typedef CrUINT16	TLSExponentSizeType;
typedef CrUINT16	TLSPublicExponentSizeType;
typedef CrUINT16	TLSPrivateExponentSizeType;
typedef CrUINT16	TLSPrime1SizeType;
typedef CrUINT16	TLSPrime2SizeType;
typedef CrUINT16	TLSExponent1SizeType;
typedef CrUINT16	TLSExponent2SizeType;
typedef CrUINT16	TLSCoefficientSizeType;

/* TLS RSA PUBLIC KEY */
typedef	struct tagTLSRSAPublicKey
{
#define TLS_EXPONENT_SIZE_LOW_BOUND	1
	TLSExponentSizeType		exponentSize;
	CrUINT8*				pExponent;

#define TLS_MODULUS_SIZE_LOW_BOUND		1
	TLSModulusSizeType		modulusSize;
	CrUINT8*				pModulus;
}TLSRSAPublicKey;

/* TLS RSA PRIVATE KEY */
typedef struct tagTLSRSAPrivateKey
{
    TLSModulusSizeType				modulusSize;
    CrUINT8*						pModulus;
    
    TLSPublicExponentSizeType		publicExponentSize;
    CrUINT8*						pPublicExponent;
    
    TLSPrivateExponentSizeType		privateExponentSize;
    CrUINT8*						pPrivateExponent;
    
    TLSPrime1SizeType				prime1Size;
    CrUINT8*						pPrime1;
    
    TLSPrime2SizeType				prime2Size;
    CrUINT8*						pPrime2;
    
    TLSExponent1SizeType			exponent1Size;
    CrUINT8*						pExponent1;
    
    TLSExponent2SizeType			exponent2Size;
    CrUINT8*						pExponent2;
    
    TLSCoefficientSizeType			coefficientSize;
    CrUINT8*						pCoefficient;
}TLSRSAPrivateKey;

#endif // #ifdef TLS_API_RSA


/**
 * #######################################################
 * 			ECC KEY STRUCTURE DEFINITION
 * #######################################################
 */
#ifdef TLS_API_ECC

// 2001.06.18 Added by Taesung Kim For WTLS, SSL ECC Curves
typedef CrUINT8							TLSProtocolIndex;
#define TLS_PROTOIDX_WTLS_ECC			1
#define TLS_PROTOIDX_SSL_ECC			2

/* TLS ECC Public Key */
typedef	CrUINT8	TLSECPointSizeType;
typedef struct	tagTLSECPoint
{
#define TLS_EC_POINT_SIZE_LOW_BOUND		1
	TLSECPointSizeType		ecPointSize;
	CrUINT8*				pECPoint;
}TLSECPoint;
typedef	struct tagTLSECPoint TLSECPublicKey;

/* TLS ECC Private Key */
typedef CrUINT8	TLSECValueSizeType;
typedef struct tagTLSECPrivateKey
{
	TLSECValueSizeType		valueSize;
	CrUINT8*				pValue;
}TLSECPrivateKey;

typedef CrUINT8	TLSASizeType;
typedef CrUINT8	TLSBSizeType;
typedef CrUINT8	TLSSeedSizeType;

typedef struct tagTLSECCurve 
{
	TLSASizeType		aSize;
	CrUINT8*			pA;	 	/* a<1..2^8-1> */

	TLSBSizeType		bSize;
	CrUINT8*			pB; 	/* b<1..2^8-1> */

	TLSSeedSizeType		seedSize;
	CrUINT8*			pSeed; 	/* seed<0..2^8-1> */
}TLSECCurve;

#endif // #ifdef TLS_API_ECC

/**
 * #######################################################
 * 		COMMON KEY STRUCTURE DEFINITION
 * #######################################################
 */
typedef CrUINT8 						TLSParameterIndex;

#define TLS_PARAMIDX_NOT_APPLICABLE 		0
#define TLS_PARAMIDX_DH_512 				1	/* Prime modulus : 512 bits */
#define TLS_PARAMIDX_DH_768 				2	/* Prime modulus : 768 bits */
typedef enum
{
	TLS_PARAMIDX_WTLS_EC_1	= 1,
	TLS_PARAMIDX_WTLS_EC_2,
	TLS_PARAMIDX_WTLS_EC_3,
	TLS_PARAMIDX_WTLS_EC_4,
	TLS_PARAMIDX_WTLS_EC_5,

	TLS_PARAMIDX_WTLS_EC_6,
	TLS_PARAMIDX_WTLS_EC_7,
	TLS_PARAMIDX_WTLS_EC_8,
	TLS_PARAMIDX_WTLS_EC_9,

	TLS_PARAMIDX_WTLS_EC_10, // NEW ADD(WAP-261-WTLS-20010406-a) = TLS_PARAMIDX_SSL_EC_6
	TLS_PARAMIDX_WTLS_EC_11, // NEW ADD(WAP-261-WTLS-20010406-a) = TLS_PARAMIDX_SSL_EC_7
	TLS_PARAMIDX_WTLS_EC_12, // NEW ADD(WAP-261-WTLS-20010406-a) = TLS_PARAMIDX_SSL_EC_21

	TLS_PARAMIDX_SSL_EC_1,
	TLS_PARAMIDX_SSL_EC_2,
	TLS_PARAMIDX_SSL_EC_3,
	TLS_PARAMIDX_SSL_EC_4,
	TLS_PARAMIDX_SSL_EC_5,
	TLS_PARAMIDX_SSL_EC_6,
	TLS_PARAMIDX_SSL_EC_7,
	TLS_PARAMIDX_SSL_EC_8,
	TLS_PARAMIDX_SSL_EC_9,
	TLS_PARAMIDX_SSL_EC_10,
	TLS_PARAMIDX_SSL_EC_11,
	TLS_PARAMIDX_SSL_EC_12,
	TLS_PARAMIDX_SSL_EC_13,
	TLS_PARAMIDX_SSL_EC_14,

	TLS_PARAMIDX_SSL_EC_15,
	TLS_PARAMIDX_SSL_EC_16,
	TLS_PARAMIDX_SSL_EC_17,
	TLS_PARAMIDX_SSL_EC_18,
	TLS_PARAMIDX_SSL_EC_19,
	TLS_PARAMIDX_SSL_EC_20,
	TLS_PARAMIDX_SSL_EC_21,
	TLS_PARAMIDX_SSL_EC_22,
	TLS_PARAMIDX_SSL_EC_23,
	TLS_PARAMIDX_SSL_EC_24,
	TLS_PARAMIDX_SSL_EC_25
} TLSECCurveIndexEnum;





#define	TLS_PARAMIDX_EXPLICIT 				0xFF	/* parameter_set present */



#endif // #ifndef TLS_CR_API_KEY_ST_H
