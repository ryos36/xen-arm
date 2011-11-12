/* crypto/bn/bn.org */

/* Copyright (C) 1995-1997 Eric Young (eay@cryptsoft.com)

 * All rights reserved.

 *

 * This package is an SSL implementation written

 * by Eric Young (eay@cryptsoft.com).

 * The implementation was written so as to conform with Netscapes SSL.

 * 

 * This library is free for commercial and non-commercial use as CrLONG32 as

 * the following conditions are aheared to.  The following conditions

 * apply to all code found in this distribution, be it the RC4, RSA,

 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation

 * included with this distribution is covered by the same copyright terms

 * except that the holder is Tim Hudson (tjh@cryptsoft.com).

 * 

 * Copyright remains Eric Young's, and as such any Copyright notices in

 * the code are not to be removed.

 * If this package is used in a product, Eric Young should be given attribution

 * as the author of the parts of the library used.

 * This can be in the form of a textual message at program startup or

 * in documentation (online or textual) provided with the package.

 * 

 * Redistribution and use in source and binary forms, with or without

 * modification, are permitted provided that the following conditions

 * are met:

 * 1. Redistributions of source code must retain the copyright

 *    notice, this list of conditions and the following disclaimer.

 * 2. Redistributions in binary form must reproduce the above copyright

 *    notice, this list of conditions and the following disclaimer in the

 *    documentation and/or other materials provided with the distribution.

 * 3. All advertising materials mentioning features or use of this software

 *    must display the following acknowledgement:

 *    "This product includes cryptographic software written by

 *     Eric Young (eay@cryptsoft.com)"

 *    The word 'cryptographic' can be left out if the rouines from the library

 *    being used are not cryptographic related :-).

 * 4. If you include any Windows specific code (or a derivative thereof) from 

 *    the apps directory (application code) you must include an acknowledgement:

 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"

 * 

 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND

 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE

 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE

 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE

 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL

 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS

 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)

 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT

 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY

 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF

 * SUCH DAMAGE.

 * 

 * The licence and distribution terms for any publically available version or

 * derivative of this code cannot be changed.  i.e. this code cannot simply be

 * copied and put under another distribution licence

 * [including the GNU Public Licence.]

 */



/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 

 *

 * Always modify bn.org since bn.h is automatically generated from

 * it during SSLeay configuration.

 *

 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

 */



#ifndef HEADER_BN_H

#define HEADER_BN_H



#ifdef  __cplusplus

