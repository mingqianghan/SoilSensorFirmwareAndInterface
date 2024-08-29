#ifndef __AD9914_H
#define __AD9914_H


#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "Sensor_Config.h"

/* ====== Register Addresses ====== */
#define CFR1                 0x00
#define CFR2                 0x01
#define CFR3                 0x02
#define CFR4                 0x03

// Address for Frequency Tuning Word Register 0 (using profile 0)
#define FreqTWAddress0       0x0B
// Address for Phase Offset and Amplitude Register 0
#define PhaAmpAddress0       0x0C

//DDS clock frequency in MHz
#define AD9914_MCLK          2400

// Used in calculating frequency/phase/amplitude tuning words
#define AD_2POW32            pow(2,32)
#define AD_2POW16            (uint32_t)(1 << 16)
#define AD_2POW12            (uint16_t)(1 << 12)



// do nothing (little delay)
#define AD9833_ASM_NOP() asm("NOP")


void AD9914_Write_Register(uint8_t RegisterAdress, const uint8_t* Value);
void AD9914_Send_Reset();
void AD9914_Calibrate_DAC();
void AD9914_Initilize_DDS();
void AD9914_Send_IO_Update();
void AD9914_Generate_Wave(double fout);
void AD9914_Init();
void AD9914_Power_Down(bool _disable);
void AD9914_IO_LOW();
uint8_t* AD9914_Calculate_frequency_tuning_word(double fout);
uint8_t* AD9914_Phase_Amp_Set(float Amp_scale, float Phase);

#endif
