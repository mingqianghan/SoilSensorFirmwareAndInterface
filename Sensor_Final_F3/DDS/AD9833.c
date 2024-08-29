/*
 * AD9833.c
 *
 *  Created on: May 30, 2023
 *      Author: mingqiang
 */
#include "AD9833.h"

//extern SPI_HandleTypeDef hspi2;

uint16_t FRQLW = 0;    // LSB of Frequency Tuning Word
uint16_t FRQHW = 0;    // MSB of Frequency Tuning Word
uint16_t Phase = 0;    // Phase Value
uint16_t Ctl_reg = 0;      // Control register

void AD9833_Write_Register(uint16_t Value)
{
	/*
	 *  Write data to register through SPI
	 *  send 16-bit data each time
	 */

	// goes low to select AD9833
	HAL_GPIO_WritePin(AD9833_SPI_CS_GPIO_Port, AD9833_SPI_CS_Pin, GPIO_PIN_RESET);

	for (uint8_t i = 0; i <16; i++)
	{
		if (Value & 0x8000) HAL_GPIO_WritePin(AD9833_SPI_MOSI_GPIO_Port,AD9833_SPI_MOSI_Pin,GPIO_PIN_SET);
		else HAL_GPIO_WritePin(AD9833_SPI_MOSI_GPIO_Port,AD9833_SPI_MOSI_Pin,GPIO_PIN_RESET);
		ASM_NOP();
		HAL_GPIO_WritePin(AD9833_SPI_CLK_GPIO_Port,AD9833_SPI_CLK_Pin,GPIO_PIN_RESET);
		ASM_NOP();
		HAL_GPIO_WritePin(AD9833_SPI_CLK_GPIO_Port,AD9833_SPI_CLK_Pin,GPIO_PIN_SET);
		Value = Value << 1;
	}
	HAL_GPIO_WritePin(AD9833_SPI_MOSI_GPIO_Port,AD9833_SPI_MOSI_Pin,GPIO_PIN_RESET);
	ASM_NOP();

	// goes high after the data was sent
	HAL_GPIO_WritePin(AD9833_SPI_CS_GPIO_Port, AD9833_SPI_CS_Pin, GPIO_PIN_SET);
	// goes high to make sure it's HIGH when CS goes LOW
	HAL_GPIO_WritePin(AD9833_SPI_CLK_GPIO_Port,AD9833_SPI_CLK_Pin,GPIO_PIN_SET);
}


void AD9833_SetFrequency(float fout)
{
	/*
	 *
	 * Calculate frequency values loaded into frequency registers
	 *
	 * FREQREG = round(fout * 2^28 / FMCLK)
	 *
	 * FREQREG: frequency value (28 bits)
	 *          send 14 bits two times
	 *          14MSBs
	 *          14LSBs
	 *
	 * fout:    Output frequency
	 * FMCLK:   Master reference clock (24MHz)
	 *
	 */
	uint32_t FreqWord = (uint32_t)(fout * (AD_2POW28 / AD9833_MCLK) + 0.5);

	// data (bit 13- bit0)
	FRQHW = (uint16_t)((FreqWord >> 14) & 0x3FFF);        // FREQ MSB
	FRQLW = (uint16_t)(FreqWord & 0x3FFF);                   // FREQ LSB

	/*
	 * Register address (bit 15 an bit 14)
	 *
	 * Frequency register in two consecutive writes, bit 15 and bit 14 are
	 * set to 0 and 1, respectively, which is the Frequency Register 0 address.
	 *
	 */

	FRQHW |= AD9833_CMD_WRITE_FREQ_0;
	FRQLW |= AD9833_CMD_WRITE_FREQ_0;

	//FRQHW = 0x4000;
	//FRQLW = 0x50C7;

	/*
	 * D28=1, allows s a complete word to be loaded into a frequency register
	 * in two consecutive write. The first write contains 14 LSBs.
	 * The second write contains 14 MSBs
	 *
	 */
	//AD9833_Write_Register(Ctl_reg);

	// Frequency register: 14 LSBs
	AD9833_Write_Register(FRQLW);

	// Frequency register: 14 MSBs
	AD9833_Write_Register(FRQHW);
}


