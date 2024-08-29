/*
 * AD9833.h
 *
 *  Created on: May 30, 2023
 *      Author: mingqiang
 */

#ifndef AD9833_H_
#define AD9833_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "Sensor_Config.h"


// Master Clock (24MHz) in kHz
#define AD9833_MCLK                   24000

// Used in calculating frequency tuning words
#define AD_2POW28                     (uint32_t)(1 << 28)

// AD9833 commands
#define AD9833_CMD_WRITE_FREQ_0       0x4000
#define AD9833_CMD_WRITE_FREQ_1       0x8000
#define AD9833_CMD_WRITE_PHASE_0      0xC000
#define AD9833_CMD_WRITE_PHASE_1      0xE000


// AD9833 waveform shapes
typedef enum{
	AD9833_WAVE_SINE,
	AD9833_WAVE_TRIANGLE,
	AD9833_WAVE_SQUARE
}AD9833_Wave_Shape;

// ----------------- Control Register Defines -----------------

// MODE = 1: triangle output; MODE = 0: sinusoidal output.
#define AD9833_MODE 	              1

// DIV2 = 1: the MSB of the DAC data is passed to the VOUT pin.
// DIV2 = 0: the MSB/2 of the DAC data is output at the VOUT pin.
#define AD9833_DIV2 	              3


// OPBITEN = 1: output of the DAC is no longer available at the VOUT pin, replaced by MSB (or MSB/2) of the DAC.
// OPBITEN = 0: DAC is connected to VOUT.
#define AD9833_OPBITEN                5

// Power Bits
#define AD9833_SLEEP12                6
#define AD9833_SLEEP1                 7

// Reset = 1 resets internal registers to 0.
// Reset = 0 disables reset.
#define AD9833_RESET	              8

//  B28 = 1 allows a complete word to be loaded into
//  a frequency register in two consecutive writes.
#define AD9833_B28 		              13



// ------------------------- Functions -------------------------
// set a bit
#define AD9833_bitSet(value, bit) ((value) |= (1UL << (bit)))
// clear a bit
#define AD9833_bitClear(value, bit) ((value) &= ~(1UL << (bit)))
// do nothing (little delay)
#define ASM_NOP() asm("NOP")


//  Write data to AD9833 through
void AD9833_Write_Register(uint16_t Value);
void AD9833_SetFrequency(float fout);
void AD9833_SetPhase(float pout);
void AD9833_SetShape(AD9833_Wave_Shape shape);
void AD9833_Reset(bool _reset);
void AD9833_Generate_Wave(float fout);
void AD9833_Init();
void AD9833_Power_Down(bool _pwrdwn);
void AD9833_IO_LOW();

#endif /* AD9833_H_ */
