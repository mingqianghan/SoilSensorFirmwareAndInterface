/*
 * Sensor_Driver.h
 *
 *  Created on: Jun 14, 2023
 *      Author: mingqiang
 */

#ifndef SENSOR_DRIVER_H_
#define SENSOR_DRIVER_H_

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "Sensor_Config.h"
#include "AD9833.h"
#include "AD9914.h"
#include "hw\board_gpio.h"
#include "sx126x\radio.h"
#include "sx126x\sx126x.h"
#include "radio-board\sx126x-board.h"
#include "radio-board\Radio_Process.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "gpio.h"


typedef enum{
	AD9833_LOWFREQ,    //DC - 1 MHz
	AD9914_HIGHFREQ    //1 MHz - 1GHz
}DDS_type;             //DDS type


typedef enum{
	Allbands,
	Option1,
	Option2,
	Option3
}Band_Selection;    // used for collecting or sending selection


void Radio_Init();
bool SDCard_Mount();
void DDS_Choose_Source(DDS_type s_type);
void Sensor_Collect_Data(Band_Selection Generating_band, Band_Selection Sending_band);
void Sensor_MX_Process();
void Sensor_ADC_Data_Average();
void Sensor_ADC_Process();
void Sensor_TX_Process( void );
void Shutoff_Data_Collection();
void MCU_Enter_Shallow_Sleep( void );
void MCU_Exit_Shallow_Sleep( void );
extern void SystemClock_Config(void);
void RTC_Time_Setup( void );
void Alarms_Setup( void );
void Set_RTC_Time( void );
void Get_Current_Time( void );
void MCU_Exit_Deep_Sleep( void );
void MCU_Enter_Deep_Sleep( void );
void Save_UG_RX_data( void );
void Current_Time_TX_Format( void );
uint8_t calculate_parity(uint8_t value);
uint8_t combine_parities(uint8_t *values);
void Time_Track( bool Exit_LP );
void Get_FreIDs( void );

#endif /* SENSOR_DRIVER_H_ */
