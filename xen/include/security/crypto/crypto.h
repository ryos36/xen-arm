#ifndef CRYPTO_H
#define CRYPTO_H

#include <public/security/secure_storage_struct.h>

//#define SUPPORT_STREAM // use INIT / UPDATE / FINAL

int crypto_init(void);

int crypto_encrypt_data(unsigned char* src, int src_len, unsigned char** output, int* output_len);

int crypto_decrypt_data(unsigned char* src, int src_len, unsigned char** output, int* output_len);

int crypto_sign_data(unsigned char* src, int src_len, unsigned char** sig, int* sig_len);

int crypto_verify_data(unsigned char* src, int src_len, unsigned char* sig, int sig_len);

int crypto_verify_data_with_certm(unsigned char* src, int src_len, unsigned char* sig, int sig_len);

int crypto_hash_data(unsigned char* src, int src_len, unsigned char** hash, int* hash_len);

int init_master_key(void);

#define SYMMETRIC_ENCRYPTION 0
#define SYMMETRIC_DECRYPTION 1

#define noError 0

#endif
