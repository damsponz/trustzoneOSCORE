/*
 * module_crypto_aes.c
 *
 *  Created on: 5 févr. 2019
 *      Author: Damien SOURSAS
 */

#include "Nuvoton_M2351_crypto_aes.h"
#include "Oscore_crypto.h"

uint8_t cipheredSessionKey[16];
uint8_t sessionIv[16];

void CRPT_IRQHandler()
{
    if(AES_GET_INT_FLAG(CRPT))
    {
        g_AES_done = 1;
        AES_CLR_INT_FLAG(CRPT);
    }
}

void Nuvoton_M2351_crypto_init(uint8_t channel, uint8_t modeAES) {

	if(modeAES != ENCRYPT && modeAES != DECRYPT) printf("Error mode , mode must be ENCRYPT or DECRYPT value\n");

    NVIC_EnableIRQ(CRPT_IRQn);
    AES_ENABLE_INT(CRPT);

	AES_Open(CRPT, channel, modeAES, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);

}

void Nuvoton_M2351_crypto_useMasterKey() {

    CRPT_T *crpt = CRPT;
    uint32_t u32Channel = 1;

    /* Load Key */
    uint32_t  key_reg_addr;
    key_reg_addr = (uint32_t)&crpt->AES0_KEY[0] + (u32Channel * 0x3CUL);
	
	    /* Enable FMC ISP function */
    FMC_Open();
		
	outpw(key_reg_addr, FMC_ReadUID(0));
	outpw(key_reg_addr+4UL, FMC_ReadUID(1));
	outpw(key_reg_addr+8UL, FMC_ReadUID(2));
	outpw(key_reg_addr+12UL, FMC_ReadUID(0));
	
	//printf("\nmasterKey = %08x%08x%08x%08x\n",FMC_ReadUID(0),FMC_ReadUID(1),FMC_ReadUID(2),FMC_ReadUID(0));

    /* Disable FMC ISP function */
    FMC_Close();

	//outpw(key_reg_addr, 0x2b7e1516);
	//outpw(key_reg_addr+4UL, 0x28aed2a6);
	//outpw(key_reg_addr+8UL, 0xabf71588);
	//outpw(key_reg_addr+12UL, 0x09cf4f3c);

    /* Load IV */
    uint32_t  iv_reg_addr;
    iv_reg_addr = (uint32_t)&crpt->AES0_IV[0] + (u32Channel * 0x3CUL);

	outpw(iv_reg_addr, 0x00000000);
	outpw(iv_reg_addr+4UL, 0x00000000);
	outpw(iv_reg_addr+8UL, 0x00000000);
	outpw(iv_reg_addr+12UL, 0x00000000);

}

void Nuvoton_M2351_crypto_useSessionKey(uint8_t channel) {

	uint8_t sessionKey[16] = {0};

	//printf("&cipheredSessionKey = %p\n", cipheredSessionKey);
    //print_Block(cipheredSessionKey);

	Nuvoton_M2351_crypto_init(1, DECRYPT);
    Nuvoton_M2351_crypto_useMasterKey();
	if (DEMO) printf("|  Secure is running ... decrypt sessionKey   |\n");
    Nuvoton_M2351_decrypt_data(1, cipheredSessionKey, sessionKey);

	//printf("&sessionKey = %p\n", sessionKey);
    //print_Block(sessionKey);

    uint32_t tmp_sk[4];
    uint32_t tmp_si[4];

	for (uint8_t z = 0; z < 4; z++) {
    	tmp_sk[z] = (sessionKey[0+(4*z)] << 24) | (sessionKey[1+(4*z)] << 16) | (sessionKey[2+(4*z)] << 8) | sessionKey[3+(4*z)];
    	tmp_si[z] = (sessionIv[0+(4*z)] << 24) | (sessionIv[1+(4*z)] << 16) | (sessionIv[2+(4*z)] << 8) | sessionIv[3+(4*z)];
    }
	AES_SetKey(CRPT, channel, tmp_sk, AES_KEY_SIZE_128);
	AES_SetInitVect(CRPT, channel, tmp_si);

	/* Reset memory */
    for (uint8_t z = 0; z < 16; z++) sessionKey[z] = 0;

    for (uint8_t z = 0; z < 4; z++) {
    	tmp_sk[z] = 0;
    	tmp_si[z] = 0;
    }

}

void Nuvoton_M2351_encrypt_data(uint8_t channel, uint8_t InputData[], uint8_t OutputData[]) {

	//printf("AES ECB encrypt start.\n");

	//printf("&inputData  = %p\n",InputData);
	//printf("&outputData  = %p\n",OutputData);

	AES_SetDMATransfer(CRPT, channel, (uint32_t)InputData, (uint32_t)OutputData, 16);
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
	AES_Start(CRPT, channel, CRYPTO_DMA_ONE_SHOT);

	/* Waiting for AES calculation */
	while(!g_AES_done);

	//printf("AES ECB encrypt done.\n");

}

void Nuvoton_M2351_decrypt_data(uint8_t channel, uint8_t InputData[], uint8_t OutputData[]) {

	//printf("AES ECB decrypt start.\n");
	/*
	printf("&inputData  = %p\n",InputData);
	printf("&outputData  = %p\n",OutputData);
	*/

    AES_SetDMATransfer(CRPT, channel, (uint32_t)InputData, (uint32_t)OutputData, 16);
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
    AES_Start(CRPT, channel, CRYPTO_DMA_ONE_SHOT);
    /* Waiting for AES calculation */
    while(!g_AES_done);
		
	//printf("AES ECB decrypt done.\n");

}
