#ifndef _DEBUG_PRINT_H_

#define _DEBUG_PRINT_H_



#define TOTAL_BN	-1

#define LEAST_BN	-2

#define MOST_BN		-3



void PrintBN2Bin(BIGNUM* pBN, CrINT32 offset);

void PrintLong2Bin(BN_ULONG* pBN);

void PrintBN2Hex(BIGNUM* pBN, CrUINT8*	pBNName);

void PrintChar2Word(CrUINT8* pChar, CrUINT16 uCharLen, CrINT8* pDataName);

void _blockPrintf(

	CrUINT8*	pBlock,

	CrUINT16	uBlockLen,

	CrUINT8*	pBlockName

);





#ifdef TLS_API_RSA

void

PrintTLSRSAPublicKey (TLSRSAPublicKey*	pTlsRSAPuKey);



void

PrintTLSRSAPrivateKey (TLSRSAPrivateKey* pTlsRSAPrKey);

#endif // end of "#ifdef TLS_API_RSA"

#endif // end of _DEBUG_PRINT_H_



