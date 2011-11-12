/* crypto/evp/evp.h */

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





/*

1998. 8 EVP_snake series and data format added

1998.11 SAIT_RSA added



*/

#ifndef 	HEADER_ENVELOPE_H

#define 	HEADER_ENVELOPE_H



#ifdef	__cplusplus

extern "C" {

#endif



//#include <CrConfig.h>



#ifdef CR_MD2

#include <CrMD2.h>

#endif

#ifdef CR_MD5

#include <CrMD5.h>

#endif

#if defined(CR_SHA) || defined(CR_SHA1)

#include <CrSHA.h>

#endif

#ifdef CR_RIPEMD

#include <RIPEMD.h>

#endif

#ifdef CR_DES

#include <CrDES.h>

#endif

#ifdef CR_RC4

#include <CrRC4.h>

#endif

#ifdef CR_RC2

#include <CrRC2.h>

#endif

#ifdef CR_RC5

#include <CrRC5.h>

#endif

#ifdef CR_SEED // 2002.03.08 Taesung Kim Added.[SEED]

#include <CrSeedx.h>

#endif

#ifdef CR_IDEA

#include <CrIDEA.h>

#endif

/*

#ifdef CR_SNAKE

#define "snake.h"

#endif

*/

#ifdef CR_AES

#include <CrAES.h>

#include <CrAESAlg.h>

#include <CrAESApi.h>

#endif



#ifdef CR_BLOWFISH

#include <BlowFish.h>

#endif

#ifdef CR_CAST

#include <Cast.h>

#endif

#ifdef CR_MDC2

#include <MDC2.h>

#endif







#define EVP_RC2_KEY_SIZE		16

#define EVP_RC4_KEY_SIZE		16

#define EVP_BLOWFISH_KEY_SIZE	16

//2000.4.1 modified

#define EVP_CAST5_KEY_SIZE			16

#define EVP_RC5_32_12_16_KEY_SIZE	16

#define EVP_RC5_32_16_16_KEY_SIZE	16

#define EVP_MAX_MD_SIZE				(16+20) /* The SSLv3 md5+sha1 type */

//#define EVP_MAX_MD_SIZE		20

#define EVP_MAX_KEY_LENGTH	24

#define EVP_MAX_IV_LENGTH	8

#define EVP_MAX_BUF_LENGTH 16 // 2002.03.08 Taesung Kim Added.[SEED]



//2000.4.1 

#define PKCS5_SALT_LEN			8

/* Default PKCS#5 iteration count */

#define PKCS5_DEFAULT_ITER		2048



#ifdef CR_RSA

#include <CrRSA.h>

#else

#define RSA	CrLONG32

#endif



#ifdef CR_DSA

#include <DSA.h>

#else

#define DSA	CrLONG32

#endif



#ifdef CR_DH

#include <CrDH.h>

#else

#define DH	CrLONG32

#endif



/**

 *	2001.06.25 Removed by Taesung Kim. (OBJECT_ID_REMOVE)

 *	Object-ID is not needed in crypto module

 */

//#include <Objects.h>



#define EVP_PK_RSA		0x0001

#define EVP_PK_DSA		0x0002

#define EVP_PK_DH		0x0004

#define EVP_PKT_SIGN	0x0010

#define EVP_PKT_ENC		0x0020

#define EVP_PKT_EXCH	0x0040

#define EVP_PKS_RSA		0x0100

#define EVP_PKS_DSA		0x0200

#define EVP_PKT_EXP		0x1000 /* <= 512 bit key */



#define EVP_PKEY_NONE	NID_undef

#define EVP_PKEY_RSA	NID_rsaEncryption

#define EVP_PKEY_RSA2	NID_rsa

#define EVP_PKEY_DSA	NID_dsa

#define EVP_PKEY_DSA2	NID_dsaWithSHA

#define EVP_PKEY_DSA3	NID_dsaWithSHA1

#define EVP_PKEY_DSA4	NID_dsaWithSHA1_2

#define EVP_PKEY_DH		NID_dhKeyAgreement



typedef struct evp_pkey_st

{

	CrINT32 type;

	CrINT32 save_type;

	CrINT32 references;

	union	

	{

		CrINT8 *ptr;

#ifdef CR_RSA

		struct rsa_st *rsa;	/* RSA */

#endif

#ifdef CR_DSA

		struct dsa_st *dsa;	/* DSA */

#endif

#ifdef CR_DH

		struct dh_st *dh;	/* DH */

#endif

	} pkey;

	CrINT32 save_parameters;

#ifdef HEADER_STACK_H

	STACK /* X509_ATTRIBUTE */ *attributes; /* [ 0 ] */

#else

	CrINT8 /* X509_ATTRIBUTE */ *attributes; /* [ 0 ] */

#endif

} EVP_PKEY;



#ifndef EVP_MD

typedef struct env_md_st

{

//	CrINT32 type;

//	CrINT32 pkey_type;

	CrINT32 md_size;

	//void (*init)();

	//void (*update)();

	//void (*final)();



//	CrINT32 (*sign)();

//	CrINT32 (*verify)();

//	CrINT32 required_pkey_type[5]; /*EVP_PKEY_xxx */

	CrINT32 block_size;

	CrINT32 ctx_size; /* how big does the ctx need to be */

} EVP_MD;



#define EVP_PKEY_NULL_method	NULL,NULL,{0,0,0,0}



#ifdef CR_DSA

#define EVP_PKEY_DSA_method	DSA_sign,DSA_verify, \

				{EVP_PKEY_DSA,EVP_PKEY_DSA2,EVP_PKEY_DSA3,0}

#else

#define EVP_PKEY_DSA_method	EVP_PKEY_NULL_method

#endif



#ifdef CR_RSA

#define EVP_PKEY_RSA_method	RSA_sign,RSA_verify, {EVP_PKEY_RSA,EVP_PKEY_RSA2,0,0}

#define EVP_PKEY_RSA_ASN1_OCTET_STRING_method RSA_sign_ASN1_OCTET_STRING, RSA_verify_ASN1_OCTET_STRING, {EVP_PKEY_RSA,EVP_PKEY_RSA2,0,0}

#else

#define EVP_PKEY_RSA_method	EVP_PKEY_NULL_method

#define EVP_PKEY_RSA_ASN1_OCTET_STRING_method EVP_PKEY_NULL_method

#endif



#endif /* !EVP_MD */



typedef struct env_md_ctx_st

{

	EVP_MD *digest;

	union	

	{

		CrUINT8 base[4];

#ifdef CR_MD2

		MD2_CTX md2;

#endif

#ifdef CR_MD5

		MD5_CTX md5;

#endif

#ifdef CR_RIPEMD5

		RIPEMD160_CTX ripemd160;

#endif

#if defined(CR_SHA) || defined(CR_SHA1)

		SHA_CTX sha;

#endif

#ifdef CR_MDC2

		MDC2_CTX mdc2;

#endif

	} md;

} EVP_MD_CTX;

/* by Sangsin Jung, dummy definition for build */
typedef int		ASN1_TYPE;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

typedef struct evp_cipher_st

{

//	CrINT32 nid;

	CrINT32 block_size;

	CrINT32 key_len;

	CrINT32 iv_len;

	void (*init)(EVP_CIPHER_CTX *ctx, CrUINT8 *key, CrUINT8 *iv,CrINT32 enc);		/* init for encryption */

	void (*do_cipher)(EVP_CIPHER_CTX *ctx, CrUINT8 *out, CrUINT8 *in, CrUINT32 inl);	/* encrypt data */

	void (*cleanup)(EVP_CIPHER_CTX *a);	/* used by cipher method */ 

	CrINT32 ctx_size;		/* how big the ctx needs to be */

	/* CrINT32 set_asn1_parameters(EVP_CIPHER_CTX,ASN1_TYPE *); */

	CrINT32 (*set_asn1_parameters)(EVP_CIPHER_CTX *c, ASN1_TYPE *type); /* Populate a ASN1_TYPE with parameters */

	/* CrINT32 get_asn1_parameters(EVP_CIPHER_CTX,ASN1_TYPE *); */

	CrINT32 (*get_asn1_parameters)(EVP_CIPHER_CTX *c, ASN1_TYPE *type); /* Get parameters from a ASN1_TYPE */

} EVP_CIPHER;



typedef struct evp_cipher_info_st

{

	EVP_CIPHER *cipher;

	CrUINT8 iv[EVP_MAX_IV_LENGTH];

} EVP_CIPHER_INFO;



struct evp_cipher_ctx_st

{

	EVP_CIPHER*	cipher;

	CrINT32		encrypt;		/* encrypt or decrypt */

	CrINT32		buf_len;		/* number we have left */

	CrUINT8		oiv[EVP_MAX_IV_LENGTH];	/* original iv */

	CrUINT8		iv[EVP_MAX_IV_LENGTH];	/* working iv */

	CrUINT8		buf[EVP_MAX_BUF_LENGTH];	/* saved partial block */

	CrINT32		num;				/* used by cfb/ofb mode */

	CrINT8*		app_data;		/* aplication stuff */



	union

	{

#ifdef CR_RC4

		struct

		{

			CrUINT8 key[EVP_RC4_KEY_SIZE];

			RC4_KEY ks;	/* working key */

		} rc4;

#endif

#ifdef CR_SNAKE

		struct

		{

			CrUINT8  oiv[8];			/* original iv */

			CrUINT8  iv[8];				/* working iv */

			snake_key_schedule sks;	/* key schedule */

		} snake_cbc;

#endif 

#ifdef CR_DES

		des_key_schedule des_ks;/* key schedule */

		struct

		{

			des_key_schedule ks;/* key schedule */

			C_Block inw;

			C_Block outw;

		} desx_cbc;

		struct

		{

			des_key_schedule ks1;/* key schedule */

			des_key_schedule ks2;/* key schedule (for ede) */

			des_key_schedule ks3;/* key schedule (for ede3) */

		} des_ede;

#endif

#ifdef CR_IDEA

		IDEA_KEY_SCHEDULE idea_ks;/* key schedule */

#endif

#ifdef CR_RC2

		RC2_KEY rc2_ks;/* key schedule */

#endif

#ifdef CR_RC5

		RC5_32_KEY rc5_ks;/* key schedule */

#endif

#ifdef CR_BLOWFISH

		BF_KEY bf_ks;/* key schedule */

#endif

#ifdef CR_CAST

		CAST_KEY cast_ks;/* key schedule */

#endif

#ifdef CR_SEED // 2002.03.08 Taesung Kim Added.[SEED]

		struct

		{

			seed_key_schedule ks;

		} seed_ecb;



		struct

		{

			CrUINT8				oiv[16];

			CrUINT8				iv[16];

			seed_key_schedule	ks;

		} seed_cbc;



		struct

		{

			CrUINT8				oiv[16];

			CrUINT8				iv[16];

			seed_key_schedule	ks;

			CrINT32				num;

		} seed_cfb;



		struct

		{

			CrUINT8				oiv[16];

			CrUINT8				iv[16];

			seed_key_schedule	ks;

			CrINT32				num;

		} seed_ofb;

#endif

#ifdef CR_AES

		struct {

			keyInstance		ki; /* key schedule */

			cipherInstance	ci;

		} aes;

#endif

	} c;

} ;



typedef struct evp_Encode_Ctx_st

{

	CrINT32 num;	/* number saved in a partial encode/decode */

	CrINT32 length;	/* The length is either the output line length

			 * (in input bytes) or the shortest input line

			 * length that is ok.  Once decoding begins,

			 * the length is adjusted up each time a longer

			 * line is decoded */

	CrUINT8 enc_data[80];	/* data to encode */

	CrINT32 line_num;	/* number read on current line */

	CrINT32 expect_nl;

} EVP_ENCODE_CTX;



#define EVP_PKEY_assign_RSA(pkey,rsa) EVP_PKEY_assign((pkey),EVP_PKEY_RSA, (CrINT8 *)(rsa))

#define EVP_PKEY_assign_DSA(pkey,dsa) EVP_PKEY_assign((pkey),EVP_PKEY_DSA, (CrINT8 *)(dsa))

#define EVP_PKEY_assign_DH(pkey,dh) EVP_PKEY_assign((pkey),EVP_PKEY_DH, (CrINT8 *)(dh))



/* Add some extra combinations */

#define EVP_get_digestbynid(a) EVP_get_digestbyname(OBJ_nid2sn(a))

#define EVP_get_digestbyobj(a) EVP_get_digestbynid(OBJ_obj2nid(a))

#define EVP_get_cipherbynid(a) EVP_get_cipherbyname(OBJ_nid2sn(a))

#define EVP_get_cipherbyobj(a) EVP_get_cipherbynid(OBJ_obj2nid(a))



#define EVP_MD_type(e)			((e)->type)

#define EVP_MD_pkey_type(e)		((e)->pkey_type)

#define EVP_MD_size(e)			((e)->md_size)

#define EVP_MD_block_size(e)		((e)->block_size)



#define EVP_MD_CTX_size(e)		EVP_MD_size((e)->digest)

#define EVP_MD_CTX_block_size(e)	EVP_MD_block_size((e)->digest)

#define EVP_MD_CTX_type(e)		((e)->digest)



#define EVP_CIPHER_nid(e)		((e)->nid)

#define EVP_CIPHER_block_size(e)	((e)->block_size)

#define EVP_CIPHER_key_length(e)	((e)->key_len)

#define EVP_CIPHER_iv_length(e)		((e)->iv_len)



#define EVP_CIPHER_CTX_cipher(e)	((e)->cipher)

#define EVP_CIPHER_CTX_nid(e)		((e)->cipher->nid)

#define EVP_CIPHER_CTX_block_size(e)	((e)->cipher->block_size)

#define EVP_CIPHER_CTX_key_length(e)	((e)->cipher->key_len)

#define EVP_CIPHER_CTX_iv_length(e)	((e)->cipher->iv_len)

#define EVP_CIPHER_CTX_get_app_data(e)	((e)->app_data)

#define EVP_CIPHER_CTX_set_app_data(e,d) ((e)->app_data=(CrINT8 *)(d))



#define EVP_ENCODE_LENGTH(l)	(((l+2)/3*4)+(l/48+1)*2+80)

#define EVP_DECODE_LENGTH(l)	((l+3)/4*3+80)



#define EVP_SignInit(a,b)		EVP_DigestInit(a,b)

#define EVP_SignUpdate(a,b,c)		EVP_DigestUpdate(a,b,c)

#define	EVP_VerifyInit(a,b)		EVP_DigestInit(a,b)

#define	EVP_VerifyUpdate(a,b,c)		EVP_DigestUpdate(a,b,c)

#define EVP_OpenUpdate(a,b,c,d,e)	EVP_DecryptUpdate(a,b,c,d,e)

#define EVP_SealUpdate(a,b,c,d,e)	EVP_EncryptUpdate(a,b,c,d,e)	



#define BIO_set_md(b,md)		BIO_ctrl(b,BIO_C_SET_MD,0,(CrINT8 *)md)

#define BIO_get_md(b,mdp)		BIO_ctrl(b,BIO_C_GET_MD,0,(CrINT8 *)mdp)

#define BIO_get_md_ctx(b,mdcp)     BIO_ctrl(b,BIO_C_GET_MD_CTX,0,(CrINT8 *)mdcp)

#define BIO_get_cipher_status(b)	BIO_ctrl(b,BIO_C_GET_CIPHER_STATUS,0,NULL)



#define	EVP_Cipher(c,o,i,l)	(c)->cipher->do_cipher((c),(o),(i),(l))



#ifndef NOPROTO



void	EVP_DigestInit(EVP_MD_CTX *ctx, EVP_MD *type);

void	EVP_DigestUpdate(EVP_MD_CTX *ctx,CrUINT8 *d,CrUINT32 cnt);

void	EVP_DigestFinal(EVP_MD_CTX *ctx,CrUINT8 *md,CrUINT32 *s);



CrINT32	EVP_read_pw_string(CrINT8 *buf,CrINT32 length,CrINT8 *prompt,CrINT32 verify);

void	EVP_set_pw_prompt(CrINT8 *prompt);

CrINT8 *	EVP_get_pw_prompt(void);



CrINT32	EVP_BytesToKey(EVP_CIPHER *type,EVP_MD *md,CrUINT8 *salt,

		CrUINT8 *data, CrINT32 datal, CrINT32 count,

		CrUINT8 *key,CrUINT8 *iv);



EVP_CIPHER *EVP_get_cipherbyname(CrINT8 *name);



void	EVP_EncryptInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,

		CrUINT8 *key, CrUINT8 *iv);

