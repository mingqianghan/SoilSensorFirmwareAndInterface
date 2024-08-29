#include "AD9914.h"

extern SPI_HandleTypeDef hspi1;


/* ====== Startup Register Values ====== */
// Sine output; Enable OSK to control Amplitude
const uint8_t CFR1Start[4] = {0x00, 0x01, 0x01, 0x00};
const uint8_t CFR2Start[4] = {0x00, 0x80, 0x00, 0x00};
const uint8_t CFR3Start[4] = {0x00, 0x00, 0x19, 0x1C};
const uint8_t CFR4Start[4] = {0x00, 0x05, 0x21, 0x20};

// Command to enable the DAC calibration
// Should be 0x01xxxxxx, where x is the last 6 digits of CFR4Start
const uint8_t DACCAlEnable[4] = {0x01, 0x05, 0x21, 0x20};

const uint8_t ALLPWRDOWN[4] = {0x00, 0x01, 0x01, 0xE0};


uint8_t FTW_update[4];            // Frequency Tuning Words
uint8_t Pha_Amp_update[4];        // Amplitude and Phase Control Words


void AD9914_Init()
{
	AD9914_Send_Reset();
	AD9914_Initilize_DDS();
}


void AD9914_Send_IO_Update()
{
	/*
	 * The input/output update initiates the transfer of written data from
	 * the serial or parallel input/output port buffer to active registers.
	 * I/O_UPDATE is active on the rising edge, and the pulse width must
	 * be greater than one SYNC_CLK period.
	*/

	HAL_GPIO_WritePin(AD9914_IOUPDATE_GPIO_Port, AD9914_IOUPDATE_Pin, GPIO_PIN_RESET);
	AD9833_ASM_NOP(); // little delay
	// send high to activate it
	HAL_GPIO_WritePin(AD9914_IOUPDATE_GPIO_Port, AD9914_IOUPDATE_Pin, GPIO_PIN_SET);
	AD9833_ASM_NOP();
	// send low to end it
	HAL_GPIO_WritePin(AD9914_IOUPDATE_GPIO_Port, AD9914_IOUPDATE_Pin, GPIO_PIN_RESET);
}

void AD9914_Send_Reset()
{
	/*
	 * Reset Ad9914 at the initialization
	*/

	// send high to reset
	HAL_GPIO_WritePin(AD9914_RESET_GPIO_Port, AD9914_RESET_Pin, GPIO_PIN_SET);
	AD9833_ASM_NOP();
	// send low to end it
	HAL_GPIO_WritePin(AD9914_RESET_GPIO_Port, AD9914_RESET_Pin, GPIO_PIN_RESET);
	AD9833_ASM_NOP();
}


void AD9914_Write_Register(uint8_t RegisterAddress, const uint8_t* Value)
{
	/*
	 *  Write data to register through SPI
	 */

	// goes to low to select the cs pin
	HAL_GPIO_WritePin(AD9914_SPI_CS_GPIO_Port, AD9914_SPI_CS_Pin, GPIO_PIN_RESET);

	// send register address first
	HAL_SPI_Transmit(&hspi1, (uint8_t*)&RegisterAddress, sizeof(RegisterAddress), 100);

	// send 4 bytes of data
	HAL_SPI_Transmit(&hspi1, (uint8_t*)Value, sizeof(Value), 100);

	// goes to high release the cs pin
	HAL_GPIO_WritePin(AD9914_SPI_CS_GPIO_Port, AD9914_SPI_CS_Pin, GPIO_PIN_SET);
}


void AD9914_Initilize_DDS()
{
	AD9914_Write_Register(CFR1, CFR1Start);
	//AD9914_Send_IO_Update();
	AD9914_Write_Register(CFR2, CFR2Start);
	//AD9914_Send_IO_Update();
	AD9914_Write_Register(CFR3, CFR3Start);
	//AD9914_Send_IO_Update();
	AD9914_Write_Register(CFR4, CFR4Start);
	//AD9914_Send_IO_Update();
	// set half of full amplitude and phase 0
	AD9914_Write_Register(PhaAmpAddress0 ,AD9914_Phase_Amp_Set(0.5, 0));
	AD9914_Send_IO_Update();
}

