/*
 * Sensor_Driver.c
 *
 *  Created on: Jun 14, 2023
 *      Author: mingqiang
 */

#include "Sensor_Drivers.h"


extern SD_HandleTypeDef hsd;
extern ADC_HandleTypeDef hadc1;

// SD Card fatfs variables
FATFS    userFATFS;
FIL      userFILE;
UINT     userBytes;

// SD Card operation variables
char CurrentFileName[28];   // Current filename
char Data_To_SDCard[31];    // Data collected to save in SD Card

// ADC Variables
uint16_t ADC_Value[20];
uint32_t Mag, Phs;
volatile bool adcConversionComplete = false; // set by callback

// Radio Parameters
#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             22        // dBm

#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       9         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                            1000
#define RadioBufferLen                              9

/* wait for remote to be in Rx, before sending a Tx frame*/
#define RX_TIME_MARGIN                              50   //50ms
#define RX_CMD_WAIT_TIME                            3000 //3s

// Define Packet type
#define CLI_WKUP                                    0x01    // send to clients
#define ST_DATA                                     0x42
#define DONE_DATA                                   0x85
#define TIM_CAL                                     0x3e
#define RST_DATA                                    0xb3
#define FID_ST                                      0x13
#define FID_DA                                      0x25
#define FID_STOP                                    0x57
#define ACK_OK                                      0xc9    // receive from clients
#define TIM_CK                                      0xad
#define CLI_DATA                                    0xff
#define DATA_STOP                                   0x76
#define FID_OK                                      0xd4
#define FID_RST                                     0xe8

// Define Communication test data
#define UG_RX_Num                                   373
uint8_t UG_RX_Data[UG_RX_Num][RadioBufferLen] = {0};
int8_t UG_RssiValue[UG_RX_Num] = {0};
int8_t UG_SnrValue[UG_RX_Num] = {0};

#define Fre_size                                    1110
uint16_t Chosen_Bandlen = 0;
uint16_t FreID_cnt = 0;
uint16_t FreID_rec_cnt = 0;
uint16_t DatatoRadio[Fre_size*3];
uint16_t FreID[Fre_size] = {0};

typedef enum
{
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
}States_Radio;

typedef enum
{
    Shallow_LP,
	Deep_LP,
	RUN,
}States_MCU;

typedef enum
{
	CMD_ACK,
	TIME_ACK,
	Fre_Done,
	Fre_rqst,
	DATA_TX,
	DATA_RESEND,
}TX_typte;

typedef struct
{
  uint8_t PktType;
  uint8_t Address;
}Packet_Head;


uint16_t BufferSize = RadioBufferLen;
uint8_t  RadioBuffer[RadioBufferLen];

TX_typte TX_cmd;
States_Radio MX_State = RX;
States_MCU  MCU_State = Shallow_LP;
Packet_Head TxPkt, RxPkt;
bool Current_Check = false;

uint16_t TX_num = 0;
uint32_t rxStartTime;

bool Deep_LP_AlarmA;
extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;


