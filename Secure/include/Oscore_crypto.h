/*
 * Oscore_crypto.h
 *
 *  Created on: 20 févr. 2019
 *      Author: Damien SOURSAS
 */

#include <arm_cmse.h>
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "Nuvoton_M2351_crypto_aes.h"

/* typedef for NonSecure callback functions */
typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
typedef int32_t (*Secure_funcptr)(uint32_t);

__NONSECURE_ENTRY
int32_t Encrypt_data(uint8_t *,uint8_t *);
__NONSECURE_ENTRY
int32_t Decrypt_data(uint8_t *,uint8_t *);
__NONSECURE_ENTRY
int32_t print_Block(uint8_t *);
__NONSECURE_ENTRY
int32_t print2Secure(char *);
__NONSECURE_ENTRY
int32_t Secure_LED_On(void);
__NONSECURE_ENTRY
int32_t Secure_LED_Off(void);
