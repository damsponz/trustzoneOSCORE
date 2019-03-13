/*
 *########################################################
 * @file       : main_ns.c
 * @version    : v1.00
 * @created on : 5 fevrier 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Non-secure main code for TrustZone OSCORE
 *########################################################
*/


#include <arm_cmse.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "xom2_func.h"
#include "xom3_func.h"

typedef int32_t (*funcptr)(uint32_t);

void DEBUG_PORT_Init(void);
void testCryptDeCrypt(void);

struct NETWORKDATA {
    char *data;
    int length;
};
typedef struct NETWORKDATA networkData;

/*----------------------------------------------------------------------------
  NonSecure Callable Functions from Secure Region
 *----------------------------------------------------------------------------*/
extern int32_t Encrypt_data(uint8_t *plainData, uint8_t *cipheredData);
extern int32_t Decrypt_data(uint8_t *cipheredData, uint8_t *resultData);
extern int32_t Store_key(uint8_t *newKey);
extern int32_t Store_iv(uint8_t *newIv);
extern void print_Block(uint8_t *block);
extern void print2Secure(char *string, void *ptr);
extern void printNetworkData(networkData *netData);
extern int32_t Secure_LED_On(void);
extern int32_t Secure_LED_Off(void);
extern int32_t Secure_SW2_Status(void);
extern int32_t Secure_SW3_Status(void);
extern int WIFI_PORT_Receive_Data(int print, networkData *netData);
extern int WIFI_PORT_Send_Data(int print, networkData *netData);

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    DEBUG_PORT_Init();

    if (DEMO) {
        print2Secure("+---------------------------------------------+",NULL);
        print2Secure("|           Nonsecure is running ...          |",NULL);
        print2Secure("+---------------------------------------------+",NULL);
        print2Secure("|                                             |",NULL);
        print2Secure("|         Wait & listen on TCP 5386 ...       |",NULL);
        print2Secure("|                                             |",NULL);
    }

    XOM3_Func(3);
    XOM2_Func(2);

    //if (DEMO) testCryptDeCrypt();

    networkData *rec = malloc(sizeof(networkData));
    rec->length = 0;
    WIFI_PORT_Receive_Data(0,rec);
    print2Secure("\nReceived\n", NULL);
    printNetworkData(rec); 

    if(rec->length == 32) {

        __attribute__((aligned(4))) uint8_t key[16] =
        {
            0x7f, 0x35, 0x91, 0xd3, 0x6f, 0xd5, 0x17, 0xa3, 0x7b, 0x6d, 0xe9, 0xe0, 0xdf, 0x93, 0x4b, 0x7a
        };
        __attribute__((aligned(4))) uint8_t iv[16] = {0};

        Store_key(key);
        Store_iv(iv);

        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);

        rec->length = 0;
        WIFI_PORT_Receive_Data(0,rec);
        print2Secure("\nReceived\n", NULL);
        printNetworkData(rec); 

        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);

        __attribute__((aligned(4))) uint8_t cipheredData[16] = 
        {
            0x30, 0x29, 0x1e, 0x38, 0x82, 0x6c, 0x5e, 0x8f, 0x9e, 0xfb, 0xca, 0xfc, 0xad, 0x3d, 0xc2, 0x8a
        };
        __attribute__((aligned(4))) uint8_t resultData[16] = {0};

        int r = Decrypt_data(cipheredData, resultData);
        if (r == NULL) print2Secure("Error : 128bits only", NULL);

        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);
        
        print2Secure("&resultData  = %p\n",resultData);
        print_Block(resultData);

        //Get response to client
        //<--


        //-->
        __attribute__((aligned(4))) uint8_t responsePlainData[16] = 
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d, 0x0F
        };
        __attribute__((aligned(4))) uint8_t responseCipherData[16] = {0};

        int c = Encrypt_data(responsePlainData, responseCipherData);
        if (c == NULL) print2Secure("Error : 128bits only", NULL);

        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);

        print2Secure("&responseData  = %p\n",responseCipherData);
        print_Block(responseCipherData);

        networkData *sent = malloc(sizeof(networkData));
        sent->length = 30;
        char *sen = malloc(sizeof(char)*30);
        char charToSend[30] = "Response from Nuvoton Board !\n";
        for (int nb=0; nb <= sent->length; nb++) sen[nb] = charToSend[nb];
        sent->data = sen;
        WIFI_PORT_Send_Data(1, sent);
        free(sent);

    }


	while(1){

        if(Secure_SW2_Status() == 0)
        {

            while(Secure_SW2_Status() == 0); //Attente du front descendant du bouton

            //char command_CWLIF[] = "AT+CWLIF\r\n";
            //WIFI_PORT_Write(0, command_CWLIF, (sizeof(command_CWLIF) / sizeof(char))-1);
            //WIFI_PORT_Red(1);

            networkData *sent = malloc(sizeof(networkData));
            sent->length = 30;
            char *sen = malloc(sizeof(char)*30);
            char charToSend[30] = "Response from Nuvoton Board !\n";
            for (int nb=0; nb <= sent->length; nb++) sen[nb] = charToSend[nb];
            sent->data = sen;
            WIFI_PORT_Send_Data(1, sent);
            free(sent);

        }
        
	}

    free(rec);
    

}

void testCryptDeCrypt() {

    //Test Function to cipher and decypher data 

    if (DEMO) Secure_LED_On();

    __attribute__((aligned(4))) uint8_t plainData[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    __attribute__((aligned(4))) uint8_t cipheredData[16] = {0};
    __attribute__((aligned(4))) uint8_t resultData[16] = {0};

    __attribute__((aligned(4))) uint8_t key[16] =
    {
        0x7f, 0x35, 0x91, 0xd3, 0x6f, 0xd5, 0x17, 0xa3, 0x7b, 0x6d, 0xe9, 0xe0, 0xdf, 0x93, 0x4b, 0x7a
    };
    __attribute__((aligned(4))) uint8_t iv[16] = {0};

    if (DEMO) {
        //print2Secure("&key  = %p\n",key);
        //print_Block(key);
        //print2Secure("&iv  = %p\n",iv);
        //print_Block(iv);
        print2Secure("&plainData  = %p\n",plainData);
        print_Block(plainData);
        CLK_SysTickLongDelay(500000);
    }

    Store_key(key);
    Store_iv(iv);

    int c = Encrypt_data(plainData, cipheredData);
    if (c == NULL) print2Secure("Error : 128bits only", NULL);

    if (DEMO) {
        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);

        print2Secure("&cipheredData  = %p\n",cipheredData);
        print_Block(cipheredData);
    }

    int r = Decrypt_data(cipheredData, resultData);
    if (r == NULL) print2Secure("Error : 128bits only", NULL);

    if (DEMO) {
        CLK_SysTickLongDelay(500000);
        print2Secure("|           Nonsecure is running ...          |\n",NULL);
        
        print2Secure("&resultData  = %p\n",resultData);
        print_Block(resultData);

        uint8_t error = 0;
        for (uint8_t i = 0 ; i < 16 ; i++) {

            if (resultData[i] != plainData[i]) error++;

        }
        if (error != 0) print2Secure("Error plainData is not equal to resultData", NULL);
        else print2Secure("No error plainData is equal to resultData", NULL);
            
        Secure_LED_Off();

    }

}


void DEBUG_PORT_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/

    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
    DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
