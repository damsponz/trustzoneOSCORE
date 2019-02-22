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
		CLK_SysTickLongDelay(200000);
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

    if (DEMO) printf("|     Secure is running ... Encrypt_data      |\n");
    if (DEMO) LED_On();

    Nuvoton_M2351_crypto_init(0, ENCRYPT);
    Nuvoton_M2351_crypto_useSessionKey(0);
    Nuvoton_M2351_encrypt_data(0, plainData, cipheredData);

    if (DEMO) LED_Off();
    return (int32_t)cipheredData;
}

__NONSECURE_ENTRY
int32_t Decrypt_data(uint8_t *cipheredData, uint8_t *resultData) {

    if (DEMO) printf("|     Secure is running ... Decrypt_data      |\n");
    if (DEMO) LED_On();	

    Nuvoton_M2351_crypto_init(0, DECRYPT);
    Nuvoton_M2351_crypto_useSessionKey(0);
    Nuvoton_M2351_decrypt_data(0, cipheredData, resultData);

    if (DEMO) LED_Off();	
    return (int32_t)resultData;
}

__NONSECURE_ENTRY
int32_t Store_key(uint8_t *newKey) {

    if (DEMO) printf("|      Secure is running ... Store_key        |\n");
    if (DEMO) LED_On(); 

    for (uint8_t z = 0; z < 16; z++) {

        cipheredSessionKey[z] = newKey[z];

        /* Reset memory */
        newKey[z] = 0;

    }

    //printf("&cipheredSessionKey = %p\n", cipheredSessionKey);
    //print_Block(cipheredSessionKey);

    if (DEMO) LED_Off();    
    return OSCORE_CRYPTO_SUCCESS;
}

__NONSECURE_ENTRY
int32_t Store_iv(uint8_t *newIv) {

    if (DEMO) printf("|      Secure is running ... Store_iv         |\n");
    if (DEMO) LED_On(); 
    
    for (uint8_t z = 0; z < 16; z++) {

        sessionIv[z] = newIv[z];

        /* Reset memory */
        newIv[z] = 0;

    }

    if (DEMO) LED_Off();    
    return OSCORE_CRYPTO_SUCCESS;
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
int32_t print2Secure(char *string, uint8_t *ptr) {

    if (ptr == NULL) printf("%s\n",string);
		else printf(string,ptr);

	return 1;

}