/*########################################################
 * @file       : main_ns.c
 * @version    : v1.00
 * @created on : 5 fevrier 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Non-secure main code for TrustZone OSCORE
/*########################################################*/


#include <arm_cmse.h>
#include "NuMicro.h"
#include "xom2_func.h"
#include "xom3_func.h"

typedef int32_t (*funcptr)(uint32_t);

void DEBUG_PORT_Init(void);

/*----------------------------------------------------------------------------
  NonSecure Callable Functions from Secure Region
 *----------------------------------------------------------------------------*/
extern int32_t Encrypt_data(uint8_t *plainData, uint8_t *cipheredData);
extern int32_t Decrypt_data(uint8_t *cipheredData, uint8_t *resultData);
extern int32_t Store_key(uint8_t *newKey);
extern int32_t Store_iv(uint8_t *newIv);
extern void print_Block(uint8_t *block);
extern void print2Secure(char *string, uint8_t *ptr);
extern int32_t Secure_LED_On(void);
extern int32_t Secure_LED_Off(void);
extern int32_t Secure_SW2_Status(void);
extern int32_t Secure_SW3_Status(void);
extern char * WIFI_PORT_Receive_Data(int);
extern int WIFI_PORT_Send_Data(int, char *, int, char *, int);

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
    }

    XOM3_Func(3);
    XOM2_Func(2);

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
	do {
        
		__WFI();
        
        if(Secure_SW2_Status() == 0)
        {
            while(Secure_SW2_Status() == 0); //Attente du front descendant du bouton
            char charToSend[30] = "Response from Nuvoton Board !\n";
            char *receive;
            
            while(1) {
                
                receive = WIFI_PORT_Receive_Data(0);
                printf("\nData Received : %s\n", receive);
                free(receive);
                WIFI_PORT_Send_Data(1, charToSend, 30, "30", 2);
                break;
                
            }
            
        }
        
        
	}
    while(1);

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