void Sensor_MX_Process()
{
	MCU_Exit_Deep_Sleep();

	Radio_Init();

	SX126xAntSwOn();
	// Radio.Sleep();
	MCU_Enter_Shallow_Sleep();
	// Radio.Rx( RX_TIMEOUT_VALUE );
	while(1)
	{
		// Check MCU Power Mode
		if (MCU_State == Shallow_LP)
		{
			MCU_Exit_Shallow_Sleep();

			MCU_State = RUN;
			rxStartTime = HAL_GetTick();
		}
		else if(MCU_State == Deep_LP)
		{
			MCU_Enter_Deep_Sleep();
		}
		switch( MX_State )
		{
		case TX:
			if(TX_cmd == CMD_ACK)
			{
				TxPkt.PktType = ACK_OK;
				TxPkt.Address = SensorID;
				RadioBuffer[0] = TxPkt.PktType;
				RadioBuffer[1] = TxPkt.Address;

				Current_Time_TX_Format();

				DelayMs(RX_TIME_MARGIN);
				Radio.Send(RadioBuffer, RadioBufferLen);
				// Wait for completing sending
				while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
				if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
			}
			else if(TX_cmd == TIME_ACK)
			{
				TxPkt.PktType = TIM_CK;
				TxPkt.Address = SensorID;
				RadioBuffer[0] = TxPkt.PktType;
				RadioBuffer[1] = TxPkt.Address;

				Current_Time_TX_Format();

				DelayMs(RX_TIME_MARGIN);
				Radio.Send(RadioBuffer, RadioBufferLen);
				// Wait for completing sending
				while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
				if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
			}
			else if(TX_cmd == Fre_rqst)
			{
				TxPkt.PktType = FID_RST;
				TxPkt.Address = SensorID;
				RadioBuffer[0] = TxPkt.PktType;
				RadioBuffer[1] = TxPkt.Address;

				Current_Time_TX_Format();

				DelayMs(RX_TIME_MARGIN);
				Radio.Send(RadioBuffer, RadioBufferLen);
				// Wait for completing sending
				while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
				if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
			}
			else if(TX_cmd == Fre_Done)
			{
				TxPkt.PktType = FID_OK;
				TxPkt.Address = SensorID;
				RadioBuffer[0] = TxPkt.PktType;
				RadioBuffer[1] = TxPkt.Address;

				Current_Time_TX_Format();

				DelayMs(RX_TIME_MARGIN);
				Radio.Send(RadioBuffer, RadioBufferLen);
				// Wait for completing sending
				while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
				if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
			}
			else if(TX_cmd == DATA_TX)
			{
				Sensor_TX_Process();
			}
			else if(TX_cmd == DATA_RESEND)
			{
				Radio_TX(DatatoRadio, Chosen_Bandlen);
			}
			MX_State = RX;
			Radio.Rx( RX_TIMEOUT_VALUE );
			rxStartTime = HAL_GetTick();
		    break;
		case RX:
			if(Radio.IrqProcess != NULL) Radio.IrqProcess();
			if((HAL_GetTick() - rxStartTime) > RX_CMD_WAIT_TIME) MCU_Enter_Shallow_Sleep();// 3s timeout
		    break;
		case RX_TIMEOUT:
			MX_State = RX;
		    break;
		case TX_TIMEOUT:
		    MX_State = RX;
		    break;
		case RX_ERROR:
			MX_State = RX;
		    break;
		default:
		   break;
		  }
	}
}

void Sensor_TX_Process( void )
{
	HAL_GPIO_WritePin(GPIOE, POWER_EN1_Pin|POWER_EN2_Pin|POWER_EN3_Pin, GPIO_PIN_SET);    //Power On
	Get_FreIDs();
	if (SDCard_Mount())
	{
		// HAL_GPIO_WritePin(GPIOE, LED_RED_Pin, GPIO_PIN_SET);
		// Use current date time for SD card file name
		Get_Current_Time();
		sprintf(CurrentFileName, "s%03d_%02d%02d%02d_%02d%02d%02d.txt",
					 SensorID, sDate.Year, sDate.Month, sDate.Date,
					 sTime.Hours, sTime.Minutes, sTime.Seconds);

		f_open(&userFILE, CurrentFileName, FA_WRITE | FA_CREATE_ALWAYS);

		Sensor_Collect_Data(Allbands, Option1);

		f_close(&userFILE);

		// Save UG nodes RX data (packet,RSSI,SNR)
		Save_UG_RX_data();
	}

	// Data collection completion
	Shutoff_Data_Collection();

	Radio_TX(DatatoRadio, Chosen_Bandlen);

	// HAL_GPIO_WritePin(GPIOE, LED_RED_Pin, GPIO_PIN_RESET);
}

