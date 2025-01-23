/*
 * pc_to_usb.c
 *
 *  Created on: Jan 21, 2025
 *      Author: kani
 */


#include "pc_to_usb.h"

char msg[64];

void send_string (char *str)
{
	CDC_Transmit_FS((uint8_t *) str, strlen(str));  // transmit in blocking mode
}

void PRINT_HEADER(void)
{
	sprintf(msg,SEPARATOR);
	send_string(msg);
	sprintf(msg,WELCOME_MSG);
	send_string(msg);
	sprintf(msg,SEPARATOR);
	send_string(msg);
	PRINT_CLOCK();
	send_string(msg);
}

void PRINT_CLOCK(void)
{
	uint8_t
		sysclk = HAL_RCC_GetSysClockFreq()/1000000,
		hclk = HAL_RCC_GetHCLKFreq()/1000000,
		pclk1 = HAL_RCC_GetPCLK1Freq()/1000000,
		pclk2 = HAL_RCC_GetPCLK2Freq()/1000000;
	send_string(CLOCK_MSG);
	sprintf(msg,SYSCLK_HCLK_MSG,sysclk,hclk);
	send_string(msg);
	sprintf(msg,PCLK_1_2_MSG,pclk1,pclk2);
	send_string(msg);
}

void PRINT_NETINFO(wiz_NetInfo *NETINFO)
{
	send_string(NETWORK_MSG);
	sprintf(msg, MAC_MSG, NETINFO->mac[0], NETINFO->mac[1], NETINFO->mac[2], NETINFO->mac[3], NETINFO->mac[4], NETINFO->mac[5]);
	send_string(msg);
	sprintf(msg, IP_MSG, NETINFO->ip[0], NETINFO->ip[1], NETINFO->ip[2], NETINFO->ip[3]);
	send_string(msg);
	sprintf(msg, NETMASK_MSG, NETINFO->sn[0], NETINFO->sn[1], NETINFO->sn[2], NETINFO->sn[3]);
	send_string(msg);
	sprintf(msg, GW_MSG, NETINFO->gw[0], NETINFO->gw[1], NETINFO->gw[2], NETINFO->gw[3]);
	send_string(msg);
}
