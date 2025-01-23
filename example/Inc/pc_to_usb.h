/*
 * pc_to_usb.h
 *
 *  Created on: Jan 21, 2025
 *      Author: kani
 */

#ifndef INC_PC_TO_USB_H_
#define INC_PC_TO_USB_H_

#include "main.h"
#include "usbd_cdc_if.h"

void
	send_string(char * str),
	PRINT_HEADER(void),
	PRINT_NETINFO(wiz_NetInfo *NETINFO),
	PRINT_CLOCK(void);

#define SEPARATOR            "=============================================\r\n"
#define WELCOME_MSG  		 "Welcome to STM32 Wiznet configuration\r\n"
#define READY_MSG			 "Ready to work\r\n"
#define CLOCK_MSG			 "CLOCK configuration\r\n"
#define SYSCLK_HCLK_MSG		 "SysClk: %dMHz\r\tHCLK: %dMHz\r\n"
#define PCLK_1_2_MSG		 "PCLK1: %dMHz\r\tPCLK2: %dMHz\r\n"
#define NETWORK_MSG  		 "Network configuration:\r\n"
#define IP_MSG 		 		 "  IP ADDRESS:  %d.%d.%d.%d\r\n"
#define NETMASK_MSG	         "  NETMASK:     %d.%d.%d.%d\r\n"
#define GW_MSG 		 		 "  GATEWAY:     %d.%d.%d.%d\r\n"
#define MAC_MSG		 		 "  MAC ADDRESS: %x:%x:%x:%x:%x:%x\r\n"
#define GREETING_MSG 		 "Well done guys! Welcome to the IoT world. Bye!\r\n"
#define CONN_ESTABLISHED_MSG "Connection established with remote IP: %d.%d.%d.%d:%d\r\n"
#define SENT_MESSAGE_MSG	 "Sent a message. Let's close the socket!\r\n"
#define WRONG_RETVAL_MSG	 "Something went wrong; return value: %d\r\n"
#define WRONG_STATUS_MSG	 "Something went wrong; STATUS: %d\r\n"
#define LISTEN_ERR_MSG		 "LISTEN Error!\r\n"

#endif /* INC_PC_TO_USB_H_ */
