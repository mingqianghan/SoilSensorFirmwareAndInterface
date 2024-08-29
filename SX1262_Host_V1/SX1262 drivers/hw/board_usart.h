/*
 * board_usart.h
 *
 *  Created on: Jun 26, 2023
 *      Author: mingqiang
 */

#ifndef HW_BOARD_USART_H_
#define HW_BOARD_USART_H_

#include "stm32l1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Function to read data from USART
void UART_Start_Receive();
bool Get_Received_Line(uint8_t *buffer);


#endif /* HW_BOARD_USART_H_ */