void Sensor_Collect_Data(Band_Selection Generating_band, Band_Selection Sending_band)
{
	uint16_t Arrayindex = 0;
	switch(Generating_band)
	{
	    case Allbands:
	    {
	    	float fout;

	    	AD9833_Init();
	    	DDS_Choose_Source(AD9833_LOWFREQ);         // generate signals with low frequencies

	    	for(uint16_t fre_index = 1; fre_index <= Fre_size; fre_index++)
	    	{
	    		// starting generating signals with high frequencies
	    		if(fre_index == 111)
	    		{
	    			AD9914_Init();
	    			DDS_Choose_Source(AD9914_HIGHFREQ);
	    		}

	    		if(fre_index <= 110)
	    		{
	    			//0.1kHz to 1kHz with step size of 0.1kHz (10 points)
	    			if(fre_index <= 10) fout = 0.1*fre_index ; // in kHZ
	    			//1kHz to 1MHz with log step size (100 points)
	    			else fout = pow(10, 0.03*(fre_index-10)); // in kHz

	    			AD9833_Generate_Wave(fout);
	    		}
	    		else
	    		{
	    			fout = pow(10, 0.003*(fre_index-110));  // in MHz
	    			AD9914_Generate_Wave(fout);
	    		}
	    		HAL_Delay(20);   //wait for generating signal

	    		Sensor_ADC_Process();

	    		sprintf(Data_To_SDCard, "%4d %11.6f %4lu %4lu\r\n", fre_index, fout, Mag, Phs);
	    		f_write(&userFILE, Data_To_SDCard, strlen(Data_To_SDCard), &userBytes);

	    		for(uint16_t i=0; i < Chosen_Bandlen; i++)
	    		{
	    			if(FreID[i] == fre_index)
	    			{
	    				DatatoRadio[3*Arrayindex] = fre_index;
	    				DatatoRadio[3*Arrayindex+1] = Mag;
	    				DatatoRadio[3*Arrayindex+2] = Phs;
	    				Arrayindex++;
	    			}
	    		}
	    	}
	    }
	    break;
	    case Option1:

	    break;
	    case Option2:

	    break;
	    case Option3:

	    break;
	}

}


void DDS_Choose_Source(DDS_type s_type)
{
	switch(s_type)
	{
	     case AD9833_LOWFREQ:
	     {
	    	 HAL_GPIO_WritePin(RF_SWA_GPIO_Port,RF_SWA_Pin,GPIO_PIN_RESET);
	    	 HAL_GPIO_WritePin(RF_SWB_GPIO_Port,RF_SWB_Pin,GPIO_PIN_SET);
	     }
	     break;
	     case AD9914_HIGHFREQ:
	     {
	    	 AD9833_Power_Down(true);
		     HAL_GPIO_WritePin(RF_SWA_GPIO_Port,RF_SWA_Pin,GPIO_PIN_SET);
		     HAL_GPIO_WritePin(RF_SWB_GPIO_Port,RF_SWB_Pin,GPIO_PIN_RESET);
	     }
	     break;
	}
}

void Shutoff_Data_Collection()
{
	AD9914_Power_Down(true);
	AD9914_IO_LOW();

	AD9833_Power_Down(true);
	AD9833_IO_LOW();

	LOWPOWER_GPIO_DeInit();
	// HAL_GPIO_WritePin(GPIOE, POWER_EN1_Pin|POWER_EN2_Pin|POWER_EN3_Pin, GPIO_PIN_RESET); //Power Off

	HAL_GPIO_WritePin(RF_SWA_GPIO_Port,RF_SWA_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RF_SWB_GPIO_Port,RF_SWB_Pin,GPIO_PIN_RESET);
}


bool SDCard_Mount()
{
	// Unlink first in case SD card is removed
	FATFS_UnLinkDriver(SDPath);
	MX_FATFS_Init();

	if (HAL_GPIO_ReadPin(uSD_Det_GPIO_Port, uSD_Det_Pin) != GPIO_PIN_RESET)
	{
		return false;    // SD Card does not exist
	}

	// mount sd card using 1-bit mode (There are bugs of the code generated by CUBEIDE using 4-bit mode)
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;


	if (f_mount(&userFATFS, SDPath, 1) != FR_OK)
	{
		return false;    // Mounting SD Card fails
	}

	// configure back to 4-bit mode for reading/ writing data
	HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);

	return true;
}


void Sensor_ADC_Data_Average()
{
	uint8_t i;
	for (i=0, Mag=0, Phs=0; i<20;)
	{
		Mag += ADC_Value[i++];
		Phs += ADC_Value[i++];
	}
	Mag /= 10;
	Phs /= 10;
}