void	EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, CrUINT8 *out,

		CrINT32 *outl, CrUINT8 *in, CrINT32 inl);

void	EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, CrUINT8 *out, CrINT32 *outl);



void	EVP_DecryptInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,

		CrUINT8 *key, CrUINT8 *iv);

void	EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, CrUINT8 *out,

		CrINT32 *outl, CrUINT8 *in, CrINT32 inl);

CrINT32	EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, CrUINT8 *outm, CrINT32 *outl);



void	EVP_CipherInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type, CrUINT8 *key,

		CrUINT8 *iv,CrINT32 enc);

void	EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, CrUINT8 *out,

		CrINT32 *outl, CrUINT8 *in, CrINT32 inl);

CrINT32	EVP_CipherFinal(EVP_CIPHER_CTX *ctx, CrUINT8 *outm, CrINT32 *outl);



CrINT32	EVP_SignFinal(EVP_MD_CTX *ctx,CrUINT8 *md,CrUINT32 *s,

		EVP_PKEY *pkey);



CrINT32	EVP_VerifyFinal(EVP_MD_CTX *ctx,CrUINT8 *sigbuf,

		CrUINT32 siglen,EVP_PKEY *pkey);



CrINT32	EVP_OpenInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,CrUINT8 *ek,

		CrINT32 ekl,CrUINT8 *iv,EVP_PKEY *priv);

