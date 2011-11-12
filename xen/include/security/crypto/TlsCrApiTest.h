/**

 *

 * File			: TlsCrApiTest.h

 * Desciption	: Header file of TLS Crypto API

 * Author		: Taesung Kim, Jaejin Choi

 * History		: 

 */



#ifndef _TLS_CR_API_TEST_H_

#define _TLS_CR_API_TEST_H_





#define TLS_CR_API_RSA_TEST

#define TLS_CR_API_SYMMETRIC_TEST

#define TLS_CR_API_HASH_TEST

//#define TLS_CR_API_RAND_TEST



#ifdef TLS_CR_API_RSA_TEST

TLSCrResult

TlsCrApiRSATest(void);

#endif 





#ifdef TLS_CR_API_ECDSA_TEST

TLSCrResult

TlsCrApiECDSATest(void);

#endif





#ifdef TLS_CR_API_SYMMETRIC_TEST

TLSCrResult

TlsCrApiSymCipherTest(void);

#endif



/* for SHA-1, MD5, MD2, SHA 256/384/512 */

#ifdef TLS_CR_API_HASH_TEST

TLSCrResult

TlsCrApiHashTest(void);

#endif



#ifdef TLS_CR_API_HMAC_TEST

TLSCrResult

TlsCrApiHmacTest(void);

#endif



#ifdef TLS_CR_API_RAND_TEST

TLSCrResult

TlsCrApiRandTest(void);

#endif



#endif



