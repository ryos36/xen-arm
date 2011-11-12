#ifndef _TLS_CR_SECURITY_H
#define _TLS_CR_SECURITY_H

/*
#include "TlsCrConfig.h"
*/

#ifndef CrUINT8_SIZE
#define CrUINT8_SIZE		sizeof(CrUINT8)
#endif
/**
 * ###########################################################
 * 						MAC ALGORITH
 * ###########################################################
 */
#define TLS_MAC_ALGORITHM_SIZE		CrUINT8_SIZE
typedef	CrUINT8					 	TLSMACAlgorithm;
typedef enum
{
	TLS_MAC_SHA_0 = 0,
	TLS_MAC_SHA_32, /* added by Sung-Min */
	TLS_MAC_SHA_40,
	TLS_MAC_SHA_80,
	TLS_MAC_SHA,
	TLS_MAC_SHA_XOR_40,
	TLS_MAC_SHA_256,	// 10 July 2002 added by Jaejin Choi
	TLS_MAC_SHA_384,	// 10 July 2002 added by Jaejin Choi
	TLS_MAC_SHA_512,	// 10 July 2002 added by Jaejin Choi
	TLS_MAC_MD5_40,
	TLS_MAC_MD5_80,
	TLS_MAC_MD5,
	TLS_MAC_MD2 // 2001.07.10 Added by Taesung Kim.
}TLSMACAlgorithmEnum;

/**
 * ###########################################################
 * 						BULK CIPHER ALGORITH
 * ###########################################################
 */
#define TLS_BULK_CIPHER_ALGORITHM_SIZE	CrUINT8_SIZE
typedef	CrUINT8							TLSBulkCipherAlgorithm;
typedef enum
{
	TLS_BCIPHER_NULL = 0,
	TLS_BCIPHER_RC5_CBC_40,
	TLS_BCIPHER_RC5_CBC_56,
	TLS_BCIPHER_RC5_CBC,
	TLS_BCIPHER_DES_CBC_40,
	TLS_BCIPHER_DES_CBC,
	TLS_BCIPHER_3DES_CBC_EDE,
	TLS_BCIPHER_IDEA_CBC_40,
	TLS_BCIPHER_IDEA_CBC_56,
	TLS_BCIPHER_IDEA_CBC,
	TLS_BCIPHER_RC4_40, // added RC4 2001.11.19
	TLS_BCIPHER_RC4,	// added RC4 2001.11.19
	TLS_BCIPHER_AES_ECB_128,
	TLS_BCIPHER_AES_ECB_192,
	TLS_BCIPHER_AES_ECB_256,
	TLS_BCIPHER_AES_CBC_128,
	TLS_BCIPHER_AES_CBC_192,
	TLS_BCIPHER_AES_CBC_256,
	TLS_BCIPHER_AES_CFB1_128,
	TLS_BCIPHER_AES_CFB1_192,
	TLS_BCIPHER_AES_CFB1_256,		// added RC4 2001.11.19
	TLS_BCIPHER_SEED_ECB,
	TLS_BCIPHER_SEED_CBC,
	TLS_BCIPHER_SEED_CFB,
	TLS_BCIPHER_SEED_OFB,
	TLS_BCIPHER_RC2_CBC,
	TLS_BCIPHER_RC2_CBC_40
}TLSBulkCipherEnum;

#endif /* #ifndef _TLS_CR_SECURITY_H */