void Sensor_ADC_Process()
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 20);   // Start ADC
	while(!adcConversionComplete);                          // Wait ADC to complete
	Sensor_ADC_Data_Average();                              // ADC Data
	adcConversionComplete = false;                          // Reset flag
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	adcConversionComplete = true;          // ADC Conversion Callback: Set the flag to start collect data
}


void Radio_Init( void )
{
	// Radio initialization
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;

	Radio.Init( &RadioEvents );

	Radio.SetChannel( RF_FREQUENCY );

	Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
	                               LORA_SPREADING_FACTOR, LORA_CODINGRATE,
	                               LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	                               true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
	                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
	                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
	                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

	Radio.SetMaxPayloadLength( MODEM_LORA, RadioBufferLen );

	Radio.Rx( RX_TIMEOUT_VALUE );
}

void Radio_TX( uint16_t *buffer, uint16_t Bandlen )
{
	uint16_t i = 0;

	// Data streaming
    TxPkt.PktType = CLI_DATA;
    TxPkt.Address = SensorID;

    while(i < Bandlen)
    {
    	RadioBuffer[0] = TxPkt.PktType;
    	RadioBuffer[1] = TxPkt.Address;

    	// Frequency Index
    	RadioBuffer[2] = (uint8_t)(buffer[3*i] & 0xFF);       // Lower 8 bits
    	RadioBuffer[3] = (uint8_t)((buffer[3*i] >> 8) & 0xFF); // Upper 8 bits

    	// Phase Data
    	RadioBuffer[4] = (uint8_t)(buffer[3*i+1] & 0xFF);       // Lower 8 bits
    	RadioBuffer[5] = (uint8_t)((buffer[3*i+1] >> 8) & 0xFF); // Upper 8 bits

    	//Magnitude Data
    	RadioBuffer[6] = (uint8_t)(buffer[3*i+2] & 0xFF);       // Lower 8 bits
    	RadioBuffer[7] = (uint8_t)((buffer[3*i+2] >> 8) & 0xFF); // Upper 8 bits

    	//Parity for RadioBuffer[2]-[7]
    	RadioBuffer[8] = combine_parities(RadioBuffer);

    	DelayMs(10);
    	Radio.Send(RadioBuffer, RadioBufferLen);
    	// Wait for completing sending
    	while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
    	if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
    	i++;
    }

    // Data streaming stop command
    TxPkt.PktType = DATA_STOP;
    TxPkt.Address = SensorID;
    RadioBuffer[0] = TxPkt.PktType;
    RadioBuffer[1] = TxPkt.Address;
    Current_Time_TX_Format();

    Radio.Send(RadioBuffer, RadioBufferLen);
    while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
    if(Radio.IrqProcess != NULL) Radio.IrqProcess( );
}


uint8_t calculate_parity(uint8_t value)
{
    uint8_t parity = 0;
    while (value)
    {
        parity ^= 1;
        value &= (value - 1);
    }
    return parity;
}


uint8_t combine_parities(uint8_t *values)
{
    uint8_t parity_byte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t parity_bit = calculate_parity(values[i]);
        parity_byte |= (parity_bit << i);
    }
    return parity_byte;
}


