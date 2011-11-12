/* crypto/rsa/rsa.h */

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



#ifndef HEADER_RSA_H

#define HEADER_RSA_H



#ifdef  __cplusplus

extern "C" {

#endif



//#include "cryptlib.h"

#include <CrBN.h>

typedef struct rsa_meth_st RSA_METHOD;

typedef struct rsa_st

{

	/* The first parameter is used to pickup errors where

	 * this is passed instead of aEVP_PKEY, it is set to 0 */

	CrINT32 	pad;

	CrINT32 	version;

	RSA_METHOD*	meth;

	BIGNUM*		n;

	BIGNUM*		e;

	BIGNUM*		d;

	BIGNUM*		p;

	BIGNUM*		q;

	BIGNUM*		dmp1;

	BIGNUM*		dmq1;

	BIGNUM*		iqmp;

	/* be carefull using this if the RSA structure is shared */

#ifdef _EX_DATA

	CRYPTO_EX_DATA ex_data;

#endif

	CrINT32 references;

	CrINT32 flags;



	/* Normally used to cached montgomery values */

	CrINT8*	method_mod_n;

	CrINT8*	method_mod_p;

	CrINT8*	method_mod_q;



	BN_BLINDING*	blinding;

} RSA;

/*RSA_METHOD structure which is the set of function pointers */

struct rsa_meth_st

{

	CrINT8*	name;

	CrINT32 (*rsa_pub_enc)(CrINT32 flen, CrUINT8 *from, CrUINT8 *to, RSA *rsa,CrINT32 padding);	//RSA_eay_public_encrypt(encryption)

	CrINT32 (*rsa_pub_dec)(CrINT32 flen, CrUINT8 *from, CrUINT8 *to, RSA *rsa,CrINT32 padding);	//RSA_eay_public_decrypt(verifying)

	CrINT32 (*rsa_priv_enc)(CrINT32 flen, CrUINT8 *from, CrUINT8 *to, RSA *rsa,CrINT32 padding);	//RSA_eay_private_encrypt(signing)

	CrINT32 (*rsa_priv_dec)(CrINT32 flen, CrUINT8 *from, CrUINT8 *to, RSA *rsa,CrINT32 padding);	//RSA_eay_private_decrypt(decryption)

	CrINT32 (*rsa_mod_exp)(BIGNUM *r0, BIGNUM *i, RSA *rsa);	//RSA_eay_mod_exp

	CrINT32 (*bn_mod_exp)(BIGNUM *r, BIGNUM *a, BIGNUM *p, BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *in_mont);	//BN_mod_exp

	CrINT32 (*init)(RSA *);	/* called at new */

	CrINT32 (*finish)(RSA *);	/* called at free */

	CrINT32 flags;			/* RSA_METHOD_FLAG_* things */

	CrINT8*	app_data;			/* may be needed! */

} ;







#define RSA_3	0x3L

#define RSA_F4	0x10001L



#define RSA_METHOD_FLAG_NO_CHECK	0x01 /* don't check pub/private match */

#define RSA_FLAG_CACHE_PUBLIC		0x02

#define RSA_FLAG_CACHE_PRIVATE		0x04

#define RSA_FLAG_BLINDING			0x08

#define RSA_FLAG_THREAD_SAFE		0x10



#define RSA_PKCS1_PADDING	1

#define RSA_SSLV23_PADDING	2

#define RSA_NO_PADDING		3



#define RSA_set_app_data(s,arg)         RSA_set_ex_data(s,0,(CrINT8 *)arg)

#define RSA_get_app_data(s)             RSA_get_ex_data(s,0)



#ifndef NOPROTO

RSA*	RSA_new(void);

RSA*	RSA_new_method(RSA_METHOD *method);

CrINT32	RSA_size(RSA *);

RSA*	RSA_generate_key(CrINT32 bits, CrULONG32 e,void

						(*callback)(CrINT32,CrINT32,CrINT8 *),CrINT8 *cb_arg);

	/* next 4 return -1 on error */

CrINT32	RSA_public_encrypt(CrINT32 flen, CrUINT8 *from,

							CrUINT8 *to, RSA *rsa,CrINT32 padding);

CrINT32	RSA_private_encrypt(CrINT32 flen, CrUINT8 *from,

							CrUINT8 *to, RSA *rsa,CrINT32 padding);

CrINT32	RSA_public_decrypt(CrINT32 flen, CrUINT8 *from, 

							CrUINT8 *to, RSA *rsa,CrINT32 padding);

CrINT32	RSA_private_decrypt(CrINT32 flen, CrUINT8 *from, 

							CrUINT8 *to, RSA *rsa,CrINT32 padding);

void	RSA_free (RSA *r);



CrINT32	RSA_flags(RSA *r);



void 	RSA_set_default_method(RSA_METHOD *meth);



/* If you have RSAref compiled in. */

RSA_METHOD*	RSA_PKCS1_RSAref(void);



/* these are the actual SSLeay RSA functions */

RSA_METHOD*	RSA_PKCS1_SSLeay(void);



void	ERR_load_RSA_strings(void );



RSA*	d2i_RSAPublicKey(RSA **a, CrUINT8 **pp, CrLONG32 length);

CrINT32	i2d_RSAPublicKey(RSA *a, CrUINT8 **pp);

RSA*	d2i_RSAPrivateKey(RSA **a, CrUINT8 **pp, CrLONG32 length);

CrINT32 i2d_RSAPrivateKey(RSA *a, CrUINT8 **pp);

#ifndef NO_FP_API

//CrINT32	RSA_print_fp(FILE *fp, RSA *r,CrINT32 offset);

#endif



#ifdef HEADER_BIO_H

CrINT32	RSA_print(BIO *bp, RSA *r,CrINT32 offset);

#endif


#if 0	/* by Sangsin Jung, cause it does not use now */
CrINT32 i2d_Netscape_RSA(RSA *a, CrUINT8 **pp, CrINT32 (*cb)());

RSA*	d2i_Netscape_RSA(RSA **a, CrUINT8 **pp, CrLONG32 length, CrINT32 (*cb)());

/* Naughty internal function required elsewhere, to handle a MS structure

 * that is the same as the netscape one :-) */

RSA*	d2i_Netscape_RSA_2(RSA **a, CrUINT8 **pp, CrLONG32 length, CrINT32 (*cb)());
#endif


/* The following 2 functions sign and verify a X509_SIG ASN1 object

 * inside PKCS#1 padded RSA encryption */

CrINT32 RSA_sign(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

				CrUINT8 *sigret, CrUINT32 *siglen, RSA *rsa);

CrINT32 RSA_verify(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

				CrUINT8 *sigbuf, CrUINT32 siglen, RSA *rsa);



CrINT32 RSA_sign_wtls(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

					CrUINT8 *sigret, CrUINT32 *siglen, RSA *rsa);

CrINT32 RSA_verify_wtls(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

					CrUINT8 *sigbuf, CrUINT32 siglen, RSA *rsa);



/* The following 2 function sign and verify a ASN1_OCTET_STRING

 * object inside PKCS#1 padded RSA encryption */

CrINT32 RSA_sign_ASN1_OCTET_STRING(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

	CrUINT8 *sigret, CrUINT32 *siglen, RSA *rsa);

CrINT32 RSA_verify_ASN1_OCTET_STRING(CrINT32 type, CrUINT8 *m, CrUINT32 m_len,

	CrUINT8 *sigbuf, CrUINT32 siglen, RSA *rsa);



CrINT32 RSA_blinding_on(RSA *rsa, BN_CTX *ctx);

void RSA_blinding_off(RSA *rsa);



CrINT32 RSA_padding_add_PKCS1_type_1(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_check_PKCS1_type_1(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_add_PKCS1_type_2(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_check_PKCS1_type_2(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_add_SSLv23(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_check_SSLv23(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_add_none(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);

CrINT32 RSA_padding_check_none(CrUINT8 *to,CrINT32 tlen,

	CrUINT8 *f,CrINT32 fl);


#if 0	/* by Sangsin Jung, cause it does not use now */
CrINT32 RSA_get_ex_new_index(CrLONG32 argl, CrINT8 *argp, CrINT32 (*new_func)(),

	CrINT32 (*dup_func)(), void (*free_func)());
#endif

CrINT32 RSA_set_ex_data(RSA *r,CrINT32 idx,CrINT8 *arg);

CrINT8 *RSA_get_ex_data(RSA *r, CrINT32 idx);



#else



RSA *	RSA_new();

RSA *	RSA_new_method();

CrINT32	RSA_size();

RSA *	RSA_generate_key();

CrINT32	RSA_public_encrypt();

CrINT32	RSA_private_encrypt();

CrINT32	RSA_public_decrypt();

CrINT32	RSA_private_decrypt();

void	RSA_free ();



CrINT32	RSA_flags();



void RSA_set_default_method();



/* RSA_METHOD *RSA_PKCS1_RSAref(); */

RSA_METHOD *RSA_PKCS1_SSLeay();



void	ERR_load_RSA_strings();



RSA *	d2i_RSAPublicKey();

CrINT32	i2d_RSAPublicKey();

RSA *	d2i_RSAPrivateKey();

CrINT32 	i2d_RSAPrivateKey();

#ifndef NO_FP_API

CrINT32	RSA_print_fp();

#endif



CrINT32	RSA_print();



CrINT32 i2d_Netscape_RSA();

RSA *d2i_Netscape_RSA();

RSA *d2i_Netscape_RSA_2();



CrINT32 RSA_sign();

CrINT32 RSA_verify();



CrINT32 RSA_sign_wtls();

CrINT32 RSA_verify_wtls();



CrINT32 RSA_sign_ASN1_OCTET_STRING();

CrINT32 RSA_verify_ASN1_OCTET_STRING();

CrINT32 RSA_blinding_on();

void RSA_blinding_off();



CrINT32 RSA_padding_add_PKCS1_type_1();

CrINT32 RSA_padding_check_PKCS1_type_1();

CrINT32 RSA_padding_add_PKCS1_type_2();

CrINT32 RSA_padding_check_PKCS1_type_2();

CrINT32 RSA_padding_add_SSLv23();

CrINT32 RSA_padding_check_SSLv23();

CrINT32 RSA_padding_add_none();

CrINT32 RSA_padding_check_none();



CrINT32 RSA_get_ex_new_index();

CrINT32 RSA_set_ex_data();

CrINT8 *RSA_get_ex_data();



#endif



/* BEGIN ERROR CODES */

/* Error codes for the RSA functions. */



/* Function codes. */

#define RSA_F_RSA_EAY_PRIVATE_DECRYPT			100

#define RSA_F_RSA_EAY_PRIVATE_ENCRYPT			101

#define RSA_F_RSA_EAY_PUBLIC_DECRYPT			102

#define RSA_F_RSA_EAY_PUBLIC_ENCRYPT			103

#define RSA_F_RSA_GENERATE_KEY					104

#define RSA_F_RSA_NEW_METHOD					105

#define RSA_F_RSA_PADDING_ADD_NONE				106

#define RSA_F_RSA_PADDING_ADD_PKCS1_TYPE_1		107

#define RSA_F_RSA_PADDING_ADD_PKCS1_TYPE_2		108

#define RSA_F_RSA_PADDING_ADD_SSLV23			109

#define RSA_F_RSA_PADDING_CHECK_NONE			110

#define RSA_F_RSA_PADDING_CHECK_PKCS1_TYPE_1	111

#define RSA_F_RSA_PADDING_CHECK_PKCS1_TYPE_2	112

#define RSA_F_RSA_PADDING_CHECK_SSLV23			113

#define RSA_F_RSA_PRINT							114

#define RSA_F_RSA_PRINT_FP						115

#define RSA_F_RSA_SIGN							116

#define RSA_F_RSA_SIGN_ASN1_OCTET_STRING		117

#define RSA_F_RSA_VERIFY						118

#define RSA_F_RSA_VERIFY_ASN1_OCTET_STRING		119



/* Reason codes. */

#define RSA_R_ALGORITHM_MISMATCH				100

#define RSA_R_BAD_E_VALUE						101

#define RSA_R_BAD_FIXED_HEADER_DECRYPT			102

#define RSA_R_BAD_PAD_BYTE_COUNT				103

#define RSA_R_BAD_SIGNATURE						104

#define RSA_R_BAD_ZERO_BYTE						105

#define RSA_R_BLOCK_TYPE_IS_NOT_01				106

#define RSA_R_BLOCK_TYPE_IS_NOT_02				107

#define RSA_R_DATA_GREATER_THAN_MOD_LEN			108

#define RSA_R_DATA_TOO_LARGE					109

#define RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE		110

#define RSA_R_DATA_TOO_SMALL					111

#define RSA_R_DIGEST_TOO_BIG_FOR_RSA_KEY		112

#define RSA_R_NULL_BEFORE_BLOCK_MISSING			113

#define RSA_R_PADDING_CHECK_FAILED				114

#define RSA_R_SSLV3_ROLLBACK_ATTACK				115

#define RSA_R_THE_ASN1_OBJECT_IDENTIFIER_IS_NOT_KNOWN_FOR_THIS_MD 116

#define RSA_R_UNKNOWN_ALGORITHM_TYPE			117

#define RSA_R_UNKNOWN_PADDING_TYPE				118

#define RSA_R_WRONG_SIGNATURE_LENGTH			119

 

#ifdef  __cplusplus

}

#endif

#endif