CrINT32	EVP_OpenFinal(EVP_CIPHER_CTX *ctx, CrUINT8 *out, CrINT32 *outl);



CrINT32	EVP_SealInit(EVP_CIPHER_CTX *ctx, EVP_CIPHER *type, CrUINT8 **ek,

		CrINT32 *ekl, CrUINT8 *iv,EVP_PKEY **pubk, CrINT32 npubk);

void	EVP_SealFinal(EVP_CIPHER_CTX *ctx,CrUINT8 *out,CrINT32 *outl);



void	EVP_EncodeInit(EVP_ENCODE_CTX *ctx);

void	EVP_EncodeUpdate(EVP_ENCODE_CTX *ctx,CrUINT8 *out,

		CrINT32 *outl,CrUINT8 *in,CrINT32 inl);

void	EVP_EncodeFinal(EVP_ENCODE_CTX *ctx,CrUINT8 *out,CrINT32 *outl);

CrINT32	EVP_EncodeBlock(CrUINT8 *t, CrUINT8 *f, CrINT32 n);



void	EVP_DecodeInit(EVP_ENCODE_CTX *ctx);

CrINT32	EVP_DecodeUpdate(EVP_ENCODE_CTX *ctx,CrUINT8 *out,CrINT32 *outl,

						CrUINT8 *in, CrINT32 inl);