void OnTxDone( void )
{
	memset(RadioBuffer, 0, sizeof(RadioBuffer)); // Clear buffer by setting all elements to 0
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	BufferSize = size;
	memcpy( RadioBuffer, payload, BufferSize );
	RxPkt.PktType = RadioBuffer[0];
	RxPkt.Address = RadioBuffer[1];

	if(RxPkt.Address == SensorID) // Talking to correct sensor
	{
		if(RxPkt.PktType == CLI_WKUP)
		{
		    MX_State = TX;
		    TX_cmd = CMD_ACK;

		    // Save wireless communication data
		    memcpy( UG_RX_Data[UG_RX_Num-3], payload, BufferSize );
		    UG_RssiValue[UG_RX_Num-3] = rssi;
		    UG_SnrValue[UG_RX_Num-3] = snr;
		}
		else if(RxPkt.PktType == TIM_CAL)
		{
			Set_RTC_Time();    // Calibrate real time
			MX_State = TX;
			TX_cmd = TIME_ACK;
			memcpy( UG_RX_Data[UG_RX_Num-2], payload, BufferSize );
			UG_RssiValue[UG_RX_Num-2] = rssi;
			UG_SnrValue[UG_RX_Num-2] = snr;
		}
		else if(RxPkt.PktType == FID_ST)
		{

			Chosen_Bandlen = (RadioBuffer[2] << 8) + RadioBuffer[3];
			FreID_cnt = 0;
			FreID_rec_cnt = 0;
		}
		else if(RxPkt.PktType == FID_DA)
		{
			if(FreID_rec_cnt < UG_RX_Num)
			{
				memcpy( UG_RX_Data[FreID_rec_cnt], payload, BufferSize );
				UG_RssiValue[FreID_rec_cnt] = rssi;
				UG_SnrValue[FreID_rec_cnt] = snr;
				FreID_rec_cnt++;
			}
			FreID_cnt += RadioBuffer[8];
		}
		else if(RxPkt.PktType == FID_STOP)
		{
			if(FreID_cnt == Chosen_Bandlen)
			{
				MX_State = TX;
				TX_cmd = Fre_Done;
			}
			else
			{
				MX_State = TX;
				TX_cmd = Fre_rqst;
			}
		}
		else if(RxPkt.PktType == ST_DATA )
		{
			MX_State = TX;
		    TX_cmd = DATA_TX;

		    // Save wireless communication data
		    memcpy( UG_RX_Data[UG_RX_Num-1], payload, BufferSize );
		    UG_RssiValue[UG_RX_Num-1] = rssi;
		    UG_SnrValue[UG_RX_Num-1] = snr;
		}
		else if (RxPkt.PktType == RST_DATA)
		{
			MX_State = TX;
			TX_cmd = DATA_RESEND;
		}
		else if(RxPkt.PktType == DONE_DATA )
		{
			MCU_Enter_Shallow_Sleep();
		}
		// Other conditions
		// Received wrong packet header
		// Entering Low Power modes if no incoming packets in 3s
	}

	// the other condition: ID not matched
	// Enter Low Power Mode if no incoming packets in 3s.

    memset(RadioBuffer, 0, sizeof(RadioBuffer)); // Clear buffer by setting all elements to 0
}

void OnTxTimeout( void )
{
    MX_State = TX_TIMEOUT;
}

void OnRxTimeout( void )
{
    MX_State = RX_TIMEOUT;
}

void OnRxError( void )
{
    MX_State = RX_ERROR;
}

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
	RadioOnDioIrq();
}

void Get_FreIDs( void )
{
	if(UG_RX_Data[0][3] == 'a' &&
	   UG_RX_Data[0][5] == 'l' &&
	   UG_RX_Data[0][7] == 'f')
	{
		for(uint16_t i=0; i<Fre_size; i++) FreID[i] = i+1;
		Chosen_Bandlen = Fre_size;
	}
	else
	{
		for(uint16_t i=0; i<FreID_rec_cnt; i++)
		{
			FreID[3*i] = (UG_RX_Data[i][2] << 8) + UG_RX_Data[i][3];
			FreID[3*i+1] = (UG_RX_Data[i][4] << 8) + UG_RX_Data[i][5];
			FreID[3*i+2] = (UG_RX_Data[i][6] << 8) + UG_RX_Data[i][7];
		}
	}
}

void Current_Time_TX_Format( void )
{
	Get_Current_Time();

	RadioBuffer[2] = sDate.Year;
	RadioBuffer[3] = sDate.Month;
	RadioBuffer[4] = sDate.Date;
	RadioBuffer[5] = sDate.WeekDay;
	RadioBuffer[6] = sTime.Hours;
	RadioBuffer[7] = sTime.Minutes;
	RadioBuffer[8] = sTime.Seconds;
}

