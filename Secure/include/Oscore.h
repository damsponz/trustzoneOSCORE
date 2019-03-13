/*
 *########################################################
 * @file       : Oscore.h
 * @version    : v1.00
 * @created on : 20 fevrier 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Header functions abstraction for OSCORE
 *########################################################
*/

#include <arm_cmse.h>
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "Nuvoton_M2351_crypto_aes.h"
#include "Nuvoton_M2351_wifi_module.h"

#define OSCORE_CRYPTO_SUCCESS 1
#define OSCORE_CRYPTO_FAIL 0

extern uint8_t cipheredSessionKey[16];
extern uint8_t sessionIv[16];

/* typedef for NonSecure callback functions */
typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
typedef int32_t (*Secure_funcptr)(uint32_t);

struct NETWORKDATA {
	char *data;
	int length;
};
typedef struct NETWORKDATA networkData;


/* Crypto */
__NONSECURE_ENTRY
int32_t Encrypt_data(uint8_t *,uint8_t *);
__NONSECURE_ENTRY
int32_t Decrypt_data(uint8_t *,uint8_t *);
__NONSECURE_ENTRY
int32_t Store_key(uint8_t *);
__NONSECURE_ENTRY
int32_t Store_iv(uint8_t *);

/* Print */
__NONSECURE_ENTRY
int32_t printBlock(uint8_t *);
__NONSECURE_ENTRY
int32_t printSecure(char *,void *,uint8_t);
__NONSECURE_ENTRY
int32_t printNetworkData(networkData *);


__NONSECURE_ENTRY
int32_t Secure_LED_On(void);
__NONSECURE_ENTRY
int32_t Secure_LED_Off(void);

/* Wifi */
__NONSECURE_ENTRY
int WIFI_PORT_Receive_Data(int, networkData *);
__NONSECURE_ENTRY
int WIFI_PORT_Send_Data(int, networkData *);