CrINT32	EVP_DecodeFinal(EVP_ENCODE_CTX *ctx, CrUINT8 *out, CrINT32 *outl);

CrINT32	EVP_DecodeBlock(CrUINT8 *t, CrUINT8 *f, CrINT32 n);



void	ERR_load_EVP_strings(void );



void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *a);

void EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *a);



#ifdef HEADER_BIO_H

BIO_METHOD *BIO_f_md(void);

BIO_METHOD *BIO_f_base64(void);

BIO_METHOD *BIO_f_cipher(void);

void BIO_set_cipher(BIO *b,EVP_CIPHER *c,CrUINT8 *k,

	CrUINT8 *i, CrINT32 enc);

#endif



EVP_MD *EVP_md_null(void);

EVP_MD *EVP_md2(void);

EVP_MD *EVP_md5(void);

EVP_MD *EVP_sha(void);

EVP_MD *EVP_sha1(void);

EVP_MD *EVP_dss(void);

EVP_MD *EVP_dss1(void);

EVP_MD *EVP_mdc2(void);

EVP_MD *EVP_ripemd160(void);



EVP_CIPHER *EVP_enc_null(void);		/* does nothing :-) */

EVP_CIPHER *EVP_des_ecb(void);

EVP_CIPHER *EVP_des_ede(void);

