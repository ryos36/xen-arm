#ifndef _TLS_CRHASH_H
#define _TLS_CRHASH_H
#include "TlsCrConfig.h"
#include "TlsCrSecurity.h"
#include "CrSHA.h"
/*
#include "TlsCrApi.h"
*/
/*
#define MAX_DIGEST_LEN 20
#define MAC_HASH_BLOCK_LEN 64
*/
/* substitutes from SHA 512 for SHA1 */
#define MAX_DIGEST_LEN			128			// SHA512_DIGEST_LENGTH
#define MAC_HASH_BLOCK_LEN		64			// SHA512_BLOCK_LENGTH
#define	IPAD					0x36
#define OPAD					0x5c
#define	HASH_BLOCK_LEN			64
#define TLS_HASH_INIT_MODE		0
#define TLS_HASH_UPDATE_MODE	1
#define TLS_HASH_FINAL_MODE		2
#define TLS_HASH_TOTAL_MODE		3
#ifdef TLS_API_SHA1
#if defined(WTLS) || defined(TLS)


TLSCrResult
TlsCrSHA1(
	CrUINT8**		ppOut,
	CrULONG32*		pOutSz,
	SHA_CTX*		pSHA,
	CrUINT8*		pIn,
	CrULONG32		uInSz,
	CrINT32			mode
);

#else

TLSCrResult TlsCrSHA1(
	CrUINT8**		ppOut,
	CrUINT16*		pOutLen,
	CrUINT8*		pIn,
	CrUINT16		uInLen
);

#endif

#endif

#ifdef TLS_API_SHA256
TLSCrResult
TlsCrSHA256(
	CrUINT8**		ppOut,		//out
	CrUINT16*		pOutLen,	//out
	CrUINT8*		pIn,		//in
	CrUINT16		uInLen		//in
);

#endif

#ifdef TLS_API_SHA384
TLSCrResult
TlsCrSHA384(
	CrUINT8**		ppOut,		//out
	CrUINT16*		pOutLen,	//out
	CrUINT8*		pIn,		//in
	CrUINT16		uInLen		//in
);
#endif

#ifdef TLS_API_SHA512
TLSCrResult
TlsCrSHA512(
	CrUINT8**		ppOut,		//out
	CrUINT16*		pOutLen,	//out
	CrUINT8*		pIn,		//in
	CrUINT16		uInLen		//in
);
#endif

#ifdef TLS_API_MD5
TLSCrResult
TlsCrMD5(
	CrUINT8**	ppOut,
	CrUINT16*	pOutSz,
	MD5_CTX*	pMD5,
	CrUINT8*	pIn,
	CrUINT16	uInSz,
	CrINT32		mode
);
#endif

TLSCrResult
TlsCrGetHashSize(
	CrUINT16*		pHashSize,
	TLSMACAlgorithm	iHashParameterIndex
);

TLSCrResult
TlsCrHash(
	CrUINT8* 		pOut,
	CrUINT16*		pOutSize,
	CrUINT8* 		pData,
	CrUINT16		uDataSize,
	TLSMACAlgorithm iHashParameterIndex
);

#ifdef TLS_API_HMAC
TLSCrResult
TlsCrHMAC(		
	CrUINT8**				ppHMACOut,
	CrUINT16*				pHMACOutSz,
	CrUINT8*				pKey,
	CrUINT16				uKeySz,
	CrUINT8*				pData,
	CrUINT16				uDataSz,
	TLSMACAlgorithm			iHashParameterIndex	// in
);
#endif

#endif // #ifndef _TLS_CRHASH_H

