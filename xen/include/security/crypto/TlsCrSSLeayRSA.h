#ifndef _TLS_CR_SSLEAY_RSA_H

#define _TLS_CR_SSLEAY_RSA_H



/**

 * ############################################################################

 *	- TlsCrRSAPublicSSLeay2Tls

 *	- TlsCrRSAPrivateSSLeay2Tls

 *	- TlsCrRSAPrivateTls2SSLeay

 *	- TlsCrRSAPublicTls2SSLeay

 *	- TlsCrRSAPrEnc

 */











 /**

 * ############################################################################

 * TlsCrRSAPublicSSLeay2Tls

 * ########################

 *

 * DESCRIPTION

 *		This function is for the conversion from [SSLeay RSA Structure] to 

 *		[TLS RSA Public Key Structure].

 * ============================================================================

 * INPUT

 *		RSA*				pRsa		

 *		: Pointer of SSLeay RSA Structure.

 * ============================================================================

 * OUTPUT

 *		TLSRSAPublicKey**	ppRSAPuKey	

 *		: Pointer to PointerTLS RSA Public Key Structure.

 * ============================================================================

 * RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory, eSSLeayBn2BinFailure

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPublicSSLeay2Tls(

	TLSRSAPublicKey**	ppRSAPuKey,

	RSA*				pRsa

);





 /**

 * ############################################################################

 * TlsCrRSAPrivateSSLeay2Tls

 * #########################

 *

 * DESCRIPTION

 *		This function is for the conversion from [SSLeay RSA Structure] to 

 *		[TLS RSA Private Key Structure].

 * ============================================================================

 * INPUT

 *		RSA*				pRsa		

 *		: Pointer of SSLeay RSA Structure.

 * ============================================================================

 * OUTPUT

 *		TLSRSAPrivateKey**	ppRSAPrKey	

 *		: Pointer to PointerTLS RSA Private Key Structure.

 * ============================================================================

 * RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory, eSSLeayBn2BinFailure

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPrivateSSLeay2Tls(

	TLSRSAPrivateKey**		ppRSAPrKey,

	RSA*					pRsa

);





 /**

 * ############################################################################

 * TlsCrRSAPrivateTls2SSLeay

 * #########################

 *

 * DESCRIPTION

 *		This function is for the conversion from [TLS RSA Private Key Structure] to 

 *		[SSLeay RSA Structure].

 * ============================================================================

 * INPUT

 *		TLSRSAPrivateKey*			pPrKey

 *		: Pointer of TLS RSA Private Key Structure.

 * ============================================================================

 * OUTPUT

 *		RSA**						ppSSLeayRsa	

 *		: Pointer to Pointer SSLeay RSA Structure.

 * ============================================================================

 * RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eNoMoreMemory, eRSAPrivateTls2SSLeayFailure

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPrivateTls2SSLeay(

	RSA**				ppSSLeayRsa,

	TLSRSAPrivateKey*	pPrKey

);









 /**

 * ############################################################################

 * TlsCrRSAPublicTls2SSLeay

 * ########################

 * Description

 *		This function is for the coversion from [TLS RSA Public Key Structure] to

 *		[SSLeay RSA Structure].

 * ============================================================================

 * INPUT

 *		TLSRSAPublicKey*	pPuKey

 *		: Pointer of TLS RSA Public Key Structure.

 * ============================================================================

 * OUTPUT

 *		RSA**				ppSSLeayRsa,

 *		: Pointer of SSLeay RSA Key Structure.

 * ============================================================================

 * RETURN

 *		SUCCESS				: noError

 *		FAILURE				: eRSAPublicTls2SSLeayFailure

 *							  eNoMoreMemory

 * ############################################################################

 */

TLSCrResult

TlsCrRSAPublicTls2SSLeay(

	RSA**				ppSSLeayRsa,

	TLSRSAPublicKey*	pPuKey

);







#endif // #ifndef _TLS_CR_SSLEAY_RSA_H


