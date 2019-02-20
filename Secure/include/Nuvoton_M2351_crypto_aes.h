/*
 * Nuvoton_M2351_crypto_aes.h
 *
 *  Created on: 5 févr. 2019
 *      Author: Damien SOURSAS
 */

#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#ifndef NUVOTON_M2351_CRYPTO_AES_H_
#define NUVOTON_M2351_CRYPTO_AES_H_

#define ENCRYPT 1
#define DECRYPT 0

static volatile int32_t g_AES_done;

void CRPT_IRQHandler(void);
void Nuvoton_M2351_crypto_init(uint8_t);
void Nuvoton_M2351_crypto_setKey(uint32_t *, uint32_t *);
void Nuvoton_M2351_encrypt_data(uint8_t *, uint8_t *);
void Nuvoton_M2351_decrypt_data(uint8_t *, uint8_t *);

#endif /* NUVOTON_M2351_CRYPTO_AES_H_ */
