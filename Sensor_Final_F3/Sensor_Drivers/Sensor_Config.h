/*
 * Sensor_Config.h
 *
 *  Created on: Jun 13, 2023
 *      Author: mingqiang
 */

#ifndef SENSOR_CONFIG_H_
#define SENSOR_CONFIG_H_

#define SensorID        3          // Must be changed for a new sensor node (0-255)



// ---------- Define date and time uploading to MCU ---------
#define userYear        24 // must be 0-99
#define userMonth       6  // must be 1-12
#define userDate        10 // must be 1-31
#define userWeekday     1  // must be 1-7
#define userHour        15 // must be 0-23
#define userMinute      0  // must be 0-59
#define userSecond      0  // must be 0-59

// ---------- Define wake up times and sleep duration  ---------
#define DSleepHourAm    11  // enter deep sleep at 11:59:30 in a day
#define DSleepHourPm    23  // enter deep sleep at 23:59:30 in a day
#define DsleepDurAm     10  // deep sleep 10 hours (10*60*60s), wake up at 21:59:30 the same day
#define DsleepDurPm     10  // deep sleep 10 hours (10*60*60s), wake up at 9:59:30 the next day



// A specific value to check if setup was done
#define FLAG_VALUE      0xA5A5
// backup register 1 used for status check after waking up from standby mode
#define FLAG_ADDRESS    RTC_BKP_DR1

// ----------------- SD Card Interface Pins ------------------
/*
 * SDIO 4-bit mode
 *
 * PC7  <----> Card Detection
 * PC8  <----> D0
 * PC9  <----> D1
 * PC10 <----> D2
 * PC11 <----> D3
 * PC12 <----> CLK
 * PD2  <----> CMD
 *
 */
#define uSD_Det_Pin             GPIO_PIN_7
#define uSD_Det_GPIO_Port       GPIOC
#define uSD_D0_Pin              GPIO_PIN_8
#define uSD_D0_GPIO_Port        GPIOC
#define uSD_D1_Pin              GPIO_PIN_9
#define uSD_D1_GPIO_Port        GPIOC
#define uSD_D2_Pin              GPIO_PIN_10
#define uSD_D2_GPIO_Port        GPIOC
#define uSD_D3_Pin              GPIO_PIN_11
#define uSD_D3_GPIO_Port        GPIOC
#define uSD_CLK_Pin             GPIO_PIN_12
#define uSD_CLK_GPIO_Port       GPIOC
#define uSD_CMD_Pin             GPIO_PIN_2
#define uSD_CMD_GPIO_Port       GPIOD



// ---------------- RF Switch Interface Pins -----------------
/*
 * PD9  <----> RF_A
 * PB11 <----> RF_B
 *
 */
 #define RF_SWA_Pin                    GPIO_PIN_9
 #define RF_SWA_GPIO_Port              GPIOD
 #define RF_SWB_Pin                    GPIO_PIN_11
 #define RF_SWB_GPIO_Port              GPIOB


// ------------------- AD9833 Interface Pins ------------------
/*
 * Software SPI
 *
 * PB15 <----> SPI CS
 * PB14 <----> SPI CLK
 * PB13 <----> SPI MOSI
 *
 */
#define AD9833_SPI_MOSI_Pin           GPIO_PIN_13
#define AD9833_SPI_MOSI_GPIO_Port     GPIOB
#define AD9833_SPI_CLK_Pin            GPIO_PIN_14
#define AD9833_SPI_CLK_GPIO_Port      GPIOB
#define AD9833_SPI_CS_Pin             GPIO_PIN_15
#define AD9833_SPI_CS_GPIO_Port       GPIOB



// ------------------- AD9914 Interface Pins ------------------
/*
 * PA3  <----> SPI CS
 * PA5  <----> SPI CLK
 * PA7  <----> SPI MOSI
 * PD13 <----> Reset
 * PD14 <----> I/O Update
 */