extern "C" {

#endif



#undef BN_LLONG



#if defined(WIN32)

#define BN_LLONG /* This comment stops Configure mutilating things */

#endif



#define RECP_MUL_MOD

#define MONT_MUL_MOD



/* This next option uses the C libraries (2 word)/(1 word) function.

 * If it is not defined, I use my C version (which is slower).

 * The reason for this flag is that when the particular C compiler

 * library routine is used, and the library is linked with a different

 * compiler, the library is missing.  This mostly happens when the

 * library is built with gcc and then linked using nornal cc.  This would

 * be a common occurance because gcc normally produces code that is

 * 2 times faster than system compilers for the big number stuff.

 * For machines with only one compiler (or shared libraries), this should

 * be on.  Again this in only really a problem on machines

 * using "CrLONG32 CrLONG32's", are 32bit, and are not using my assember code. */

#if defined(MSDOS) || defined(WINDOWS) || defined(linux)

#define BN_DIV2W

#endif



/* Only one for the following should be defined */

/* The prime number generation stuff may not work when

 * EIGHT_BIT but I don't care since I've only used this mode

 * for debuging the bignum libraries */

#undef SIXTY_FOUR_BIT_LONG

#undef SIXTY_FOUR_BIT

#define THIRTY_TWO_BIT

#undef SIXTEEN_BIT

#undef EIGHT_BIT



/* assuming CrLONG32 is 64bit - this is the DEC Alpha

 * CrULONG32 CrLONG32 is only 64 bits :-(, don't define

 * BN_LLONG for the DEC Alpha */



#ifdef THIRTY_TWO_BIT

#define BN_ULLONG	CrULONG64

#define BN_ULONG	CrULONG32

#define BN_LONG		CrLONG32

#define BN_BITS		64

#define BN_BYTES	4

#define BN_BITS2	32

#define BN_BITS4	16

#define BN_MASK2	(0xffffffffL)

#define BN_MASK2l	(0xffff)

#define BN_MASK2h1	(0xffff8000L)

#define BN_MASK2h	(0xffff0000L)

#define BN_TBIT		(0x80000000L)

#define BN_DEC_CONV	(1000000000L)

#define BN_DEC_FMT1	"%lu"

#define BN_DEC_FMT2	"%09lu"

#define BN_DEC_NUM	9

#endif



#define BN_DEFAULT_BITS	163



#ifdef BIGNUM

#undef BIGNUM

#endif



typedef struct bignum_st

{

	BN_ULONG*	d;	/* Pointer to an array of 'BN_BITS2' bit chunks. */

	CrINT32		top;	/* Index of last used d +1. */

	/* The next are internal book keeping for bn_expand. */

	CrINT32		max;	/* Size of the d array. */

	CrINT32		neg;	/* one if the number is negative */

} BIGNUM;



/* Used for temp variables */

#define BN_CTX_NUM	20



//If you use this in the ECC library, please modify BN_CTX_NUM 12

typedef struct bignum_ctx

{

	CrINT32		tos;

	BIGNUM*		bn[BN_CTX_NUM+1];

} BN_CTX;



typedef struct bn_blinding_st

{

	CrINT32 		init;

	BIGNUM 		*A;

	BIGNUM 		*Ai;

	BIGNUM 		*mod; /* just a reference */

} BN_BLINDING;



/* Used for montgomery multiplication */

typedef struct bn_mont_ctx_st

{

        CrINT32		ri;         /* number of bits in R */

        BIGNUM*		RR;     /* used to convert to montgomery form */

        BIGNUM*		N;      /* The modulus */

        BIGNUM*		Ni;     /* The inverse of N */

		BN_ULONG	n0;	/* word form of inverse, normally only one of

			 * Ni or n0 is defined */

} BN_MONT_CTX;



#define BN_to_montgomery(r,a,mont,ctx)	BN_mod_mul_montgomery(r,a,(mont)->RR,(mont),ctx)



#define BN_prime_checks		(5)



#define BN_num_bytes(a)		((BN_num_bits(a)+7)/8)

#define BN_is_word(a,w)		(((a)->top == 1) && ((a)->d[0] == (BN_ULONG)(w)))

#define BN_is_zero(a)		(((a)->top <= 1) && ((a)->d[0] == (BN_ULONG)0))

#define BN_is_one(a)		(BN_is_word((a),1))

#define BN_is_odd(a)		((a)->d[0] & 1)

#define BN_one(a)			(BN_set_word((a),1))

#define BN_zero(a)			(BN_set_word((a),0))



#define bn_fix_top(a) \
{ \
	BN_ULONG *fix_top_l; \
	for (fix_top_l= &((a)->d[(a)->top-1]); (a)->top > 0; (a)->top--) \
		if (*(fix_top_l--)) break; \
}



#define bn_expand(n,b) ((((b)/BN_BITS2) <= (n)->max)?(n):bn_expand2((n),(b)/BN_BITS2))

#define bn_wexpand(n,b) (((b) <= (n)->max)?(n):bn_expand2((n),(b)))



#ifndef NOPROTO

BIGNUM*		BN_value_one(void);

CrINT8*		BN_options(void);

BN_CTX*		BN_CTX_new(void);

void		BN_CTX_free(BN_CTX *c);

CrINT32		BN_rand(BIGNUM *rnd, CrINT32 bits, CrINT32 top,CrINT32 bottom);

CrINT32		BN_num_bits(BIGNUM *a);

CrINT32		BN_num_bits_word(BN_ULONG);

BIGNUM*		BN_new(void);

void		BN_clear_free(BIGNUM *a);

BIGNUM*		BN_copy(BIGNUM *a, BIGNUM *b);

BIGNUM*		BN_bin2bn(CrUINT8 *s,CrINT32 len,BIGNUM *ret);

CrINT32		BN_bn2bin(BIGNUM *a, CrUINT8 *to);

BIGNUM*		BN_mpi2bn(CrUINT8 *s,CrINT32 len,BIGNUM *ret);

CrINT32		BN_bn2mpi(BIGNUM *a, CrUINT8 *to);

CrINT32		BN_sub(BIGNUM *r, BIGNUM *a, BIGNUM *b);

void		bn_qsub(BIGNUM *r, BIGNUM *a, BIGNUM *b);

void		bn_qadd(BIGNUM *r, BIGNUM *a, BIGNUM *b);

CrINT32		BN_add(BIGNUM *r, BIGNUM *a, BIGNUM *b);

CrINT32		BN_mod(BIGNUM *rem, BIGNUM *m, BIGNUM *d, BN_CTX *ctx);

CrINT32		BN_div(BIGNUM *dv, BIGNUM *rem, BIGNUM *m, BIGNUM *d, BN_CTX *ctx);

CrINT32		BN_mul(BIGNUM *r, BIGNUM *a, BIGNUM *b);

CrINT32		BN_sqr(BIGNUM *r, BIGNUM *a,BN_CTX *ctx);

BN_ULONG 	BN_mod_word(BIGNUM *a, CrULONG32 w);

BN_ULONG 	BN_div_word(BIGNUM *a, CrULONG32 w);

CrINT32		BN_mul_word(BIGNUM *a, CrULONG32 w);

CrINT32		BN_add_word(BIGNUM *a, CrULONG32 w);

CrINT32		BN_sub_word(BIGNUM *a, CrULONG32 w);

CrINT32		BN_set_word(BIGNUM *a, CrULONG32 w);

CrULONG32 	BN_get_word(BIGNUM *a);

CrINT32		BN_cmp(BIGNUM *a, BIGNUM *b);

void		BN_free(BIGNUM *a);

CrINT32		BN_is_bit_set(BIGNUM *a, CrINT32 n);

CrINT32		BN_lshift(BIGNUM *r, BIGNUM *a, CrINT32 n);

CrINT32		BN_lshift1(BIGNUM *r, BIGNUM *a);

CrINT32		BN_exp(BIGNUM *r, BIGNUM *a, BIGNUM *p,BN_CTX *ctx);

CrINT32		BN_mod_exp(BIGNUM *r, BIGNUM *a, BIGNUM *p, BIGNUM *m,BN_CTX *ctx);

CrINT32		BN_mod_exp_mont(BIGNUM *r, BIGNUM *a, BIGNUM *p, BIGNUM *m,BN_CTX *ctx,

							BN_MONT_CTX *m_ctx);

CrINT32		BN_mod_exp_recp(BIGNUM *r, BIGNUM *a, BIGNUM *p, BIGNUM *m,BN_CTX *ctx);

CrINT32		BN_mod_exp_simple(BIGNUM *r, BIGNUM *a, BIGNUM *p,

							BIGNUM *m,BN_CTX *ctx);

CrINT32		BN_mask_bits(BIGNUM *a,CrINT32 n);

CrINT32		BN_mod_mul_reciprocal(BIGNUM *r, BIGNUM *x, BIGNUM *y, BIGNUM *m, 

								BIGNUM *i, CrINT32 nb, BN_CTX *ctx);

CrINT32		BN_mod_mul(BIGNUM *ret, BIGNUM *a, BIGNUM *b, BIGNUM *m,

						BN_CTX *ctx);

CrINT32 	BN_mod_sqrt(BIGNUM* sqrt, BIGNUM* g, BIGNUM* m, BN_CTX* ctx);

CrINT32 	BN_print(CrINT8 *mesg, BIGNUM *a);

CrINT32		BN_reciprocal(BIGNUM *r, BIGNUM *m, BN_CTX *ctx);

CrINT32		BN_rshift(BIGNUM *r, BIGNUM *a, CrINT32 n);

CrINT32		BN_rshift1(BIGNUM *r, BIGNUM *a);

void			BN_clear(BIGNUM *a);

BIGNUM*		bn_expand2(BIGNUM *b, CrINT32 words);

BIGNUM*		BN_dup(BIGNUM *a);

CrINT32		BN_ucmp(BIGNUM *a, BIGNUM *b);

CrINT32		BN_set_bit(BIGNUM *a, CrINT32 n);

CrINT32		BN_clear_bit(BIGNUM *a, CrINT32 n);

CrINT8*		BN_bn2hex(BIGNUM *a);

CrINT8*		BN_bn2dec(BIGNUM *a);

CrINT32 	BN_hex2bn(BIGNUM **a,CrINT8 *str);

CrINT32 	BN_dec2bn(BIGNUM **a,CrINT8 *str);

CrINT32		BN_gcd(BIGNUM *r,BIGNUM *in_a,BIGNUM *in_b,BN_CTX *ctx);

BIGNUM*		BN_mod_inverse(BIGNUM *a, BIGNUM *n,BN_CTX *ctx);

CrINT32		BN_mod_inverse_kyung(BIGNUM *inverse,BIGNUM *a, BIGNUM *n,BN_CTX *ctx);

BIGNUM*		BN_generate_prime(CrINT32 bits,CrINT32 strong,BIGNUM *add,

							BIGNUM *rem,void (*callback)(CrINT32,CrINT32,CrINT8 *),CrINT8 *cb_arg);

CrINT32		BN_is_prime(BIGNUM *p,CrINT32 nchecks,void (*callback)(CrINT32,CrINT32,CrINT8 *),

						BN_CTX *ctx,CrINT8 *cb_arg);

void		ERR_load_BN_strings(void );



BN_ULONG 	bn_mul_add_words(BN_ULONG *rp, BN_ULONG *ap, CrINT32 num, BN_ULONG w);

BN_ULONG 	bn_mul_words(BN_ULONG *rp, BN_ULONG *ap, CrINT32 num, BN_ULONG w);

void     	bn_sqr_words(BN_ULONG *rp, BN_ULONG *ap, CrINT32 num);

BN_ULONG 	bn_div64(BN_ULONG h, BN_ULONG l, BN_ULONG d);

BN_ULONG 	bn_add_words(BN_ULONG *rp, BN_ULONG *ap, BN_ULONG *bp,CrINT32 num);



BN_MONT_CTX*	BN_MONT_CTX_new(void );

CrINT32 		BN_mod_mul_montgomery(BIGNUM *r,BIGNUM *a,BIGNUM *b,BN_MONT_CTX *mont,

									BN_CTX *ctx);

CrINT32 		BN_from_montgomery(BIGNUM *r,BIGNUM *a,BN_MONT_CTX *mont,BN_CTX *ctx);

void 			BN_MONT_CTX_free(BN_MONT_CTX *mont);

CrINT32 		BN_MONT_CTX_set(BN_MONT_CTX *mont,BIGNUM *modulus,BN_CTX *ctx);



BN_BLINDING*	BN_BLINDING_new(BIGNUM *A,BIGNUM *Ai,BIGNUM *mod);

void 			BN_BLINDING_free(BN_BLINDING *b);

CrINT32 		BN_BLINDING_update(BN_BLINDING *b,BN_CTX *ctx);

CrINT32 		BN_BLINDING_convert(BIGNUM *n, BN_BLINDING *r, BN_CTX *ctx);

CrINT32 		BN_BLINDING_invert(BIGNUM *n, BN_BLINDING *b, BN_CTX *ctx);



#else // #ifndef NOPROTO



BIGNUM*		BN_value_one();

CrINT8*		BN_options();

BN_CTX*		BN_CTX_new();

void		BN_CTX_free();

CrINT32     BN_rand();

CrINT32		BN_num_bits();

CrINT32		BN_num_bits_word();

BIGNUM*		BN_new();

void		BN_clear_free();

BIGNUM*		BN_copy();

BIGNUM*		BN_bin2bn();

CrINT32		BN_bn2bin();

BIGNUM*		BN_mpi2bn();

CrINT32		BN_bn2mpi();

CrINT32		BN_sub();

void		bn_qsub();

void		bn_qadd();

CrINT32		BN_add();

CrINT32		BN_mod();

CrINT32		BN_div();

CrINT32		BN_mul();

CrINT32		BN_sqr();

BN_ULONG 	BN_mod_word();

BN_ULONG 	BN_div_word();

CrINT32		BN_add_word();

CrINT32		BN_sub_word();

CrINT32		BN_mul_word();

CrINT32		BN_set_word();

CrULONG32 	BN_get_word();

CrINT32		BN_cmp();

void		BN_free();

CrINT32		BN_is_bit_set();

CrINT32		BN_lshift();

CrINT32		BN_lshift1();

CrINT32		BN_exp();

CrINT32		BN_mod_exp();

CrINT32		BN_mod_exp_mont();

CrINT32		BN_mod_exp_recp();

CrINT32		BN_mod_exp_simple();

CrINT32		BN_mask_bits();

CrINT32		BN_mod_mul_reciprocal();

CrINT32		BN_mod_mul();

CrINT32		BN_mod_sqrt();

CrINT32		BN_print();

CrINT32		BN_reciprocal();

CrINT32		BN_rshift();

CrINT32		BN_rshift1();

void		BN_clear();

BIGNUM*		bn_expand2();

BIGNUM*		BN_dup();

CrINT32		BN_ucmp();

CrINT32		BN_set_bit();

CrINT32		BN_clear_bit();

CrINT8*		BN_bn2hex();

CrINT8*		BN_bn2dec();

CrINT32 	BN_hex2bn();

CrINT32 	BN_dec2bn();

CrINT32		BN_gcd();

BIGNUM*		BN_mod_inverse();

CrINT32		BN_mod_inverse_kyung();

BIGNUM*		BN_generate_prime();

CrINT32		BN_is_prime();

void		ERR_load_BN_strings();



BN_ULONG 	bn_mul_add_words();

BN_ULONG 	bn_mul_words();

void     	bn_sqr_words();

BN_ULONG 	bn_div64();

BN_ULONG 	bn_add_words();



CrINT32 		BN_mod_mul_montgomery();

CrINT32 		BN_from_montgomery();

BN_MONT_CTX*	BN_MONT_CTX_new();

void 			BN_MONT_CTX_free();

CrINT32 		BN_MONT_CTX_set();



BN_BLINDING*	BN_BLINDING_new();

void			BN_BLINDING_free();

CrINT32		BN_BLINDING_update();

CrINT32		BN_BLINDING_convert();

CrINT32		BN_BLINDING_invert();



#endif // #ifndef NOPROTO



/* BEGIN ERROR CODES */

/* Error codes for the BN functions. */



#define eBNError						-5000



#define eBNBadTopValue			eBNError-1

#define eBNDivisionOverflow		eBNBadTopValue-1







/* Function codes. */

#define BN_F_BN_BLINDING_CONVERT		100

#define BN_F_BN_BLINDING_INVERT			101

#define BN_F_BN_BLINDING_NEW			102

#define BN_F_BN_BLINDING_UPDATE			103

#define BN_F_BN_BN2DEC					104

#define BN_F_BN_BN2HEX					105

#define BN_F_BN_CTX_NEW					106

#define BN_F_BN_DIV						107

#define BN_F_BN_EXPAND2					108

#define BN_F_BN_MOD_EXP_MONT			109

#define BN_F_BN_MOD_INVERSE				110

#define BN_F_BN_MOD_MUL_RECIPROCAL		111

#define BN_F_BN_MPI2BN					112

#define BN_F_BN_NEW						113

#define BN_F_BN_RAND					114



/* Reason codes. */

#define BN_R_BAD_RECIPROCAL				100

#define BN_R_CALLED_WITH_EVEN_MODULUS	101

#define BN_R_DIV_BY_ZERO				102

#define BN_R_ENCODING_ERROR				103

#define BN_R_INVALID_LENGTH				104

#define BN_R_NOT_INITALISED				105

#define BN_R_NO_INVERSE					106



#define ERR_R_MALLOC_FAILURE			200		

 

#ifdef  __cplusplus

}

#endif



#endif //#ifndef HEADER_BN_H


