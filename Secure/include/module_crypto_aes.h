/*
 * module_crypto_aes.h
 *
 *  Created on: 5 févr. 2019
 *      Author: dso
 */

#ifndef MODULE_CRYPTO_AES_H_
#define MODULE_CRYPTO_AES_H_

static volatile int32_t g_AES_done;

void CRPT_IRQHandler(void);
void OSCORE_crypto_init(void);
void OSCORE_SetKey(uint8_t *);
void AES_ONE_BLOCK_encrypt_data(uint8_t *, uint8_t *);
void AES_ONE_BLOCK_decrypt_data(uint8_t *, uint8_t *);
void print_Block(uint8_t *);

#endif /* MODULE_CRYPTO_AES_H_ */
