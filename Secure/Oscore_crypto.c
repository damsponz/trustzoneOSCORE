/*
 * Oscore_crypto.c
 *
 *  Created on: 20 févr. 2019
 *      Author: Damien SOURSAS
 */
 
#include "Oscore_crypto.h"

/*----------------------------------------------------------------------------
  Secure LED control function
 *----------------------------------------------------------------------------*/
int32_t LED_On(void)
{
    //printf("Secure LED On\n");
    PA11 = 0;
    return 1;
}

int32_t LED_Off(void)
{
    //printf("Secure LED Off\n");
    PA11 = 1;
    return 1;
}

/*----------------------------------------------------------------------------
  Secure functions exported to NonSecure application
  Must place in Non-secure Callable
 *----------------------------------------------------------------------------*/
__NONSECURE_ENTRY
int32_t Secure_LED_On(void)
{
    printf("Secure LED On call by Non-secure\n");
    PA10 = 0;
    return 1;
}

__NONSECURE_ENTRY
int32_t Secure_LED_Off(void)
{
    printf("Secure LED Off call by Non-secure\n");
    PA10 = 1;
    return 1;
}

__NONSECURE_ENTRY
int32_t Encrypt_data(uint8_t *plainData, uint8_t *cipheredData) {

    printf("|           Secure is running ...          |\n");
    LED_On();
    printf("Encrypt_data NSC func\n");
    
    __attribute__((aligned(4))) uint32_t au32AESKey[4] =
    {
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c
    };
    __attribute__((aligned(4))) uint32_t au32AESIV[4] =
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    printf("&plainData  = %p",plainData);

    Nuvoton_M2351_crypto_init(ENCRYPT);
    Nuvoton_M2351_crypto_setKey(au32AESKey,au32AESIV);
    Nuvoton_M2351_encrypt_data(plainData, cipheredData);
		
	printf("&cipheredData  = %p",cipheredData);

    LED_Off();
    return (int32_t)cipheredData;
}
__NONSECURE_ENTRY
int32_t Decrypt_data(uint8_t *cipheredData, uint8_t *resultData) {

    printf("|           Secure is running ...          |\n");
    LED_On();	
    printf("Decrypt_data NSC func\n");
	
	    __attribute__((aligned(4))) uint32_t au32AESKey[4] =
    {
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c
    };
    __attribute__((aligned(4))) uint32_t au32AESIV[4] =
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    Nuvoton_M2351_crypto_init(DECRYPT);
    Nuvoton_M2351_crypto_setKey(au32AESKey,au32AESIV);
    Nuvoton_M2351_decrypt_data(cipheredData, resultData);

    printf("&resultData  = %p",resultData);

    LED_Off();	
    return (int32_t)resultData;
}

__NONSECURE_ENTRY
int32_t print_Block(uint8_t *block) {

    //printf("&block  = %p\n",block);

    for (uint8_t i = 0; i < 4; i++) {
        printf("|");
        for (uint8_t j = 0; j < 16; j = j+4) {

            printf(" %02x",block[i+j]);

        }
        printf(" |\n");
    }
    printf("\n");

		return 1;
}

__NONSECURE_ENTRY
int32_t print2Secure(char *string) {

    printf("%s",string);

		return 1;
}