/*
 * Radio_Process.h
 *
 *  Created on: Jun 24, 2023
 *      Author: mingqiang
 */

#ifndef RADIO_PROCESS_H_
#define RADIO_PROCESS_H_


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

/*!
 * \brief Function executed on Radio Rx Error event
 */
void Radio_TX( uint16_t *buffer, uint16_t size );


#endif /* Radio_PROCESS_H_ */
