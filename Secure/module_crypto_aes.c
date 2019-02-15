/*
 * module_crypto_aes.c
 *
 *  Created on: 5 févr. 2019
 *      Author: Damien SOURSAS
 */

#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "module_crypto_aes.h"

void CRPT_IRQHandler()
{
    if(AES_GET_INT_FLAG(CRPT))
    {
        g_AES_done = 1;
        AES_CLR_INT_FLAG(CRPT);
    }
}

void OSCORE_crypto_init(uint8_t modeAES) {

	if(modeAES != ENCRYPT && modeAES != DECRYPT) printf("Error mode , mode must be ENCRYPT or DECRYPT value\n");

    NVIC_EnableIRQ(CRPT_IRQn);
    AES_ENABLE_INT(CRPT);

	AES_Open(CRPT, 0, modeAES, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);

}

void OSCORE_crypto_SetKey(uint32_t InputKey[],uint32_t InputIv[]) {

	AES_SetKey(CRPT, 0, InputKey, AES_KEY_SIZE_128);
	AES_SetInitVect(CRPT, 0, InputIv);

}

void AES_ONE_BLOCK_encrypt_data(uint8_t InputData[], uint8_t OutputData[]) {

	printf("encrypt function.\n");

	//printf("&inputData  = %p\n",InputData);
	//printf("&outputData  = %p\n",OutputData);

	/*---------------------------------------
	 *  AES-128 ECB mode encrypt
	 *---------------------------------------*/

	AES_SetDMATransfer(CRPT, 0, (uint32_t)InputData, (uint32_t)OutputData, 16);
/*
	printf("before start AES : \n"
		    		"CRPT->AES0_CNT   : %d\n"
		    		"CRPT->AES0_DADDR : %d\n"
		    		"CRPT->AES0_IV    : %d\n"
		    		"CRPT->AES0_KEY   : %d\n"
		    		"CRPT->AES0_SADDR : %d\n", CRPT->AES0_CNT,CRPT->AES0_DADDR,CRPT->AES0_IV, CRPT->AES0_KEY, CRPT->AES0_SADDR);
*/

	g_AES_done = 0;
	/* Start AES encrypt */
	AES_Start(CRPT, 0, CRYPTO_DMA_ONE_SHOT);

	/* Waiting for AES calculation */
	while(!g_AES_done);

}

void AES_ONE_BLOCK_decrypt_data(uint8_t InputData[], uint8_t OutputData[]) {

	printf("decrypt function.\n");

	/*
	printf("&inputData  = %p\n",InputData);
	print_Block(InputData);
	printf("&outputData  = %p\n",OutputData);
	print_Block(OutputData);
	*/

    /*---------------------------------------
     *  AES-128 ECB mode decrypt
     *---------------------------------------*/

    AES_SetDMATransfer(CRPT, 0, (uint32_t)InputData, (uint32_t)OutputData, 16);
/*
	printf("before start AES : \n"
		    		"CRPT->AES0_CNT   : %d\n"
		    		"CRPT->AES0_DADDR : %d\n"
		    		"CRPT->AES0_IV    : %d\n"
		    		"CRPT->AES0_KEY   : %d\n"
		    		"CRPT->AES0_SADDR : %d\n", CRPT->AES0_CNT,CRPT->AES0_DADDR,CRPT->AES0_IV, CRPT->AES0_KEY, CRPT->AES0_SADDR);
*/
    g_AES_done = 0;
    /* Start AES decrypt */
    AES_Start(CRPT, 0, CRYPTO_DMA_ONE_SHOT);
    /* Waiting for AES calculation */
    while(!g_AES_done);

}
void print_Block(uint8_t *block) {

    printf("&block  = %p\n",block);

    for (uint8_t i = 0; i < 4; i++) {
        printf("|");
        for (uint8_t j = 0; j < 16; j = j+4) {

            printf(" %02x",block[i+j]);

        }
        printf(" |\n");
    }
    printf("\n");

}
