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

void OSCORE_crypto_init() {

	uint32_t au32AESKey[8] =
	{
	    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
	    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f
	};

	printf("&au32AESKey  = %p\n",au32AESKey);

	uint32_t au32AESIV[4] =
	{
	    0x00000000, 0x00000000, 0x00000000, 0x00000000
	};

    NVIC_EnableIRQ(CRPT_IRQn);
    AES_ENABLE_INT(CRPT);

	AES_Open(CRPT, 0, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
	AES_SetKey(CRPT, 0, au32AESKey, AES_KEY_SIZE_128);
	AES_SetInitVect(CRPT, 0, au32AESIV);

}

void OSCORE_SetKey(uint8_t InputKey[AES_KEY_SIZE_128/8]) {

	printf("nothing");

}

void AES_ONE_BLOCK_encrypt_data(uint8_t InputData[], uint8_t OutputData[]) {

	printf("encrypt function.\n");

	printf("&inputData  = %p\n",InputData);
	printf("&outputData  = %p\n",OutputData);

	OSCORE_crypto_init();
	printf("init ok.\n");

	/*---------------------------------------
	 *  AES-128 ECB mode encrypt
	 *---------------------------------------*/

	AES_SetDMATransfer(CRPT, 0, (uint32_t)InputData, (uint32_t)OutputData, (uint32_t)(16));
	printf("dma ok.\n");
	g_AES_done = 0;
	/* Start AES Eecrypt */
	//printf("Input data\n");
	//print_Block(InputData);
	//printf("Output data\n");
	//print_Block(OutputData);

	printf("&inputData  = %p\n",InputData);
	printf("&outputData  = %p\n",OutputData);

	AES_Start(CRPT, 0, CRYPTO_DMA_ONE_SHOT);
	printf("start AES.\n");
	CLK_SysTickLongDelay(1000000);
	print_Block(OutputData);
	/* Waiting for AES calculation */
	//while(!g_AES_done);

}

void AES_ONE_BLOCK_decrypt_data(uint8_t InputData[AES_KEY_SIZE_128/8], uint8_t OutputData[AES_KEY_SIZE_128/8]) {

	OSCORE_crypto_init();

    /*---------------------------------------
     *  AES-128 ECB mode decrypt
     *---------------------------------------*/

    AES_SetDMATransfer(CRPT, 0, (uint32_t)OutputData, (uint32_t)InputData, (uint32_t)(AES_KEY_SIZE_128/8));

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