EVP_CIPHER *EVP_des_ede3(void);

EVP_CIPHER *EVP_des_cfb(void);

EVP_CIPHER *EVP_des_ede_cfb(void);

EVP_CIPHER *EVP_des_ede3_cfb(void);

EVP_CIPHER *EVP_des_ofb(void);

EVP_CIPHER *EVP_des_ede_ofb(void);

EVP_CIPHER *EVP_des_ede3_ofb(void);

EVP_CIPHER *EVP_des_cbc(void);

EVP_CIPHER *EVP_des_ede_cbc(void);

EVP_CIPHER *EVP_des_ede3_cbc(void);

EVP_CIPHER *EVP_desx_cbc(void);

EVP_CIPHER *EVP_rc4(void);

EVP_CIPHER *EVP_rc4_40(void);

EVP_CIPHER *EVP_idea_ecb(void);

EVP_CIPHER *EVP_idea_cfb(void);

EVP_CIPHER *EVP_idea_ofb(void);

EVP_CIPHER *EVP_idea_cbc(void);

EVP_CIPHER *EVP_rc2_ecb(void);

EVP_CIPHER *EVP_rc2_cbc(void);

EVP_CIPHER *EVP_rc2_40_cbc(void);

EVP_CIPHER *EVP_rc2_cfb(void);

EVP_CIPHER *EVP_rc2_ofb(void);

EVP_CIPHER *EVP_bf_ecb(void);

EVP_CIPHER *EVP_bf_cbc(void);

EVP_CIPHER *EVP_bf_cfb(void);

