/*
 * board_spi.c
 *
 *  Created on: Jun 24, 2023
 *      Author: mingqiang
 */

#include "board_spi.h"

uint8_t Sx126x_SpiInOut(uint8_t data)
{
	uint8_t i, rdata = 0x00;
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80) Sx126x_MOSI(SET);
		else Sx126x_MOSI(RESET);

		data <<= 1;
		rdata <<= 1;

		Sx126x_SCK(SET);
		if (Sx126x_MISO()) rdata |= 0x01;
		Sx126x_SCK(RESET);
	}
	Sx126x_MOSI(SET);
	return (rdata);
}
