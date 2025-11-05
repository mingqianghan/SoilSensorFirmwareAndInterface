#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__


#include "stm32l1xx_hal.h"

// #define BOARD_TCXO_WAKEUP_TIME                      5
#define BOARD_TCXO_WAKEUP_TIME                      0

#define RADIO_RESET_Pin GPIO_PIN_0
#define RADIO_RESET_GPIO_Port GPIOA
#define RADIO_DEVICE_SEL_Pin GPIO_PIN_4
#define RADIO_DEVICE_SEL_GPIO_Port GPIOA
#define RADIO_SCLK_Pin GPIO_PIN_5
#define RADIO_SCLK_GPIO_Port GPIOA
#define RADIO_MISO_Pin GPIO_PIN_6
#define RADIO_MISO_GPIO_Port GPIOA
#define RADIO_MOSI_Pin GPIO_PIN_7
#define RADIO_MOSI_GPIO_Port GPIOA
#define RADIO_NSS_Pin GPIO_PIN_8
#define RADIO_NSS_GPIO_Port GPIOA
#define RADIO_ANT_SWITCH_POWER_Pin GPIO_PIN_9
#define RADIO_ANT_SWITCH_POWER_GPIO_Port GPIOA
#define RADIO_BUSY_Pin GPIO_PIN_3
#define RADIO_BUSY_GPIO_Port GPIOB
#define RADIO_DIO_1_Pin GPIO_PIN_4
#define RADIO_DIO_1_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif // __BOARD_CONFIG_H__