void Save_UG_RX_data( void )
{
	Get_Current_Time();
	sprintf(CurrentFileName, "w%03d_%02d%02d%02d_%02d%02d%02d.txt",
			SensorID, sDate.Year, sDate.Month, sDate.Date,
			sTime.Hours, sTime.Minutes, sTime.Seconds);

	f_open(&userFILE, CurrentFileName, FA_WRITE | FA_CREATE_ALWAYS);

	char outputform[50];

	for(uint16_t i=0; i<FreID_rec_cnt; i++)
	{

		sprintf(outputform, "%3d %3d %3d %3d %3d %3d %3d %3d %3d %4d %4d\r\n",
				UG_RX_Data[i][0], UG_RX_Data[i][1], UG_RX_Data[i][2],
				UG_RX_Data[i][3], UG_RX_Data[i][4], UG_RX_Data[i][5],
				UG_RX_Data[i][6], UG_RX_Data[i][7], UG_RX_Data[i][8],
				UG_RssiValue[i], UG_SnrValue[i]);

		f_write(&userFILE, outputform, strlen(outputform), &userBytes);

	}
	for(uint16_t i=UG_RX_Num-3; i<UG_RX_Num; i++)
	{
		sprintf(outputform, "%3d %3d %3d %3d %3d %3d %3d %3d %3d %4d %4d\r\n",
				UG_RX_Data[i][0], UG_RX_Data[i][1], UG_RX_Data[i][2],
				UG_RX_Data[i][3], UG_RX_Data[i][4], UG_RX_Data[i][5],
				UG_RX_Data[i][6], UG_RX_Data[i][7], UG_RX_Data[i][8],
				UG_RssiValue[i], UG_SnrValue[i]);
		f_write(&userFILE, outputform, strlen(outputform), &userBytes);
	}
	f_close(&userFILE);
}

void MCU_Exit_Deep_Sleep( void )
{
	// Check if the MCU wakes up from Standby mode
	// The SB flag is set
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);   // Clear the flag

	  	// Deactivate the RTC wakeup
	  	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	  	Time_Track( true );
	}
}

void RTC_Time_Setup( void )
{
	if((HAL_RTCEx_BKUPRead(&hrtc, FLAG_ADDRESS) != FLAG_VALUE))
	{
		//Initialize RTC and set the Time and Date
		sTime.Hours = userHour;
		sTime.Minutes = userMinute;
		sTime.Seconds = userSecond;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_SUB1H;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

		sDate.WeekDay = userWeekday;
		sDate.Month = userMonth;
		sDate.Date = userDate;
		sDate.Year = userYear;
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		// Writes a data in a RTC Backup data Register 1
		HAL_RTCEx_BKUPWrite(&hrtc, FLAG_ADDRESS, FLAG_VALUE);
	}
}


void Alarms_Setup( void )
{
	RTC_AlarmTypeDef sAlarm = {0};

	sAlarm.AlarmTime.Hours =  DSleepHourAm;
	sAlarm.AlarmTime.Minutes = 59;
	sAlarm.AlarmTime.Seconds = 30;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_SUB1H;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	// Enable Alarm B
	sAlarm.AlarmTime.Hours = DSleepHourPm;
	sAlarm.AlarmTime.Minutes = 59;
	sAlarm.AlarmTime.Seconds = 30;
	sAlarm.Alarm = RTC_ALARM_B;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}


void Set_RTC_Time( void )
{
	/* Received time and date info in RadioBuffer[2-7]
	 * RadioBuffer[2] <-> year      RadioBuffer[3] <-> month
	 * RadioBuffer[4] <-> date      RadioBuffer[4] <-> weekday
	 * RadioBuffer[5] <-> hours     RadioBuffer[6] <-> minutes
	 * RadioBuffer[7] <-> seconds
	 */

	sDate.Year = RadioBuffer[2];
	sDate.Month = RadioBuffer[3];
	sDate.Date = RadioBuffer[4];
	sDate.WeekDay = RadioBuffer[5];

	sTime.Hours = RadioBuffer[6];
	sTime.Minutes = RadioBuffer[7];
	sTime.Seconds = RadioBuffer[8];
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_SUB1H;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;


	// Make sure a valid date and time
	if(IS_RTC_WEEKDAY(sDate.WeekDay) && IS_RTC_YEAR(sDate.Year) &&
	   IS_RTC_MONTH(sDate.Month)     && IS_RTC_DATE(sDate.Date) &&
	   IS_RTC_HOUR24(sTime.Hours)    && IS_RTC_MINUTES(sTime.Minutes) &&
	   IS_RTC_SECONDS(sTime.Seconds))
	{
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		// Writes a data in a RTC Backup data Register 1
		HAL_RTCEx_BKUPWrite(&hrtc, FLAG_ADDRESS, FLAG_VALUE);
	}
}


