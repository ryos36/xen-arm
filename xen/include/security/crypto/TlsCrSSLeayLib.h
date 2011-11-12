#ifndef _TLS_CR_SSLEAYLIB_H

#define _TLS_CR_SSLEAYLIB_H



TLSCrResult

TlsCrSSLeayBn2Bin(

	CrUINT8**	ppOut,

	CrUINT16*	pOutSize,

	BIGNUM*		pSSLeayBN

);





TLSCrResult

TlsCrGenerateRandomNumber(

	CrUINT8**	ppOut,

	CrUINT16	outSize

);



#endif // #ifndef _TLS_CR_SSLEAYLIB_H