void AD9914_Calibrate_DAC()
{
	/*
	 * The DAC calibration should be manually done and then cleared after each power-up and
	 * every time the REF CLK or internal system clock is changed. This initiates an internal
	 * calibration routine to optimize the setup and hold times for internal DAC timing.
	 * Failure to calibrate may degrade performance and even result in loss of functionality.
	 *
	 *                *DAC CAL enable bit in the e CFR4 control register (0x03[24])*
	 *
	 */
	AD9914_Write_Register(CFR4, DACCAlEnable);
	AD9914_Send_IO_Update();
	AD9914_Write_Register(CFR4, CFR4Start);
	AD9914_Send_IO_Update();
}

void AD9914_Power_Down(bool _disable)
{
	if(_disable) AD9914_Write_Register(CFR1 ,ALLPWRDOWN);// Set power-down bits to enter power-down mode
	else
	{
		AD9914_Write_Register(CFR1, CFR1Start); // clear power-down bits
		AD9914_Calibrate_DAC();                 // Calibrate DDS after recovering from Power down
		HAL_Delay(5);
	}

	AD9914_Send_IO_Update();
}


void AD9914_Generate_Wave(double fout)
{
	AD9914_Write_Register(FreqTWAddress0 ,AD9914_Calculate_frequency_tuning_word(fout));
	AD9914_Send_IO_Update();
}


uint8_t* AD9914_Calculate_frequency_tuning_word(double fout)
{
	/*
	 * Calculate Frequency Tuning Word (FTW)
	 *
	 * The equation is FTW = round(2^32(fout/fsysclk))
	 *
	 * fout: the desired frequency
	 * fsysclk: the reference clock frequency
	 *
	 */

	uint32_t ftemp =  (uint32_t)(fout * (AD_2POW32 / AD9914_MCLK)+0.5);

	// Frequency Tuning Word sent to AD9914 (MSB first)
	FTW_update[3] = ftemp & 0xff;
	FTW_update[2] = (ftemp >> 8) & 0xff;
	FTW_update[1] = (ftemp >> 16) & 0xff;
	FTW_update[0] = (ftemp >> 24) & 0xff;

	return FTW_update;
}

uint8_t* AD9914_Phase_Amp_Set(float Amp_scale, float Phase)
{
	/*
	 * Calculate Amplitude and Phase Tuning Word
	 *
	 * Amp_scale: amplitude scale 0-1 (1: Full scale - Vpp: ~600mV )
	 * Amp_word = Amp_scale * 2^12
	 *
	 * Phase: Phase in degree
	 * Phase_word = phase/360 *2^16
	 *
	 * They should be updated to the 32-bit register with the address of 0x0C
	 * Amplitude: bit [27:16]
	 * Phase    : bit [15:0]
	 *
	 */
	uint16_t Amp_word = (uint16_t)(Amp_scale * AD_2POW12 +0.5);
	uint16_t Phase_word = (uint16_t)(Phase / 360 * AD_2POW16 +0.5);

	// MSB first
	Pha_Amp_update[3] = Phase_word & 0xff;         // Phase
	Pha_Amp_update[2] = (Phase_word >> 8) & 0xff;
	Pha_Amp_update[1] = Amp_word & 0xff;           // Amplitude
	Pha_Amp_update[0] = (Amp_word >> 8) & 0x0f;     //Highest 4 bits open

	return Pha_Amp_update;
}

void AD9914_IO_LOW()
{
	HAL_GPIO_WritePin(AD9914_RESET_GPIO_Port, AD9914_RESET_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9914_IOUPDATE_GPIO_Port, AD9914_IOUPDATE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9914_SPI_CS_GPIO_Port, AD9914_SPI_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9914_SPI_CLK_GPIO_Port, AD9914_SPI_CLK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9914_SPI_MOSI_GPIO_Port, AD9914_SPI_MOSI_Pin, GPIO_PIN_RESET);
}

