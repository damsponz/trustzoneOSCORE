/*########################################################
 * @file       : Oscore.c
 * @version    : v1.00
 * @created on : 20 fevrier 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Functions abstraction for OSCORE
 /*########################################################*/
 
#include "Oscore.h"

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
  LED Secure functions exported to NonSecure application
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


/*----------------------------------------------------------------------------
 CRYPTO Secure functions exported to NonSecure application
 Must place in Non-secure Callable
 *----------------------------------------------------------------------------*/
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
/*----------------------------------------------------------------------------
 PRINT Secure functions exported to NonSecure application
 Must place in Non-secure Callable
 *----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------
 WIFI Secure functions exported to NonSecure application
 Must place in Non-secure Callable
 *----------------------------------------------------------------------------*/

__NONSECURE_ENTRY
int WIFI_PORT_Send_Data(int print, char *charToSend, int nChar, char *nbChar, int nbDigit)
{
    //char *charToSend  : in ASCII without '\0'
    //int nChar         : nb of char in charToSend
    //char *nbChar      : char list with nb of char in charToSend
    //char *nbDigit     : nb digits in nbChar list
    
    LED_Y = 0;
    
    char cmd[13] = "AT+CIPSEND=0,";
    char cmdEnd[2] = "\r\n";
    
    WIFI_PORT_Write(0, cmd, (sizeof(cmd) / sizeof(char)));
    WIFI_PORT_Write(0, nbChar, nbDigit);
    WIFI_PORT_Write(0, cmdEnd, 2);
    WIFI_PORT_Read(0);
    //printf("%s\n", charToSend);
    //printf("%d",nChar);
    WIFI_PORT_Write(0, charToSend, nChar);
    WIFI_PORT_Write(0, cmdEnd, 2);
    WIFI_PORT_Read(0);
    if (print) printf("Data Sent !\n");
    
    LED_Y = 1;
    
    return 1;
    
}

__NONSECURE_ENTRY
char * WIFI_PORT_Receive_Data(int print)
{
    
    LED_G = 0;
    char buff = 0;
    int dataReceive = 0;
    int loop = 0;
    int indx = 0;
    char dataR[300] = {0};
    int i = 0;
    int c = 0;
    int nbCharLength = 0;
    int lengthData = 0;
    int columnDetect = 0;
    
    while (dataReceive == 0) {
        while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) {
            loop = 0;
            buff = WIFI_PORT->DAT;
            //printf("%c",buff);
            if(indx == 0 && buff == '+' && loop == 0) {
                indx = 1;
                loop = 1;
                //printf("%c",buff);
            }
            if(indx == 1 && buff == 'I' && loop == 0) {
                indx = 2;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 1 && loop == 0) indx = 0;
            
            if(indx == 2 && buff == 'P' && loop == 0) {
                indx = 3;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 2 && loop == 0) indx = 0;
            
            if(indx == 3 && buff == 'D' && loop == 0) {
                indx = 4;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 3 && loop == 0) indx = 0;
            
            if(indx == 4 && buff == ',' && loop == 0) {
                dataReceive = 1;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 4 && loop == 0) indx = 0;
        }
    }
    while(dataReceive) {
        while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) {
            buff = WIFI_PORT->DAT;
            //printf("%c",buff);
            if (c == 0) {
                
                uint8_t channelID = buff-0x30;
                if (print) printf("channel ID : %d\n", channelID);
                
            }
            if (c == 1 && buff != ',') dataReceive = 0; //ERROR
            //Data Read
            if (columnDetect && c < lengthData+3+nbCharLength) {
                
                dataR[c-(3+nbCharLength)] = buff;
                
            }
            //Length parse
            if (c > 1 && columnDetect == 0) {
                
                if (buff == ':') {
                    columnDetect = 1;
                    lengthData--;
                    if (print) printf("lengthData : %d\n", lengthData);
                }
                else {
                    lengthData = 10*lengthData+buff-0x30;
                    nbCharLength++;
                }
                
            }
            if (columnDetect && c == lengthData+3+nbCharLength) dataReceive = 0;
            c++;
        }
    }
    dataR[lengthData] = '\0';
    if (print) printf("data : %s\n", dataR);
    char *outData = malloc(sizeof(char)*(lengthData+1));
    for (int nb=0; nb <= lengthData; nb++) outData[nb] = dataR[nb];
    LED_G = 1;
    return outData;
}