void AD9833_SetPhase(float pout)
{


    /*
     * phase = (pout * 4096) / (2*pi)
     *
     * phase: the value loaded to phase register
     * pout : desired phase value in degree
     *
     * Phase register address: first 3 bits of the 16 bits
     * 110x :PHASE0
     * 111x :PHASE1
     */
	Phase = ((uint16_t)(pout * (4096/360)+ 0.5) | AD9833_CMD_WRITE_PHASE_0);

	AD9833_Write_Register(Phase);
}



void AD9833_SetShape(AD9833_Wave_Shape shape)
{
	switch(shape)
	{
	case AD9833_WAVE_SINE:
		AD9833_bitClear(Ctl_reg, AD9833_OPBITEN);
		AD9833_bitClear(Ctl_reg, AD9833_MODE);
	break;
	case AD9833_WAVE_TRIANGLE:
		AD9833_bitClear(Ctl_reg, AD9833_OPBITEN);
		AD9833_bitSet(Ctl_reg, AD9833_MODE);
	break;
	case AD9833_WAVE_SQUARE:
		AD9833_bitSet(Ctl_reg, AD9833_OPBITEN);
		AD9833_bitClear(Ctl_reg, AD9833_MODE);
		AD9833_bitSet(Ctl_reg, AD9833_DIV2);
	break;
	}

	AD9833_Write_Register(Ctl_reg);
}



void AD9833_Reset(bool _reset)
{
	if (_reset) AD9833_bitSet(Ctl_reg, AD9833_RESET);   //reset
	else     AD9833_bitClear(Ctl_reg, AD9833_RESET);  // clear reset

	AD9833_Write_Register(Ctl_reg);
}

void AD9833_Power_Down(bool _pwrdwn)
{
	if (_pwrdwn)
	{
		AD9833_bitSet(Ctl_reg, AD9833_SLEEP1);
		AD9833_bitSet(Ctl_reg, AD9833_SLEEP12);
	}
	else
	{
		AD9833_bitClear(Ctl_reg, AD9833_SLEEP1);  // clear reset
		AD9833_bitClear(Ctl_reg, AD9833_SLEEP12);  // clear reset
	}
	AD9833_Write_Register(Ctl_reg);
}


void AD9833_Generate_Wave(float fout)
{
	Ctl_reg = 0;

	// Set Frequency register in two consecutive writes
	AD9833_bitSet(Ctl_reg, AD9833_B28);
	AD9833_Write_Register(Ctl_reg);

	// Reset to write register
	AD9833_Reset(true);

	// Set frequency
	AD9833_SetFrequency(fout);

	// Set phase
	// AD9833_SetPhase(pout);   comment out to always output phase of 0 degree

	// select frequency register 0 and phase register 0
	// clear reset hold to enable waveform output
	AD9833_Reset(false);

	// Set shape
	// AD9833_SetShape(shape);  comment to always output sine wave
}

void AD9833_Init()
{
	HAL_GPIO_WritePin(AD9833_SPI_CS_GPIO_Port, AD9833_SPI_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD9833_SPI_MOSI_GPIO_Port, AD9833_SPI_MOSI_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD9833_SPI_CLK_GPIO_Port, AD9833_SPI_CLK_Pin, GPIO_PIN_SET);
}

void AD9833_IO_LOW()
{
	HAL_GPIO_WritePin(AD9833_SPI_CS_GPIO_Port, AD9833_SPI_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9833_SPI_MOSI_GPIO_Port, AD9833_SPI_MOSI_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9833_SPI_CLK_GPIO_Port, AD9833_SPI_CLK_Pin, GPIO_PIN_RESET);
}