EVP_CIPHER *EVP_bf_ofb(void);

EVP_CIPHER *EVP_cast5_ecb(void);

EVP_CIPHER *EVP_cast5_cbc(void);

EVP_CIPHER *EVP_cast5_cfb(void);

EVP_CIPHER *EVP_cast5_ofb(void);



//kijung add

EVP_CIPHER *EVP_snake_cbc(void);



//2000.4.14 KyungIm Jung Added

EVP_CIPHER *EVP_rc5_32_12_16_cbc(void);

EVP_CIPHER *EVP_rc5_32_12_16_ecb(void);

EVP_CIPHER *EVP_rc5_32_12_16_cfb(void);

EVP_CIPHER *EVP_rc5_32_12_16_ofb(void);



//2000.5.13 KyungIm Jung Added

EVP_CIPHER *EVP_rc5_32_16_16_cbc(void);



// 2002.02.23 Taesung Kim Added.[SEED]

EVP_CIPHER *EVP_seed_ecb(void);

EVP_CIPHER *EVP_seed_cbc(void);

EVP_CIPHER *EVP_seed_cfb(void);

EVP_CIPHER *EVP_seed_ofb(void);



//2002.4.30 Taesung Kim Added.[AES]

EVP_CIPHER* EVP_aes_cbc_128(void);

EVP_CIPHER* EVP_aes_cbc_192(void);

EVP_CIPHER* EVP_aes_cbc_256(void);

EVP_CIPHER* EVP_aes_ecb_128(void);

EVP_CIPHER* EVP_aes_ecb_192(void);

EVP_CIPHER* EVP_aes_ecb_256(void);

EVP_CIPHER* EVP_aes_cfb1_128(void);

EVP_CIPHER* EVP_aes_cfb1_192(void);

EVP_CIPHER* EVP_aes_cfb1_256(void);





void SSLeay_add_all_algorithms(void);

void SSLeay_add_all_ciphers(void);

void SSLeay_add_all_digests(void);



CrINT32 EVP_add_cipher(EVP_CIPHER *cipher);

CrINT32 EVP_add_digest(EVP_MD *digest);

CrINT32 EVP_add_alias(CrINT8 *name,CrINT8 *alias);

CrINT32 EVP_delete_alias(CrINT8 *name);



EVP_CIPHER *EVP_get_cipherbyname(CrINT8 *name);

EVP_MD *EVP_get_digestbyname(CrINT8 *name);

void EVP_cleanup(void);



CrINT32		EVP_PKEY_decrypt(CrUINT8 *dec_key,CrUINT8 *enc_key,

			CrINT32 enc_key_len,EVP_PKEY *private_key);

CrINT32		EVP_PKEY_encrypt(CrUINT8 *enc_key,

			CrUINT8 *key,CrINT32 key_len,EVP_PKEY *pub_key);

CrINT32		EVP_PKEY_type(CrINT32 type);

CrINT32		EVP_PKEY_bits(EVP_PKEY *pkey);

CrINT32		EVP_PKEY_size(EVP_PKEY *pkey);

CrINT32 	EVP_PKEY_assign(EVP_PKEY *pkey,CrINT32 type,CrINT8 *key);

EVP_PKEY*	EVP_PKEY_new(void);

void		EVP_PKEY_free(EVP_PKEY *pkey);

EVP_PKEY*	d2i_PublicKey(CrINT32 type,EVP_PKEY **a, CrUINT8 **pp,

			CrLONG32 length);

CrINT32		i2d_PublicKey(EVP_PKEY *a, CrUINT8 **pp);



EVP_PKEY*	d2i_PrivateKey(CrINT32 type,EVP_PKEY **a, CrUINT8 **pp,

			CrLONG32 length);

CrINT32		i2d_PrivateKey(EVP_PKEY *a, CrUINT8 **pp);



CrINT32 	EVP_PKEY_copy_parameters(EVP_PKEY *to,EVP_PKEY *from);

CrINT32 	EVP_PKEY_missing_parameters(EVP_PKEY *pkey);

CrINT32 	EVP_PKEY_save_parameters(EVP_PKEY *pkey,CrINT32 mode);

CrINT32 	EVP_PKEY_cmp_parameters(EVP_PKEY *a,EVP_PKEY *b);



#if 0

/* calls methods */

CrINT32 EVP_CIPHER_param_to_asn1(EVP_CIPHER_CTX *c, ASN1_TYPE *type);

CrINT32 EVP_CIPHER_asn1_to_param(EVP_CIPHER_CTX *c, ASN1_TYPE *type);



