#ifndef _TLS_CR_RSA_HEADER_

#define _TLS_CR_RSA_HEADER_



/**

 * ############################################################################

 * TlsCrGenerateRSAKey

 * ###################

 * @ Description

 *		This function is for the generation of TLS RSA Public/Private Key Pair.

 * ============================================================================

 * @ INPUT

 *		TLSModulusBitsSizeType			modlusBitsSize

 *		: Bit Length of RSA modulus.

 * ============================================================================

 * @ OUTPUT

 *		TLSRSAPrivateKey**				ppRSAPrivateKey

 *		: Pointer to pointer of TLS RSA Private Key Structure.

 *		TLSRSAPrivateKey**				ppRSAPublicKey

 *		: Pointer to pointer of TLS RSA Public Key Structure.

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAKeyGenerationFailure

 *							  eNoMoreMemory

 * ############################################################################

 */

TLSCrResult 

TlsCrGenerateRSAKey(

	TLSRSAPrivateKey**		ppRSAPrivateKey,			// out

	TLSRSAPublicKey**		ppRSAPublicKey,				// out

	TLSModulusBitsSizeType	modlusBitsSize				// in

);









/**

 * ############################################################################

 * TlsCrGenerateRSAEncSecret

 * #########################

 * @ Description

 *		This function is for the RSA Encryption(RSA Public Encryption).

 * ============================================================================

 * @ INPUT

 *		CrUINT8*				pEncodedSecret

 *			: Pointer to the value to be encrypted.

 *		CrUINT16				uEncodedSecretLen

 *			: Byte length of the pEncodedSecret parameter.

 *		TLSRSAPublicKey*		pPuKey

 *			: Pointer to the TLS RSA Public Key Structure.

 * ============================================================================

 * @ OUTPUT

 *		CrUINT8**				ppRSAEncSecret

 *		: Pointer to pointer of the encrypted data.

 *		CrUINT16*				pRSAEncSecretLen

 *		: Pointer to the byte length of encrypted data.

 *		TLSRSAPublicKey*		pPuKey

 *		: Pointer to the TLS RSA Public Key Structure.

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory

 * ############################################################################

 */

TLSCrResult

TlsCrGenerateRSAEncSecret(

	CrUINT8**			ppRSAEncSecret,		// out

	CrUINT16*			pRSAEncSecretLen,	// out

	CrUINT8*			pEncodedSecret,		// in

	CrUINT16			uEncodedSecretLen,	// in

	TLSRSAPublicKey*	pPuKey				// in

);









/**

 * ############################################################################

 * TlsCrComputeRSAPreMasterSecretInClient

 * ######################################

 * @ Description

 *		This function is for the Generation of TLS RSA Premaster Secret Value.

 * ============================================================================

 * @ INPUT

 *		CrUINT8*				pEncodedSecret

 *			: Pointer to the Encoded Secret Value.

 *		CrUINT16				uEncodedSecretLen

 *			: Byte length of the encoded secret value.

 *		CrUINT8*				pEncodedKey

 *			: Pointer to the encoded key value.

 *		CrUINT16				uEncodedKeyLen

 *			: Byte length of the encoded key value.

 * ============================================================================

 * @ OUTPUT

 *		CrUINT8**				ppRSAPreMasterSecret

 *			: Pointer to the RSA premaster secret value.

 *		CrUINT16*				pRSAPreMasterSecretLen

 *			: Byte length of the RSA premaster secret value.

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory

 * ============================================================================

 * @ HISTORY

 *

 * ############################################################################

 */

TLSCrResult 

TlsCrComputeRSAPreMasterSecretInClient(

	CrUINT8**			ppRSAPreMasterSecret,

	CrUINT16*			pRSAPreMasterSecretLen,

	CrUINT8*			pEncodedSecret,

	CrUINT16			uEncodedSecretLen,

	CrUINT8*			pEncodedKey,

	CrUINT16			uEncodedKeyLen

);







