/*
 * board_gpio.h
 *
 *  Created on: Jun 26, 2023
 *      Author: mingqiang
 */

#ifndef HW_BOARD_GPIO_H_
#define HW_BOARD_GPIO_H_

#include "stm32l1xx.h"

#define RADIO_BUSY()  HAL_GPIO_ReadPin(RADIO_BUSY_GPIO_Port,RADIO_BUSY_Pin)
#define RADIO_DIO1()  HAL_GPIO_ReadPin(RADIO_DIO_1_GPIO_Port,RADIO_DIO_1_Pin)

#define RADIO_NRST(STATUS)  HAL_GPIO_WritePin(RADIO_RESET_GPIO_Port,RADIO_RESET_Pin,STATUS)
#define RADIO_ANT_SW_ON(STATUS)  HAL_GPIO_WritePin(RADIO_ANT_SWITCH_POWER_GPIO_Port,RADIO_ANT_SWITCH_POWER_Pin,STATUS)



void DelayMs(uint32_t ms);
void GpioWrite(GPIO_TypeDef* GPIOx, uint16_t pin, GPIO_PinState STATUS);
GPIO_PinState GpioRead(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);





#endif /* HW_BOARD_GPIO_H_ */