/* These are used by EVP_CIPHER methods */

CrINT32 EVP_CIPHER_set_asn1_iv(EVP_CIPHER_CTX *c,ASN1_TYPE *type);

CrINT32 EVP_CIPHER_get_asn1_iv(EVP_CIPHER_CTX *c,ASN1_TYPE *type);

#endif // #if 0



#else



void		EVP_DigestInit();

void		EVP_DigestUpdate();

void		EVP_DigestFinal();



CrINT32		EVP_read_pw_string();

void		EVP_set_pw_prompt();

CrINT8*		EVP_get_pw_prompt();



CrINT32		EVP_BytesToKey();



EVP_CIPHER*	EVP_get_cipherbyname();



void		EVP_EncryptInit();

void		EVP_EncryptUpdate();

void		EVP_EncryptFinal();



void		EVP_DecryptInit();

void		EVP_DecryptUpdate();

CrINT32		EVP_DecryptFinal();



void		EVP_CipherInit();

void		EVP_CipherUpdate();

CrINT32		EVP_CipherFinal();



CrINT32		EVP_SignFinal();



CrINT32		EVP_VerifyFinal();



CrINT32		EVP_OpenInit();

CrINT32		EVP_OpenFinal();



CrINT32		EVP_SealInit();

void		EVP_SealFinal();



void		EVP_EncodeInit();

void		EVP_EncodeUpdate();

void		EVP_EncodeFinal();

CrINT32		EVP_EncodeBlock();



void		EVP_DecodeInit();

CrINT32		EVP_DecodeUpdate();

CrINT32		EVP_DecodeFinal();

CrINT32		EVP_DecodeBlock();



void		ERR_load_EVP_strings();



void 		EVP_CIPHER_CTX_init();

void 		EVP_CIPHER_CTX_cleanup();



#ifdef HEADER_BIO_H

BIO_METHOD*	BIO_f_md();

BIO_METHOD*	BIO_f_base64();

BIO_METHOD*	BIO_f_cipher();

void 		BIO_set_cipher();

#endif



EVP_MD*		EVP_md_null();

EVP_MD*		EVP_md2();

EVP_MD*		EVP_md5();

EVP_MD*		EVP_sha();

EVP_MD*		EVP_sha1();

EVP_MD*		EVP_dss();

EVP_MD*		EVP_dss1();

EVP_MD*		EVP_mdc2();



EVP_CIPHER*	EVP_enc_null();

EVP_CIPHER*	EVP_des_ecb();

EVP_CIPHER*	EVP_des_ede();

EVP_CIPHER*	EVP_des_ede3();

EVP_CIPHER*	EVP_des_cfb();

EVP_CIPHER*	EVP_des_ede_cfb();

EVP_CIPHER*	EVP_des_ede3_cfb();

EVP_CIPHER*	EVP_des_ofb();

EVP_CIPHER*	EVP_des_ede_ofb();

EVP_CIPHER*	EVP_des_ede3_ofb();

EVP_CIPHER*	EVP_des_cbc();

EVP_CIPHER*	EVP_des_ede_cbc();

EVP_CIPHER*	EVP_des_ede3_cbc();

EVP_CIPHER*	EVP_desx_cbc();

EVP_CIPHER*	EVP_rc4();

EVP_CIPHER*	EVP_rc4_40();

EVP_CIPHER*	EVP_idea_ecb();

EVP_CIPHER*	EVP_idea_cfb();

EVP_CIPHER*	EVP_idea_ofb();

EVP_CIPHER*	EVP_idea_cbc();

EVP_CIPHER*	EVP_rc2_ecb();

EVP_CIPHER*	EVP_rc2_cbc();

EVP_CIPHER*	EVP_rc2_40_cbc();

EVP_CIPHER*	EVP_rc2_cfb();

EVP_CIPHER*	EVP_rc2_ofb();

EVP_CIPHER*	EVP_bf_ecb();

EVP_CIPHER*	EVP_bf_cbc();

EVP_CIPHER*	EVP_bf_cfb();

EVP_CIPHER*	EVP_bf_ofb();

EVP_CIPHER*	EVP_cast5_ecb();

EVP_CIPHER*	EVP_cast5_cbc();

EVP_CIPHER*	EVP_cast5_cfb();

EVP_CIPHER*	EVP_cast5_ofb();



//kijung add

EVP_CIPHER*	EVP_snake_cbc();



//2000.4.14 KyungIm Jung Added

EVP_CIPHER*	EVP_rc5_32_12_16_cbc();

