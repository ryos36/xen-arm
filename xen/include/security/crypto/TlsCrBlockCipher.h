#ifndef _TLS_CR_EVP_ENC_H_
#define _TLS_CR_EVP_ENC_H_
/**
 * ###########################################################
 *						Encrypt API
 * ###########################################################
 */
#define SYMMETRIC_ENCRYPTION		1
#define SYMMETRIC_DECRYPTION		0

TLSCrResult 
TlsCrEncryptBlock(
	CrUINT8**				ppOut,	//out
	CrUINT16*				pOutSz,	//out
	CrUINT8*				pData,	//in
	CrUINT16				uDataSz,	//in
	CrUINT8*				pKey,	//in
	CrUINT16				uKeySz,	//in
	CrUINT8*				pIV,	//in
	CrUINT16				uIVSz,	//in
	TLSBulkCipherAlgorithm	iBulkCipherIndex	//in
);

TLSCrResult 
TlsCrDecryptBlock(
	CrUINT8**				ppOut,	//out
	CrUINT16*				pOutSz,
	CrUINT8*				pData,	//in
	CrUINT16				uDataSz,
	CrUINT8*				pKey,	//in
	CrUINT16				uKeySz,
	CrUINT8*				pIV,	//in
	CrUINT16				uIVSz,
	TLSBulkCipherAlgorithm	iBulkCipherIndex	// in
);

TLSCrResult 
TlsCrEncryptBlock_32(
	CrUINT8**				ppOut,				//out
	CrUINT32*				pOutLen,				//out
	CrUINT8*				pData,				//in
	CrUINT32				uDataLen,			//in
	CrUINT8*				pKey,				//in
	CrUINT32				uKeyLen,				//in
	CrUINT8*				pIV,				//in
	CrUINT32				uIVLen,				//in
	TLSBulkCipherAlgorithm	iBulkCipherIndex	//in
);


TLSCrResult 
TlsCrDecryptBlock_32(
	CrUINT8**				ppOut,	//out
	CrUINT32*				pOutLen,
	CrUINT8*				pData,	//in
	CrUINT32				uDataLen,
	CrUINT8*				pKey,	//in
	CrUINT32				uKeyLen,
	CrUINT8*				pIV,	//in
	CrUINT32				uIVLen,
	TLSBulkCipherAlgorithm	iBulkCipherIndex	// in
);

TLSCrResult
TlsCrEvpInit (
	EVP_CIPHER_CTX**	ppCipherCtx,	// update와 final에서 필요한 구조체
	CrUINT8*			pKey,		// key
	CrUINT16			uKeyLen,	// key size
	CrUINT8*			pIV,		// initial vector
	CrUINT16			uIVLen,		// initial vector size
	CrINT32				enc,		// enc/dec
	TLSBulkCipherAlgorithm	iBCIdx	// algorithm index
);

TLSCrResult
TlsCrEvpUpdate (
		EVP_CIPHER_CTX *pCipherCtx,		/* 이전의 enc/dec결과와 key값들을 저장해 둔 구조체 */
		CrUINT8 **ppOut,			/* enc/dec 결과 */
		CrUINT32 *pOutLen,		/* enc/dec 결과의 길이 */
		CrUINT8 *pData,			/* enc/dec할려는 데이터 */
		CrUINT32 uDataLen		/* Data의 길이 */
);

TLSCrResult
TlsCrEvpFinal (
	EVP_CIPHER_CTX*		pCipherCtx,	// enc에서는 마지막으로 padding후 enc
	CrUINT8**			ppOut,
	CrUINT32*			pOutLen
);

#endif

