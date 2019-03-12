/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Secure main code for TrustZone OSCORE
 *
 * @note
 * AES
 *
 ******************************************************************************/

#include <arm_cmse.h>
#include "Oscore_crypto.h"
#include "Nuvoton_M2351_wifi_module.h"
#include <stdio.h>
#include "NuMicro.h"                      /* Device header */
#include "partition_M2351.h"
#include "xom0_func.h"
#include "xom1_func.h"

#include <string.h>
#include "M2351.h"

#define SW2             PB0
#define SW3             PB1

#define NEXT_BOOT_BASE  0x10040000
#define JUMP_HERE       0xe7fee7ff      /* Instruction Code of "B ." */


/* typedef for NonSecure callback functions */
typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
typedef int32_t (*Secure_funcptr)(uint32_t);

/*----------------------------------------------------------------------------
  NonSecure callable function for NonSecure callback
 *----------------------------------------------------------------------------*/
/*
NonSecure_funcptr pfNonSecure_LED_On = (NonSecure_funcptr)NULL;
NonSecure_funcptr pfNonSecure_LED_Off = (NonSecure_funcptr)NULL;

__NONSECURE_ENTRY
int32_t Secure_LED_On_callback(NonSecure_funcptr *callback)
{
    pfNonSecure_LED_On = (NonSecure_funcptr)cmse_nsfptr_create(callback);
    return 0;
}

__NONSECURE_ENTRY
int32_t Secure_LED_Off_callback(NonSecure_funcptr *callback)
{
    pfNonSecure_LED_Off = (NonSecure_funcptr)cmse_nsfptr_create(callback);
    return 0;
}
*/


void SYS_Init(void);
void DEBUG_PORT_Init(void);
void Boot_Init(uint32_t u32BootBase);

/*----------------------------------------------------------------------------
    Boot_Init function is used to jump to next boot code.
 *----------------------------------------------------------------------------*/
void Boot_Init(uint32_t u32BootBase)
{
    NonSecure_funcptr fp;

    /* SCB_NS.VTOR points to the Non-secure vector table base address. */
    SCB_NS->VTOR = u32BootBase;

    /* 1st Entry in the vector table is the Non-secure Main Stack Pointer. */
    __TZ_set_MSP_NS(*((uint32_t *)SCB_NS->VTOR));      /* Set up MSP in Non-secure code */

    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    fp = ((NonSecure_funcptr)(*(((uint32_t *)SCB_NS->VTOR) + 1)));

    /* Clear the LSB of the function address to indicate the function-call
       will cause a state switch from Secure to Non-secure */
    fp = cmse_nsfptr_create(fp);

    /* Check if the Reset_Handler address is in Non-secure space */
    if(cmse_is_nsfptr(fp) && (((uint32_t)fp & 0xf0000000) == 0x10000000))
    {
        printf("Next Boot to non-secure code ...\n");
        fp(0); /* Non-secure function call */
    }
    else
    {
        /* Something went wrong */
        printf("No code in non-secure region!\n");
        printf("CPU will halted at non-secure state\n");

        /* Set nonsecure MSP in nonsecure region */
        __TZ_set_MSP_NS(NON_SECURE_SRAM_BASE + 512);

        /* Try to halted in non-secure state (SRAM) */
        M32(NON_SECURE_SRAM_BASE) = JUMP_HERE;
        fp = (NonSecure_funcptr)(NON_SECURE_SRAM_BASE + 1);
        fp(0);

        while(1);
    }
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    SYS_UnlockReg();

    SYS_Init();

    /* UART0 is configured as debug port */
    DEBUG_PORT_Init();
    WIFI_PORT_Init();

    XOM1_Func(1);
    XOM0_Func(0);

    /* Init GPIO Port A for secure LED control */
    GPIO_SetMode(PA, BIT11 | BIT10, GPIO_MODE_OUTPUT);

    /* Enable debounce of PB0~1 */
    PB->DBEN = 0x3;
    PB->DBCTL |= GPIO_DBCTL_DBCLKSRC_Msk | 0x3;

    if (DEMO) {
        printf("+---------------------------------------------+\n");
        printf("|             Secure is running ...           |\n");
        printf("+---------------------------------------------+\n");
	}

    WIFI_PORT_Start();
    
    //printf("&cipheredSessionKey = %p\n", cipheredSessionKey);
    //print_Block((uint8_t *)cipheredSessionKey);

    char command_CWLIF[] = "AT+CWLIF\r\n";

    while(1)
    {

        if(SW3 == 0)
        {
            while(SW3 == 0); //Attente du front descendant du bouton
            Boot_Init(NEXT_BOOT_BASE);   
        }


        if(SW2 == 0)
        {
            while(SW2 == 0); //Attente du front descendant du bouton

            WIFI_PORT_Write(0, command_CWLIF, (sizeof(command_CWLIF) / sizeof(char))-1);
            WIFI_PORT_Read(1);

            char charToSend[30] = "Response from Nuvoton Board !\n";
            char *receive;

            while(1) {

                receive = WIFI_PORT_Receive_Data(0);
                printf("\nData Received : %s\n", receive);
                free(receive);
                WIFI_PORT_Send_Data(1, charToSend, 30, "30", 2);

                if(SW2 == 0)
                {
                    while(SW2 == 0); //Attente du front descendant du bouton
                    break;
                }

            }
   
        }
                
    }

}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable PLL */
    CLK->PLLCTL = CLK_PLLCTL_128MHz_HIRC;

    /* Waiting for PLL stable */
    while((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0);

    /* Set HCLK divider to 2 */
    CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_HCLKDIV_Msk)) | 1;

    /* Switch HCLK clock source to PLL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;

    CLK->PWRCTL |= CLK_PWRCTL_HIRC48EN_Msk;
    while((CLK->STATUS & CLK_STATUS_HIRC48STB_Msk) == 0);
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLKSEL_HIRC48;

    /* Select UART clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | CLK_CLKSEL1_UART0SEL_HIRC;
    CLK->CLKSEL3 = CLK_CLKSEL3_UART3SEL_HIRC;

    /* Enable UART clock */
    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk | CLK_APBCLK0_TMR0CKEN_Msk | CLK_APBCLK0_UART1CKEN_Msk | CLK_APBCLK0_UART3CKEN_Msk;

    /* Enable Crypto Accelerator */
    CLK->AHBCLK  |= CLK_AHBCLK_CRPTCKEN_Msk;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = 128000000;           // PLL
    SystemCoreClock = 128000000 / 2;       // HCLK
    CyclesPerUs     = 64000000 / 1000000;  // For SYS_SysTickDelay()

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(UART0_RXD_PB12_Msk | UART0_TXD_PB13_Msk))) | UART0_RXD_PB12 | UART0_TXD_PB13;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init for nonsecure code                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk;
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART1SEL_Msk)) | CLK_CLKSEL1_UART1SEL_HIRC;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_RXD_PA2_Msk)) | UART1_RXD_PA2;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_TXD_PA3_Msk)) | UART1_TXD_PA3;

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
