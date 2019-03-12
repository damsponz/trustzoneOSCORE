/*########################################################
 * @file       : Nuvoton_M2351_crypto_aes.h
 * @version    : v1.00
 * @created on : 5 fevrier 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Header Crypto accelerator
 /*########################################################*/

#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#ifndef NUVOTON_M2351_CRYPTO_AES_H_
#define NUVOTON_M2351_CRYPTO_AES_H_

#define ENCRYPT 1
#define DECRYPT 0

static volatile int32_t g_AES_done;

void CRPT_IRQHandler(void);
void Nuvoton_M2351_crypto_init(uint8_t, uint8_t);
void Nuvoton_M2351_crypto_useSessionKey(uint8_t );
void Nuvoton_M2351_crypto_useMasterKey(void);
void Nuvoton_M2351_encrypt_data(uint8_t, uint8_t *, uint8_t *);
void Nuvoton_M2351_decrypt_data(uint8_t, uint8_t *, uint8_t *);

#endif /* NUVOTON_M2351_CRYPTO_AES_H_ */
