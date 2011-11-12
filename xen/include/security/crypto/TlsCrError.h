#ifndef _TLS_CRERROR_H_

#define _TLS_CRERROR_H_



#define eSSLeayErr0				0

#define eSSLeayErrN1			-1



#define noError					 0



#define TLS_CR_ERROR			-15000



#define eNoMoreMemory			TLS_CR_ERROR-1

#define eVerificationFailure	eNoMoreMemory-1

#define eAlgMismatch			eVerificationFailure-1

#define eRandomNumberFailure    eAlgMismatch-1

#define eLengthMismatch			eRandomNumberFailure-1

//for sample test

#define eNotEqu					eLengthMismatch-1



//1998.10.22 added

#define eBEncAlg				TLS_CR_ERROR-100

#define eBEncFailure			eBEncAlg-1

#define eBDecFailure			eBEncFailure-1

#define eBadCipherCtx			eBDecFailure-1	// 2001.11.28

#define eEvpFinalFailure		eBadCipherCtx-1 // 2001.11.28







#define eSSLeayBn2BinFailure           eEvpFinalFailure-1



#define	eRSAKeyGenerationFailure	   eSSLeayBn2BinFailure -1





#define eMacHashAlgorithm				TLS_CR_ERROR-200

#define ePRF							eMacHashAlgorithm-1



#define eDHParameterIndex				TLS_CR_ERROR-300

#define eDHParametersMismatch			eDHParameterIndex-1

#define eDHKeyComputationFailure		eDHParametersMismatch-1

#define eDHPARAMS2SSLeayDHStxFailure	eDHKeyComputationFailure-1

#define eDHPrKey2SSLeayDHStxFailure		eDHPARAMS2SSLeayDHStxFailure-1

#define eDHKeyGenerationFailure			eDHPrKey2SSLeayDHStxFailure-1



#define eRSAPMSInServerFailure			TLS_CR_ERROR-300

#define eRSAPrivateTls2SSLeayFailure	eRSAPMSInServerFailure-1

#define eRSAPublicTls2SSLeayFailure		eRSAPrivateTls2SSLeayFailure-1

#define eRSAPublicEncryptionFailure		eRSAPublicTls2SSLeayFailure-1

#define eRSAPrivateDecryptionFailure	eRSAPublicEncryptionFailure-1

#define eRSASignFailure					eRSAPrivateDecryptionFailure-1

#define eRSAVerifyFailure				eRSASignFailure-1

#define eRSAPrivateEncryptionFailure	eRSAVerifyFailure-1

#define eRSAPublicDecrytionFailure		eRSAPrivateEncryptionFailure-1

#define eRSAWrongSignatureLength		eRSAPublicDecrytionFailure-1

#define eRSAVerifyinSSL					eRSAWrongSignatureLength-1



#define CHECK_ERROR(e) if (e!= noError) return (e);

#define CHECK_LOCAL_ERROR(e) if (e!=noError) goto ERR;



#endif


