/*
 * Nuvoton_M2351_aes_tinycrypt.c
 *
 *  Created on: 18 févr. 2019
 *      Author: Damien SOURSAS
 */
#include "include/aes.h"
#include "include/utils.h"
#include "include/constants.h"


int tc_aes_encrypt(uint8_t *out, const uint8_t *in)
{
	uint8_t state[Nk*Nb];
	unsigned int i;

	if (out == (uint8_t *) 0) {
		return TC_CRYPTO_FAIL;
	} else if (in == (const uint8_t *) 0) {
		return TC_CRYPTO_FAIL;
	}

	//int c = Encrypt_data(plainData, cipheredData);
  //if (c == NULL) print2Secure("Error : 128bits only", NULL);


	return TC_CRYPTO_SUCCESS;
}

int tc_aes_decrypt(uint8_t *out, const uint8_t *in)
{
	uint8_t state[Nk*Nb];
	unsigned int i;

	if (out == (uint8_t *) 0) {
		return TC_CRYPTO_FAIL;
	} else if (in == (const uint8_t *) 0) {
		return TC_CRYPTO_FAIL;
	}


	return TC_CRYPTO_SUCCESS;
}
