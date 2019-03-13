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
void stringKeyToKey(char *, uint8_t *);
void keyToStringKey(uint8_t *, char *);

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
extern void printBlock(uint8_t *block);
extern void printSecure(char *string, void *ptr, uint8_t val);
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
        printSecure("+---------------------------------------------+",NULL,NULL);
        printSecure("#           Nonsecure is running ...          #",NULL,NULL);
        printSecure("+---------------------------------------------+",NULL,NULL);
    }

    XOM3_Func(3);
    XOM2_Func(2);

    //if (DEMO) testCryptDeCrypt();

	while(1){

        if(Secure_SW2_Status() == 0)
        {

            while(Secure_SW2_Status() == 0); //Attente du front descendant du bouton
            demoPres();

        }

        if(Secure_SW3_Status() == 0)
        {

            while(Secure_SW3_Status() == 0); //Attente du front descendant du bouton

            //char command_CWLIF[] = "AT+CWLIF\r\n";
            //WIFI_PORT_Write(0, command_CWLIF, (sizeof(command_CWLIF) / sizeof(char))-1);
            //WIFI_PORT_Red(1);

            networkData *sent = malloc(sizeof(networkData));
            sent->data = malloc(sizeof(char)*30);
            sent->length = 30;
            
            char charToSend[30] = "Response from Nuvoton Board !\n";
            for (int nb=0; nb <= sent->length; nb++) (sent->data)[nb] = charToSend[nb];
            WIFI_PORT_Send_Data(1, sent);
            free(sent->data);
            free(sent);

        }
        
	}
    

}

demoPres(void) {

    networkData *rec = malloc(sizeof(networkData));
    rec->data = malloc(sizeof(char)*33);
    (rec->data)[32] = '\0';
    rec->length = 0;
    WIFI_PORT_Receive_Data(0,rec);
    printSecure("#         Wait & listen on TCP 5386 ...       #",NULL,NULL);
    printSecure("#            Ciphered Session Key ...         #",NULL,NULL);
    printSecure("#                                             #",NULL,NULL);
    printNetworkData(rec); 
    printSecure("#                                             #",NULL,NULL);

    if(rec->length == 32) {

        __attribute__((aligned(4))) uint8_t iv[16] = {0};
        __attribute__((aligned(4))) uint8_t key[16] = {0};
        stringKeyToKey(rec->data, key);
        //printBlock(key);

        Store_key(key);     //NSC Function
        Store_iv(iv);       //NSC Function

        CLK_SysTickLongDelay(500000);
        printSecure("#           Nonsecure is running ...          #",NULL,NULL);
        printSecure("#         Wait & listen on TCP 5386 ...       #",NULL,NULL);

        rec->length = 0;
        WIFI_PORT_Receive_Data(0,rec);     //NSC Function
        printSecure("#            Ciphered User Data ...           #",NULL,NULL);
        printSecure("#                                             #",NULL,NULL);
        printNetworkData(rec); 
        printSecure("#                                             #",NULL,NULL);

        CLK_SysTickLongDelay(500000);
        printSecure("#           Nonsecure is running ...          #",NULL,NULL);


        if(rec->length == 32) {

            __attribute__((aligned(4))) uint8_t cipheredData[16] = {0};
            __attribute__((aligned(4))) uint8_t resultData[16] = {0};
            stringKeyToKey(rec->data, cipheredData);

            int r = Decrypt_data(cipheredData, resultData);      //NSC Function
            if (r == NULL) printSecure("Error : 128bits only", NULL,NULL);

            CLK_SysTickLongDelay(500000);
            printSecure("#           Nonsecure is running ...          #",NULL,NULL);
            
            //printSecure("&resultData  = %p\n",resultData,NULL);
            //printBlock(resultData);
            char *messageR = malloc(sizeof(char)*17);
            messageR[16] = '\0';
            for (int nb=0; nb < 16; nb++) messageR[nb] = (char)(resultData[nb]);
            printSecure("#                                             #",NULL,NULL);
            printSecure("#        User Data  = %s        #\n",messageR,NULL);
            printSecure("#                                             #",NULL,NULL);

            //Get response to client
            //<--

            printSecure("#               Create Answer ...             #",NULL,NULL);

            //__attribute__((aligned(4))) uint8_t responsePlainData[16] = {"Que fait on ici?"};
            __attribute__((aligned(4))) uint8_t responsePlainData[16] = {"SoutenanceOSCORE"};
            __attribute__((aligned(4))) uint8_t responseCipherData[16] = {0};

            //-->

            int c = Encrypt_data(responsePlainData, responseCipherData);      //NSC Function
            if (c == NULL) printSecure("Error : 128bits only", NULL,NULL);

            CLK_SysTickLongDelay(500000);
            printSecure("#           Nonsecure is running ...          #",NULL,NULL);
            printSecure("#               Send Answer ...               #",NULL,NULL);
            //printSecure("&responseData  = %p\n",responseCipherData,NULL);
            //printBlock(responseCipherData);

            networkData *sent = malloc(sizeof(networkData));
            sent->data = malloc(sizeof(char)*33);
            sent->length = 33;

            char *messageS = malloc(sizeof(char)*33);
            messageS[32] = '\0';
            keyToStringKey(responseCipherData, messageS);
            (sent->data)[32] = '\n';
            keyToStringKey(responseCipherData, sent->data);
            WIFI_PORT_Send_Data(0, sent);
            printSecure("#                 Data Sent !                 #",NULL,NULL);
            printSecure("+---------------------------------------------+",NULL,NULL);
            free(sent->data);
            free(sent);

        }

    }

    free(rec->data);
    free(rec);

}

