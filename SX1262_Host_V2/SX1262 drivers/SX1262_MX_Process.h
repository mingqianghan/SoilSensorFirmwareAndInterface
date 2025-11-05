/*
 * SX1262_MX_Process.h
 *
 *  Created on: Jun 24, 2023
 *      Author: mingqiang
 */

#ifndef SX1262_MX_PROCESS_H_
#define SX1262_MX_PROCESS_H_

#include <string.h>
#include <stdlib.h>
#include "board_gpio.h"
#include "radio.h"
#include "board_usart.h"
#include "sx126x.h"
#include "sx126x-board.h"


// Adjust based on the maximum expected number of sensors
#define MAX_CLIENTS                                 8

typedef struct {
    uint8_t one_index[MAX_CLIENTS];  // Indices where bits are 1
    uint8_t zero_index[MAX_CLIENTS]; // Indices where bits are 0
    uint8_t onesCount;  // Number of 1 bits
    uint8_t zerosCount; // Number of 0 bits
} BitIndices;


typedef struct {
	float latitude;                       // GPS latitude
	float longitude;                      // GPS longitude
	char locationName[20];                // Location name
    uint8_t sensorCount;                  // Number of sensors at this location
    uint8_t sensorIDs[MAX_CLIENTS];      // Array to hold sensor IDs
} Location_Info;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );
void MX_Send_Receive( void );
void Print_Radio_RX_Data( void );
void Raido_Init( void );
BitIndices findBitIndices(uint8_t value, uint8_t n);
void Sensor_Node_Local_time( void );
void UART_Sync_Check( void );
void DateTime_UART_To_Radio_Format( void );
void Radio_Test_Data_Format( void );
void Get_TimetoRadio( uint8_t NodeID, uint8_t cmd );
void Wait_UART_Start( uint8_t NodeID );
void Get_RandnumtoRadio( uint8_t cmd );
void Time_Calibration_Check( void );
bool Check_ReceivedData_Error( void );
uint8_t Calculate_Parity(uint8_t value);
void FrequencyIDs_From_UART( void );
void WorkingIDs_From_UART( void );
void FreIDs_To_Radio( void );
void UART_IDs_RX( void );
void Wait_UART_Confirm_Done( void );

#endif /* SX1262_MX_PROCESS_H_ */
