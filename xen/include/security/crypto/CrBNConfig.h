/**

 * Big Number�� ���õ� ��� ������ ���⼭ �ϰ� �ȴ�.

 * ���� CrConfig.h���� �⺻ ������ �ϰ� �ǰ�, 

 * �� ���Ͽ��� specific�ϰ� �����ϰ� �ȴ�.

 * ���⼭�� RSAȤ�� ECC�� ���� �����ϵǾ�� �ϴ� �Լ��鿡

 * ���� ������ ������Ʈ�� ���� �����ϵǾ�� �ϴ� �ҽ��鿡

 * ���� ������ �ϰ� �ȴ�. 

 * �׸��� WTLS�� ���� ��쿡 CLASS�� ���� �����ϵǾ�� ��

 * �ҽ���� �Լ����� �޶����� �Ǵµ� �̸� ���⼭ �����ϱ�� �Ѵ�.

 *

 * ����� �޺κп� Big Number���� 

 * ����ϴ� �ҽ���� �Լ��鿡 ���� ����Ʈ�� ��Ÿ������.

 */

#ifndef HEADER_BN_CONFIG_H

#define HEADER_BN_CONFIG_H



#include <CrConfig.h>



#	define _BN_ADD_C

#	define _BN_BLIND_C

#	define _BN_DIV_C

#	define _BN_EXP_C

#	define _BN_GCD_C

#	define _BN_LIB_C

#	define _BN_MOD_C

#	define _BN_MODMUL_C

#	define _BN_MONT_C

#	define _BN_MPI_C

#	define _BN_MUL_C

#	define _BN_MULW_C

#	define _BN_PRIME_C

#	define _BN_RAND_C

#	define _BN_RECP_C

#	define _BN_SHIFT_C

#	define _BN_SQR_C

#	define _BN_SQRT_C

#	define _BN_SUB_C

#	define _BN_WORD_C

#	define _BN_PRINT_C



#	define NO_STDIO



#ifdef CR_RSA



#endif // #ifdef CR_RSA



#ifdef CR_ECC

//define...������ �Ǿ�� �� �ҽ���

#endif // #ifdef CR_ECC



#endif // #ifndef HEADER_BN_CONFIG_H





/*###############################################################

 *

 *				BN SOURCE(BN_XX.._C) & FUNCTIONS LISTS

 *

 *###############################################################

 */

#if 0

_BN_ADD_C

	BN_add

	bn_qadd

_BN_BLIND_C

	BN_BLINDING_new

	BN_BLINDING_free

	BN_BLINDING_update

	BN_BLINDING_convert

	BN_BLINDING_invert



_BN_DIV_C

	BN_div



_BN_EXP_C

	BN_exp

	BN_mod_exp

	BN_mod_exp_recp

	BN_mod_exp_mont

	BN_mod_exp_simple



_BN_GCD_C

	BN_gcd

	euclid

	BN_mod_inverse

	BN_mod_inverse_kyung



_BN_LIB_C

	BN_value_one

	BN_options

	BN_num_bits_word

	BN_num_bits

	BN_clear_free

	BN_free

	BN_new

	BN_CTX_new

	BN_CTX_free

	bn_expand2

	BN_dup

	BN_copy

	BN_clear

	BN_get_word

	BN_set_word

	BN_bin2bn

	BN_bn2bin

	BN_ucmp

	BN_cmp

	BN_set_bit

	BN_clear_bit

	BN_is_bit_set

	BN_mask_bits



_BN_MOD_C

	BN_mod



_BN_MOD_MUL_C

	BN_mod_mul



_BN_MONT_C

	BN_mod_mul_montgomery

	BN_from_montgomery

	BN_MONT_CTX_new

	BN_MONT_CTX_free

	BN_MONT_CTX_set



_BN_MPI_C

	BN_bn2mpi

	BN_mpi2bn



_BN_MUL_C

	BN_mul



_BN_MULW_C

	bn_mul_add_words

	bn_mul_words

	bn_sqr_words

	bn_add_words

	bn_div64



_BN_PRIME_C

	BN_generate_prime

	BN_is_prime

	witness

	probable_prime

	probable_prime_dh

	probable_prime_dh_strong



_BN_PRINT_C

	BN_bn2hex

	BN_bn2dec

	BN_hex2bn

	BN_dec2bn

	BN_print



_BN_RAND_C

	BN_rand



_BN_RECP_C

	BN_mod_mul_reciprocal

	BN_reciprocal



_BN_SHIFT_C

	BN_lshift1

	BN_rshift1

	BN_lshift

	BN_rshift



_BN_SQR_C

	BN_sqr



_BN_SQRT_C

	BN_mod_sqrt



_BN_SUB_C

	bn_qsub

	BN_sub



_BN_WORD_C

	BN_mod_word

	BN_div_word

	BN_add_word

	BN_sub_word



#endif // #if 0



