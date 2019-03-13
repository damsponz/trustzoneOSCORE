/* aes.h - TinyCrypt interface to Nuvoton M2351 AES ECB */

/**
 * @file
 * @brief -- Interface to an AES-128 implementation.
 *
 *  Overview:   AES-128 is a NIST approved block cipher specified in
 *              FIPS 197. Block ciphers are deterministic algorithms that
 *              perform a transformation specified by a symmetric key in fixed-
 *              length data sets, also called blocks.
 *
 *  Security:   AES-128 provides approximately 128 bits of security.
 *
 *  Usage:      1) call tc_aes128_set_encrypt/decrypt_key to set the key.
 *
 *              2) call tc_aes_encrypt/decrypt to process the data.
 */

#ifndef __TC_AES_H__
#define __TC_AES_H__

#include <stdint.h>
#include "Oscore.h"

#define Nb (4)  /* number of columns (32-bit words) comprising the state */
#define Nk (4)  /* number of 32-bit words comprising the key */
#define Nr (10) /* number of rounds */
#define TC_AES_BLOCK_SIZE (Nb*Nk)
#define TC_AES_KEY_SIZE (Nb*Nk)

/**
 *  @brief AES-128 Encryption procedure
 *  Encrypts contents of in buffer into out buffer under key;
 *              schedule s
 *  @note Assumes s was initialized by aes_set_encrypt_key;
 *              out and in point to 16 byte buffers
 *  @return  returns TC_CRYPTO_SUCCESS (1)
 *           returns TC_CRYPTO_FAIL (0) if: out == NULL or in == NULL or s == NULL
 *  @param out IN/OUT -- buffer to receive ciphertext block
 *  @param in IN -- a plaintext block to encrypt
 */
int tc_aes_encrypt(uint8_t *out, const uint8_t *in);

/**
 *  @brief AES-128 Encryption procedure
 *  Decrypts in buffer into out buffer under key schedule s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if: out is NULL or in is NULL or s is NULL
 *  @note   Assumes s was initialized by aes_set_encrypt_key
 *          out and in point to 16 byte buffers
 *  @param out IN/OUT -- buffer to receive ciphertext block
 *  @param in IN -- a plaintext block to encrypt
 */
int tc_aes_decrypt(uint8_t *out, const uint8_t *in);

#endif /* __TC_AES_H__ */