void Get_Current_Time( void )
{
	// Get the RTC current time
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	// Get the RTC current Date
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void Time_Track( bool Exit_LP )
{
	char Str_buff[64];

	Get_Current_Time();

	if(Exit_LP)
	{
		sprintf(Str_buff, "Node %3d Waking up  (%02d/%02d/%02d %02d:%02d:%02d)\r\n",
				SensorID, sDate.Month, sDate.Date, sDate.Year,
				sTime.Hours, sTime.Minutes, sTime.Seconds);
	}
	else
	{
		sprintf(Str_buff, "Node %3d Etring DLP (%02d/%02d/%02d %02d:%02d:%02d)\r\n",
				SensorID, sDate.Month, sDate.Date, sDate.Year,
				sTime.Hours, sTime.Minutes, sTime.Seconds);

	}

	HAL_GPIO_WritePin(GPIOE, POWER_EN1_Pin, GPIO_PIN_SET);    //3.3V On
	HAL_Delay(20);   //wait for a stable power

	if (SDCard_Mount())
	{

		f_open(&userFILE, "logger.txt", FA_OPEN_APPEND | FA_WRITE);
	    f_write(&userFILE, Str_buff, strlen(Str_buff), &userBytes);
	    f_close(&userFILE);
	}
}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *myhrtc)
{
	// Resume systick
	// Reconfigure system clock
	if (MCU_State == Shallow_LP)
	{
		MCU_Exit_Shallow_Sleep();
		rxStartTime = HAL_GetTick();
	}

	// Check if Alarm A triggered the callback
	if (__HAL_RTC_ALARM_GET_FLAG(myhrtc, RTC_FLAG_ALRAF))
	{
		__HAL_RTC_ALARM_CLEAR_FLAG(myhrtc, RTC_FLAG_ALRAF); // Clear the Alarm A flag
		// Handle Alarm A event
		Deep_LP_AlarmA = true;
	}
	// Deep sleep mode
	MCU_State = Deep_LP;
}


void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *myhrtc)
{
	// Resume systick
	// Reconfigure system clock
	if (MCU_State == Shallow_LP)
	{
		MCU_Exit_Shallow_Sleep();
		rxStartTime = HAL_GetTick();
	}

	// Check if Alarm B triggered the callback
	if (__HAL_RTC_ALARM_GET_FLAG(myhrtc, RTC_FLAG_ALRBF))
	{
		__HAL_RTC_ALARM_CLEAR_FLAG(myhrtc, RTC_FLAG_ALRBF); // Clear the Alarm B flag
		// Handle Alarm B event
		Deep_LP_AlarmA = false;
	}
	// Deep sleep mode
	MCU_State = Deep_LP;
}

void MCU_Enter_Shallow_Sleep( void )
{
	// Enter STOP mode
	MX_State = RX;
	Radio.Rx( RX_TIMEOUT_VALUE );

	MCU_State = Shallow_LP;

	LOWPOWER_GPIO_DeInit();

	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void MCU_Exit_Shallow_Sleep( void )
{
	// Exit STOP Mode
	SystemClock_Config();
	HAL_ResumeTick();
	LOWPOWER_GPIO_Init();
}

void MCU_Enter_Deep_Sleep( void )
{
	// MCU enters Standby Mode
	// Radio enters SLEEP Mode before MCU enters Standby mode
	SX126xAntSwOff();
	Radio.Sleep();

	Time_Track(false);

	// clear the WU flag
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	// clear the RTC Wake UP flag
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

	// Enable the WakeUp
	if(Deep_LP_AlarmA)
	{
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, DsleepDurAm*60*60, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	}
	else
	{
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, DsleepDurPm*60*60, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	}

	// Finally enter the standby mode
	HAL_PWR_EnterSTANDBYMode();
}