#define AD9914_SPI_CS_Pin          GPIO_PIN_3
#define AD9914_SPI_CS_GPIO_Port    GPIOA
#define AD9914_SPI_CLK_Pin         GPIO_PIN_5
#define AD9914_SPI_CLK_GPIO_Port   GPIOA
#define AD9914_SPI_MOSI_Pin        GPIO_PIN_7
#define AD9914_SPI_MOSI_GPIO_Port  GPIOA
#define AD9914_RESET_Pin           GPIO_PIN_13
#define AD9914_RESET_GPIO_Port     GPIOD
#define AD9914_IOUPDATE_Pin        GPIO_PIN_14
#define AD9914_IOUPDATE_GPIO_Port  GPIOD


// ------------------- Power Control Pins ------------------
/*
 * PE9  <----> EN1 (3.3V)
 * PE10 <----> EN2 (1.8V)
 * PE11 <----> EN3 (5.0V)
 */
#define POWER_EN1_Pin              GPIO_PIN_9
#define POWER_EN1_GPIO_Port        GPIOE
#define POWER_EN2_Pin              GPIO_PIN_10
#define POWER_EN2_GPIO_Port        GPIOE
#define POWER_EN3_Pin              GPIO_PIN_11
#define POWER_EN3_GPIO_Port        GPIOE


// ------------------- LED Pins ------------------
/*
 * PE9  <----> EN1 (3.3V)
 * PE10 <----> EN2 (1.8V)
 * PE11 <----> EN3 (5.0V)
 */
#define LED_RED_Pin                 GPIO_PIN_13
#define LED_RED_GPIO_Port           GPIOE
#define LED_GREEN_Pin               GPIO_PIN_14
#define LED_GREEN_GPIO_Port         GPIOE


// --------------------------- SWD ---------------------------
#define SWDIO_Pin               GPIO_PIN_13
#define SWDIO_GPIO_Port         GPIOA
#define SWCLK_Pin               GPIO_PIN_14
#define SWCLK_GPIO_Port         GPIOA
#define SWO_Pin                 GPIO_PIN_3
#define SWO_GPIO_Port           GPIOB




// ------------------- RADIO Interface Pins ------------------
/*
 * PE2  <----> RADIO RESET
 * PE3  <----> RADIO DEVICE SEL
 * PE4  <----> RADIO BUSY
 * PE5  <----> RADIO ANT SWITCH
 * PE7  <----> RADIO DIO1
 * PB0  <----> RADIO SPI MOSI
 * PB1  <----> RADIO SPI MISO
 * PB2  <----> RADIO SPI SCLK
 * PC5  <----> RADIO NSS
 *
 */
// #define BOARD_TCXO_WAKEUP_TIME      5
#define BOARD_TCXO_WAKEUP_TIME      0

#define RADIO_RESET_Pin             GPIO_PIN_2
#define RADIO_RESET_GPIO_Port       GPIOE
#define RADIO_DEVICE_SEL_Pin        GPIO_PIN_3
#define RADIO_DEVICE_SEL_GPIO_Port  GPIOE
#define RADIO_BUSY_Pin              GPIO_PIN_4
#define RADIO_BUSY_GPIO_Port        GPIOE
#define RADIO_ANT_SW_Pin            GPIO_PIN_5
#define RADIO_ANT_SW_GPIO_Port      GPIOE
#define RADIO_DIO_1_Pin             GPIO_PIN_7
#define RADIO_DIO_1_GPIO_Port       GPIOE
#define RADIO_SPI_MOSI_Pin          GPIO_PIN_0
#define RADIO_SPI_MOSI_GPIO_Port    GPIOB
#define RADIO_SPI_MISO_Pin          GPIO_PIN_1
#define RADIO_SPI_MISO_GPIO_Port    GPIOB
#define RADIO_SPI_SCLK_Pin          GPIO_PIN_2
#define RADIO_SPI_SCLK_GPIO_Port    GPIOB
#define RADIO_NSS_Pin               GPIO_PIN_5
#define RADIO_NSS_GPIO_Port         GPIOC


// -------------------- ADC Interface Pins -------------------
/*
 * PA1  <----> Phase
 * PA0  <----> Magnitude
 *
 */
#define PHS_ADC_Pin              GPIO_PIN_1
#define PHS_ADC_GPIO_Port        GPIOA
#define MAG_ADC_Pin              GPIO_PIN_0
#define MAG_ADC_GPIO_Port        GPIOA

#endif /* SENSOR_CONFIG_H_ */






