#ifndef TLS_RSA_KEY_C
#define TLS_RSA_KEY_C

#include "crypto/TlsCrKeySt.h"

#define HASH_SIZE 20

void serialize_pr_key(const TLSRSAPrivateKey* key, unsigned char** buf, int* len);
void serialize_pu_key(const TLSRSAPublicKey* key, unsigned char** buf, int* len);
void deserialize_pr_key(TLSRSAPrivateKey* key, const unsigned char* src);
void deserialize_pu_key(TLSRSAPublicKey* key, const unsigned char* src);
int generate_rsa_keys(TLSRSAPrivateKey** pr_key, TLSRSAPublicKey** pu_key);

#endif
