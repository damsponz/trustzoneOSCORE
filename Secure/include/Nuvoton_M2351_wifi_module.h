/*########################################################
 * @file       : Nuvoton_M2351_wifi_module.h
 * @version    : v1.00
 * @created on : 11 mars 2019
 * @updated on : 12 mars 2019
 * @author     : Damien SOURSAS
 *
 * @note       : Header WiFi module
/*########################################################*/

#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

#ifndef NUVOTON_M2351_WIFI_MODULE_H_
#define NUVOTON_M2351_WIFI_MODULE_H_

#define WIFI_PORT   UART3    // Used to connect to WIFI module
#define LED_Y           PA11 
#define LED_G           PA10 // Green LED
#define PWR_OFF         PD7
#define FW_UPDATE_OFF   PD6
#define IOCTL_INIT      { \
    PD->MODE = (GPIO_MODE_OUTPUT << 6*2) | (GPIO_MODE_OUTPUT << 7*2); \
    PA->MODE = (GPIO_MODE_OUTPUT << 10*2) | (GPIO_MODE_OUTPUT << 11*2) ;}

void WIFI_PORT_Init();
void WIFI_PORT_Start();
void WIFI_PORT_Read(int);
void WIFI_PORT_Write(int, char *, int);

#endif /* NUVOTON_M2351_WIFI_MODULE_H_ */
