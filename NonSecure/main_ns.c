/**************************************************************************//**
 * @file     main_ns.c
 * @version  V1.00
 * @brief    Non-secure main code for TrustZone OSCORE
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <arm_cmse.h>
#include "NuMicro.h"
#include "xom2_func.h"
#include "xom3_func.h"

typedef int32_t (*funcptr)(uint32_t);

void App_Init(uint32_t u32BootBase);
void DEBUG_PORT_Init(void);

/*----------------------------------------------------------------------------
  NonSecure Callable Functions from Secure Region
 *----------------------------------------------------------------------------*/
extern int32_t Encrypt_data(uint8_t *plainData, uint8_t *cipheredData);
extern int32_t Decrypt_data(uint8_t *cipheredData, uint8_t *resultData);
extern void print_Block(uint8_t *block);
extern void print2Secure(char *string);
extern int32_t Secure_LED_On(void);
extern int32_t Secure_LED_Off(void);


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    DEBUG_PORT_Init();

    print2Secure("\n");
    print2Secure("+---------------------------------------------+\n");
    print2Secure("|           Nonsecure is running ...          |\n");
    print2Secure("+---------------------------------------------+\n");

    XOM3_Func(3);
    XOM2_Func(2);

    Secure_LED_On();

    __attribute__((aligned(4))) uint8_t plainData[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    __attribute__((aligned(4))) uint8_t cipheredData[16] = {0};
    __attribute__((aligned(4))) uint8_t resultData[16] = {0};

    printf("&plainData  = %p\n",plainData);
    print_Block(plainData);
    CLK_SysTickLongDelay(2000000);
	
    print2Secure("AES encrypt start.\n");
    int c = Encrypt_data(plainData, cipheredData);
    print2Secure("AES encrypt done.\n");
    printf("&cipheredData  = %p\n",cipheredData);
    print_Block(cipheredData);
	CLK_SysTickLongDelay(2000000);
	
    print2Secure("AES decrypt start.\n");
    int r = Decrypt_data(cipheredData, resultData);
    print2Secure("AES decrypt done.\n");
    printf("&resultData  = %p\n",resultData);
    print_Block(resultData);
	CLK_SysTickLongDelay(2000000);

    uint8_t error = 0;
    for (uint8_t i = 0 ; i < 16 ; i++) {

        if (resultData[i] != plainData[i]) {

            error++;

        }

    }
    if (error != 0) {

        print2Secure("Error plainData is not equal to resultData");

    }
    else print2Secure("No error plainData is equal to resultData");
		
    Secure_LED_Off();
		

		do {
			__WFI();
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


void App_Init(uint32_t u32BootBase)
{
    funcptr fp;
    uint32_t u32StackBase;

    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    fp = ((funcptr)(*(((uint32_t *)SCB->VTOR) + 1)));

    /* Check if the stack is in secure SRAM space */
    u32StackBase = M32(u32BootBase);
    printf("u32StackBase = %x",u32StackBase);
    if((u32StackBase >= 0x30000000UL) && (u32StackBase < 0x40000000UL))
    {
        printf("Execute non-secure code ...\n");
        /* SCB.VTOR points to the target Secure vector table base address. */
        SCB->VTOR = u32BootBase;

        fp(0); /* Non-secure function call */
    }
    else
    {
        /* Something went wrong */
        printf("No code in non-secure region!\n");

        while(1);
    }
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
