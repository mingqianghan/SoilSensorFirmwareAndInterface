/*
 * borad_gpio.c
 *
 *  Created on: Jun 26, 2023
 *      Author: mingqiang
 */

#include "board_gpio.h"
#include "radio.h"

void DelayMs(uint32_t ms)
{
	 HAL_Delay( ms );
}

void GpioWrite(GPIO_TypeDef* GPIOx, uint16_t pin, GPIO_PinState STATUS)
{
	HAL_GPIO_WritePin(GPIOx, pin, STATUS);
}

GPIO_PinState GpioRead(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}


void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
	RadioOnDioIrq();
}

