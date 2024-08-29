/*
 * board_spi.h
 *
 *  Created on: Jun 24, 2023
 *      Author: mingqiang
 */

#ifndef HW_BOARD_SPI_H_
#define HW_BOARD_SPI_H_


#include "stm32f4xx_hal.h"
#include "Sensor_Drivers.h"

#define Sx126x_NSS(STATUS)  HAL_GPIO_WritePin(RADIO_NSS_GPIO_Port,RADIO_NSS_GPIO_Pin,STATUS)
#define Sx126x_SCK(STATUS)  HAL_GPIO_WritePin(RADIO_SPI_SCLK_GPIO_Port,RADIO_SPI_SCLK_Pin,STATUS)
#define Sx126x_MOSI(STATUS) HAL_GPIO_WritePin(RADIO_SPI_MOSI_GPIO_Port,RADIO_SPI_MOSI_Pin,STATUS)
#define Sx126x_MISO()       HAL_GPIO_ReadPin(RADIO_SPI_MISO_GPIO_Port,RADIO_SPI_MISO_Pin)


uint8_t Sx126x_SpiInOut(uint8_t data);


#endif /* HW_BOARD_SPI_H_ */