EVP_CIPHER*	EVP_rc5_32_12_16_ecb();

EVP_CIPHER*	EVP_rc5_32_12_16_cfb();

EVP_CIPHER*	EVP_rc5_32_12_16_ofb();



//2000.5.13 KyungIm Jung Added

EVP_CIPHER*	EVP_rc5_32_16_16_cbc();



// 2002.02.23 Taesung Kim Added.[SEED]

EVP_CIPHER *EVP_seed_ecb();

EVP_CIPHER *EVP_seed_cbc();

EVP_CIPHER *EVP_seed_cfb();

EVP_CIPHER *EVP_seed_ofb();



//2002.4.30 Taesung Kim Added.[AES]

EVP_CIPHER* EVP_aes_cbc_128();

EVP_CIPHER* EVP_aes_cbc_192();

EVP_CIPHER* EVP_aes_cbc_256();

EVP_CIPHER* EVP_aes_ecb_128();

EVP_CIPHER* EVP_aes_ecb_192();

EVP_CIPHER* EVP_aes_ecb_256();

EVP_CIPHER* EVP_aes_cfb1_128();

EVP_CIPHER* EVP_aes_cfb1_192();

EVP_CIPHER* EVP_aes_cfb1_256();



void SSLeay_add_all_algorithms();

void SSLeay_add_all_ciphers();

void SSLeay_add_all_digests();



CrINT32 EVP_add_cipher();

CrINT32 EVP_add_digest();

CrINT32 EVP_add_alias();

CrINT32 EVP_delete_alias();



EVP_CIPHER *EVP_get_cipherbyname();

EVP_MD *EVP_get_digestbyname();

void EVP_cleanup();



CrINT32		EVP_PKEY_decrypt();

CrINT32		EVP_PKEY_encrypt();

CrINT32		EVP_PKEY_type();

CrINT32		EVP_PKEY_bits();

CrINT32		EVP_PKEY_size();

CrINT32 		EVP_PKEY_assign();

EVP_PKEY*	EVP_PKEY_new();

void		EVP_PKEY_free();

EVP_PKEY*	d2i_PublicKey();

CrINT32		i2d_PublicKey();



EVP_PKEY*	d2i_PrivateKey();

CrINT32		i2d_PrivateKey();



CrINT32 	EVP_PKEY_copy_parameters();

CrINT32 	EVP_PKEY_missing_parameters();

CrINT32 	EVP_PKEY_save_parameters();

CrINT32 	EVP_PKEY_cmp_parameters();



#if 0

CrINT32 EVP_CIPHER_param_to_asn1(EVP_CIPHER_CTX *c, ASN1_TYPE *type);

CrINT32 EVP_CIPHER_asn1_to_param(EVP_CIPHER_CTX *c, ASN1_TYPE *type);



CrINT32 EVP_CIPHER_set_asn1_iv();

CrINT32 EVP_CIPHER_get_asn1_iv();

#endif // #if 0

#endif



/* BEGIN ERROR CODES */

/* Error codes for the EVP functions. */



/* Function codes. */

#define EVP_F_D2I_PKEY					 		100

#define EVP_F_EVP_DECRYPTFINAL				 	101

#define EVP_F_EVP_OPENINIT				 		102

#define EVP_F_EVP_PKEY_COPY_PARAMETERS			103

#define EVP_F_EVP_PKEY_DECRYPT				 	104

#define EVP_F_EVP_PKEY_ENCRYPT				 	105

#define EVP_F_EVP_PKEY_NEW				 		106

#define EVP_F_EVP_SIGNFINAL				 		107	

#define EVP_F_EVP_VERIFYFINAL				 	108



/* Reason codes. */

#define EVP_R_BAD_DECRYPT				 		100

#define EVP_R_DIFFERENT_KEY_TYPES			 	101

#define EVP_R_IV_TOO_LARGE				 		102

#define EVP_R_MISSING_PARMATERS				 	103

#define EVP_R_CRYPTO_SIGN_FUNCTION_CONFIGURED	104

#define EVP_R_CRYPTO_VERIFY_FUNCTION_CONFIGURED	105

#define EVP_R_PUBLIC_KEY_NOT_RSA			 	106

#define EVP_R_UNSUPPORTED_CIPHER			 	107

#define EVP_R_WRONG_FINAL_BLOCK_LENGTH			108

#define EVP_R_WRONG_PUBLIC_KEY_TYPE				109

 

#ifdef  __cplusplus

}

#endif

#endif