void stringKeyToKey(char *stringKey, uint8_t *key) {

    //printSecure("StringKeyToKey\n", NULL, NULL);

    uint8_t temp0;
    int i = 0;

    //printSecure("stringKey[0] = %x\n", NULL,(uint8_t)stringKey[0]);

    for (int nb=0; nb < 32; nb++) {
        if (stringKey[nb] >= 0x30 && stringKey[nb] <= 0x39) {
            if (!(nb%2)) temp0 = ((uint8_t)stringKey[nb]-0x30)<<4;
            else temp0 = temp0 | ((uint8_t)stringKey[nb]-0x30);
            //printSecure("temp0_A : %x\n", NULL,temp0);
        }
        else if (stringKey[nb] >= 0x61 && stringKey[nb] <= 0x66) {
            if (!(nb%2)) temp0 = (stringKey[nb]-0x57)<<4;
            else temp0 = temp0 | (stringKey[nb]-0x57);
            //printSecure("temp0_B : %x\n", NULL,temp0);
        }
        else {
            printSecure("StringKeyToKey ERROR : Bad Key.\n", NULL, NULL);
            break;
        }
        i = (int)(nb/2);
        key[i] = temp0;
    }

}

void keyToStringKey(uint8_t *key, char *stringKey) {

    //printSecure("keyToStringKey\n", NULL, NULL);
    //printSecure("key[0] = %x\n", NULL,(uint8_t)key[0]);
    uint8_t temp0;
    int i = 0;

    for (int nb=0; nb < 32; nb++) {

        i = (int)(nb/2);
        if (!(nb%2)) temp0 = (key[i]>>4) & 0x0f;
        else temp0 = 0x0f & key[i];

        if (temp0 >= 0 && temp0 <= 9) stringKey[nb] = (char)(temp0+0x30);
        else if (temp0 >= 0xa && temp0 <= 0xf) stringKey[nb] = (char)(temp0+0x57);

    }

    //printSecure("stringKey = %s\n", stringKey, NULL);

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
        //printSecure("&key  = %p\n",key);
        //printBlock(key);
        //printSecure("&iv  = %p\n",iv);
        //printBlock(iv);
        printSecure("&plainData  = %p\n",plainData, NULL);
        printBlock(plainData);
        CLK_SysTickLongDelay(500000);
    }

    Store_key(key);
    Store_iv(iv);

    int c = Encrypt_data(plainData, cipheredData);
    if (c == NULL) printSecure("Error : 128bits only", NULL, NULL);

    if (DEMO) {
        CLK_SysTickLongDelay(500000);
        printSecure("|           Nonsecure is running ...          |\n",NULL, NULL);

        printSecure("&cipheredData  = %p\n",cipheredData, NULL);
        printBlock(cipheredData);
    }

    int r = Decrypt_data(cipheredData, resultData);
    if (r == NULL) printSecure("Error : 128bits only", NULL, NULL);

    if (DEMO) {
        CLK_SysTickLongDelay(500000);
        printSecure("|           Nonsecure is running ...          |\n",NULL, NULL);
        
        printSecure("&resultData  = %p\n",resultData, NULL);
        printBlock(resultData);

        uint8_t error = 0;
        for (uint8_t i = 0 ; i < 16 ; i++) {

            if (resultData[i] != plainData[i]) error++;

        }
        if (error != 0) printSecure("Error plainData is not equal to resultData", NULL, NULL);
        else printSecure("No error plainData is equal to resultData", NULL, NULL);
            
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