/**

 * ############################################################################

 * TlsCrRSAPuEnc

 * #############

 * @ Description

 *		This function is for the RSA Encryption.

 * ============================================================================

 * @ INPUT

 *		CrUINT8*				pIn

 *			: Pointer to the data to be encrypted.

 *		CrUINT16				uInLen

 *			: Byte length of the data to be encrypted.

 *		TLSRSAPublicKey*		pPuKey

 *			: Pointer to the TLS RSA Public Key Structure.

 * ============================================================================

 * @ OUTPUT

 *		CrUINT8*				pEncOut

 *			: Pointer to the RSA encrypted data.

 *		CrUINT16*				pEncOutLen

 *			: Pointer to the byte length of the RSA encrypted data.

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory

 *							  eSSLeayErrN1

 * ============================================================================

 * @ HISTORY

 *

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPuEnc(

	CrUINT8**				ppEncOut,

	CrUINT16*				pEncOutLen,

	CrUINT8*				pIn,

	CrUINT16				uInLen,

	TLSRSAPublicKey*		pPuKey

);



/**

 * ############################################################################

 * TlsCrRSAPrDec

 * ############

 * @ DESCRIPTION

 *		This function is for the RSA Private Decryption Operation.

 *		(RSA Private Decryption)

 * ============================================================================

 * @ INPUT

 *		CrINT8*				pEncIn

 *			: Pointer to the RSA Public Encrypted Value.

 *		CrINT16				uEncInLen

 *			: Byte length of the RSA Public Encrypted Value.

 *		TLSRSAPrivateKey*	pRSAPrKey

 *			: Pointer to the TLS RSA Private Key Structure.

 * ============================================================================

 * @ OUTPUT

 *		CrINT8**			ppDecOut

 *			: Pointer of pointer to the RSA Private Decrypted Value.

 *		CrINT16*			pDecOutLen

 *			: Pointer to the byte length of the RSA Private Decrypted Value.

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAPrivateTls2SSLeayFailure

 *							  eNoMoreMemory

 *							  eRSAPrivateEncryptionFailure

 * ============================================================================

 * @ HISTORY

 *

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPrDec(

	CrUINT8**				ppDecOut,		// out

	CrUINT16*				pDecOutLen,		// out

	CrUINT8*				pEncIn,			// in

	CrUINT16				uEncInLen,		// in

	TLSRSAPrivateKey*		pRSAPrKey		// in

);



/**

 * ############################################################################

 * TlsCrRSASign

 * ############

 * @ DESCRIPTION

 *		This function is for the RSA Sign Operation.(RSA Private Encryption)

 * ============================================================================

 * @ INPUT

 *		CrINT8*				pHash

 *			: Pointer to the MD5 or SHA1 Hashed Value.

 *		CrINT16				uHashLen

 *			: Byte length of the MD5 or SHA1 Hashed Value.

 *		TLSRSAPrivateKey*	pRSAPrKey

 *			: Pointer to the TLS RSA Private Key Structure.

 * ============================================================================

 * @ OUTPUT

 *		CrINT8**			ppOut

 *			: Pointer of pointer to the RSA Signed Value.

 *		CrINT16*			pOutLen

 *			: Pointer to the byte length of the RSA Signed Value

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAPrivateTls2SSLeayFailure

 *							  eNoMoreMemory

 *							  eRSAPrivateEncryptionFailure

 * ============================================================================

 * @ HISTORY

 *

 * ############################################################################

 */

TLSCrResult 
TlsCrRSASign(
	CrUINT8**				ppOut,
	CrUINT32*				pOutLen,
	CrUINT8*				pHash,
	CrUINT32				uHashLen,
	TLSRSAPrivateKey* 		pRSAPrKey
);



/**

 * ############################################################################

 * TlsCrRSAVerify

 * ##############

 * @ DESCRIPTION

 *		This function is for the RSA Verify Operation.(RSA Public Decryption)

 * ============================================================================

 * @ INPUT

 *		CrINT8*				pHash

 *			: Pointer to the MD5 or SHA1 or MD5+SHA1 Hashed Value.

 *		CrINT16				uHashLen

 *			: Byte length of the MD5 or SHA1 or MD5+SHA1 Hashed Value.

 *		CrUINT8*			pSignedValue

 *			: Pointer to the RSA Signed Value.

 *		CrUINT16			uSignedValue

 *			: Byte length of the RSA Signed Value.

 *		TLSRSAPrivateKey*	pRSAPrKey

 *			: Pointer to the TLS RSA Private Key Structure.

 * ============================================================================

 * @ OUTPUT

 * ============================================================================

 * @ RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAWrongSignatureLength

 *							  eNoMoreMemory

 *							  eRSAVerifyFailure

 *							  eRSAVerifyinSSL

 * ============================================================================

 * @ HISTORY

 *

 * ############################################################################

 */

TLSCrResult
TlsCrRSAVerify(
	CrUINT8*				pHash,
	CrUINT32				uHashLen,		/* 16, 20, 36 */
	CrUINT8*				pSignedValue,
	CrUINT32				uSignedValueLen,
	TLSRSAPublicKey*		pRSAPuKey
);


 /**

 * ############################################################################

 * TlsCrRSAPrEnc

 * #############

 * Description

 *		This function is for the RSA Sign Operation.(Private Encryption).

 *		The data to be signed is the hashed value such as SHA1 or MD5 algorithm.

 * ============================================================================

 * INPUT

 *		CrINT8*				pHashedValue,		: MD5 or SHA1 Hashed Value

 *		CrINT16				uHashValueSize,		: MD5 or SHA1 Hashed Value Size

 *		TLSRSAPrivateKey*	pRSAPrKey			: TLS RSA Private Key

 * ============================================================================

 * OUTPUT

 *		CrINT8*				pSignValue,			: RSA Private Encrypted Value

 *		CrINT16*			pSignValueSize,		: RSA Private Encrypted Value Size

 * ============================================================================

 * RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAPrivateTls2SSLeayFailure

 *							  eNoMoreMemory

 *							  eRSAVerifyFailure

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPrEnc(

	CrUINT8**			ppSignValue,

	CrUINT16*			pSignValueSize,

	CrUINT8*			pHashedValue,

	CrUINT16			uHashValueSize,

	TLSRSAPrivateKey*	pRSAPrKey

);









#endif // end of #ifndef _TLS_CR_RSA_HEADER_

