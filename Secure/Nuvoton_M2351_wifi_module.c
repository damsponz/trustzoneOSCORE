/*########################################################
 * @file       : Nuvoton_M2351_wifi_module.c
 * @version    : v1.00
 * @created on : 11 mars 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : WiFi Module
/*########################################################*/


#include "Nuvoton_M2351_wifi_module.h"

void WIFI_PORT_Init()
{
    CLK->APBCLK0 |= CLK_APBCLK0_UART3CKEN_Msk;
    CLK->CLKSEL3 = (CLK->CLKSEL3 & (~CLK_CLKSEL3_UART3SEL_Msk)) | CLK_CLKSEL3_UART3SEL_HIRC;

    WIFI_PORT->LINE = UART_PARITY_NONE | UART_STOP_BIT_1 | UART_WORD_LEN_8;
    WIFI_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);

    /* Set multi-function pins for RXD and TXD */
    //SYS->GPC_MFPL = (SYS->GPC_MFPL & (~(UART4_RXD_PC6_Msk | UART4_TXD_PC7_Msk))) | UART4_RXD_PC6 | UART4_TXD_PC7;
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~(UART3_RXD_PD0_Msk | UART3_TXD_PD1_Msk))) | UART3_RXD_PD0 | UART3_TXD_PD1;
}

void WIFI_PORT_Start()
{

    if (DEMO) {
        printf("|            WiFi Module Loading  ...         |\n");
        printf("|                                             |\n");
    }

    IOCTL_INIT;
    PWR_OFF = 1;
    FW_UPDATE_OFF = 1;

    //FW_UPDATE_OFF = 0; // Set 0 to enable WIFI module firmware update.
    //FW_UPDATE_OFF = 1; // Set 1 to Disable WIFI module firmware update.
    CLK_SysTickLongDelay(1000000);

    LED_Y = 1;
    LED_G = 1;
    PWR_OFF = 0;
    FW_UPDATE_OFF = 1;

    char command_AT[] = "AT\r\n";
    char command_CWMODE[] = "AT+CWMODE=2\r\n";
    char command_CWDHCP[] = "AT+CWDHCP=0,0\r\n";
    char command_CWSAP[] = "AT+CWSAP=\"NuvotonBoard\",\"oscorepass\",5,3\r\n";
    char command_CIPAP[] = "AT+CIPAP=\"192.168.4.254\"\r\n";
    char command_CIPMUX[] = "AT+CIPMUX=1\r\n";
    char command_CIPSERVER[] = "AT+CIPSERVER=1,5386\r\n";
    char command_CIPSTO[] = "AT+CIPSTO?\r\n";
    char buff;
    int ready = 0;
    int indx = 0;
    int loop = 0;
    int wifiUP = 0;

    while(wifiUP == 0) {
        LED_G = 0;
        //while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_RXIDLE_Msk) == 1);
        while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) {
            loop = 0;
            buff = WIFI_PORT->DAT;
            //printf("%c",buff);
            if(indx == 0 && buff == 'r' && loop == 0) {
                indx = 1;
                loop = 1;
                //printf("%c",buff);
            }
            if(indx == 1 && buff == 'e' && loop == 0) {
                indx = 2;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 1 && loop == 0) indx = 0;

            if(indx == 2 && buff == 'a' && loop == 0) {
                indx = 3;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 2 && loop == 0) indx = 0;

            if(indx == 3 && buff == 'd' && loop == 0) {
                indx = 4;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 3 && loop == 0) indx = 0;

            if(indx == 4 && buff == 'y' && loop == 0) {
                ready = 1;
                loop = 1;
                //printf("%c",buff);
            }
            else if (indx == 4 && loop == 0) indx = 0;
        }
        if(ready) {

            WIFI_PORT_Write(0, command_AT, (sizeof(command_AT) / sizeof(char))-1);
            WIFI_PORT_Read(0);

            WIFI_PORT_Write(0, command_CWMODE, (sizeof(command_CWMODE) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|           WiFi Access Point Enabled         |\n");

            WIFI_PORT_Write(0, command_CWDHCP, (sizeof(command_CWDHCP) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|               WiFi DHCP Enabled             |\n");

            WIFI_PORT_Write(0, command_CWSAP, (sizeof(command_CWSAP) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|           WiFi SSID :\"NuvotonBoard\"         |\n");
            if (DEMO) printf("|           WiFi WPA2 : oscorepass            |\n");

            WIFI_PORT_Write(0, command_CIPAP, (sizeof(command_CIPAP) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|     WiFi Nuvoton IP : 192.168.4.254         |\n");

            WIFI_PORT_Write(0, command_CIPMUX, (sizeof(command_CIPMUX) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|  WiFi Multiple Server Connections Enabled   |\n");

            WIFI_PORT_Write(0, command_CIPSERVER, (sizeof(command_CIPSERVER) / sizeof(char))-1);
            WIFI_PORT_Read(0);
            if (DEMO) printf("|       Server TCP enabled on 5386 port       |\n");

            //WIFI_PORT_Write(command_CIPSTO, (sizeof(command_CIPSTO) / sizeof(char))-1);
            //printf("WiFi Nuvoton Timeout is : \n");
            //WIFI_PORT_Read(1);

            if (DEMO) {
                printf("|                                             |\n");
                printf("|        WiFi Module ESP8266 is Ready         |\n");
                printf("+---------------------------------------------+\n");
            }
            wifiUP = 1;

        }
        LED_G = 1;
    }


}

void WIFI_PORT_Read(int print)
{

    LED_G = 0;
    char buffer = 0;
    char lastBuffer = 0;
    int cmdOK = 0;

    LED_G = 0;
    while(cmdOK == 0) {
        
        while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) {
            buffer = (char)WIFI_PORT->DAT;
            if (print) printf("%c",buffer);
            if (lastBuffer == 'O' && buffer == 'K') {
                cmdOK = 1;
            }
            else lastBuffer = buffer;
        }

    }
    LED_G = 1;

}
void WIFI_PORT_Write(int print, char *command, int nbCharCommand)
{
    
    for (int i = 0; i < nbCharCommand; i++) {
        while((WIFI_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk));
        WIFI_PORT->DAT = command[i];
        if(print) printf("%c", command[i]);
    }

}


