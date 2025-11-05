/*
 * SX1262_MX_Process.c
 *
 *  Created on: Jun 24, 2023
 *      Author: mingqiang
 */
#include "SX1262_MX_Process.h"

#define UART_BUFF_SIZE                              256
#define TIMEDIFF                                    5         // seconds
#define MAXTIMECOM                                  3         // max times of time calibration
#define MAXDATARXCOUNT                              3         // max times of data resending

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

/* wait for remote to be in RX, before sending a TX frame*/
#define RX_TIME_MARGIN                              50   //50ms

#define RX_TIMEOUT_VALUE                            1000 //1s
#define RX_CMD_WAIT_TIME                            3000 //3s
#define RX_DATA_WAIT_TIME                           120000// 120s

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

typedef enum
{
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
}Radio_States_t;

typedef enum
{
    WK,
    DATA_ST,
	TIME_CA,
	FID_TX,
	DATA_RESENT,
	DATA_DONE,
}TX_typte;

typedef struct
{
  uint8_t PktType;
  uint8_t Address;
}Packet_Structure;

typedef struct
{
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t weekday;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
}TimeDate;

#define BUFFER_SIZE                                 9
#define Band_Max_len                                1110

/*
 * Change the following to define sensor ID at each location
 */
Location_Info L1 = {
    .latitude = 39.18991451170659,
    .longitude = -96.58213927621752,
    .locationName = "North Farm",
    .sensorCount = 1,
    .sensorIDs = {8} //test stm32
};

uint8_t Last_freIDs_Pkt_len = 0;
uint16_t Fre_RX_count = 0;
uint16_t Frelen = 0;
uint16_t Fre_Length = 0;
uint8_t fre_IDs[Band_Max_len/3][6] = {0};
uint8_t DatatoUART[Band_Max_len][BUFFER_SIZE];

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

uint8_t CLI_Radio_State = 0x00;

Radio_States_t Radio_State;
TX_typte TX_cmd;
Packet_Structure TxPkt, RxPkt;
bool Current_Check;
bool RX_DATA;
bool TX_FreIDs;

int8_t RssiValue[Band_Max_len];
int8_t SnrValue[Band_Max_len];
uint16_t RX_num;

uint8_t UART_RXBuffer[UART_BUFF_SIZE];

TimeDate ctimedate;
TimeDate Rtimedate;
bool TimeCheck_status = false;
uint8_t TimeCheck_count = 0;

uint8_t DataResend_count = 0;
uint8_t FreIDResebd_count = 0;

uint8_t data_RXTX_Test[BUFFER_SIZE-2] ={23, 56, 122, 197, 10, 238, 5};

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

void Raido_Init( void )
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

	Radio.SetMaxPayloadLength( MODEM_LORA, BUFFER_SIZE );
}

/**
 * Main application entry point.
 */
void MX_Send_Receive( void )
{
	uint32_t rxStartTime;
	SX126xAntSwOn();
	UART_Start_Receive();
	UART_Sync_Check();
	FrequencyIDs_From_UART();
    while(1)
    {
    	BitIndices indices_result = findBitIndices(CLI_Radio_State, L1.sensorCount);

    	bool All_Done = (indices_result.zerosCount == 0); //L1.sensorCount

    	if(All_Done)
    	{
    		printf("#I#%d#AllDone#\n", L1.sensorCount);
       	    // Clear all sensor bits for data collection again.
       	    CLI_Radio_State &= 0x00;

       	    FrequencyIDs_From_UART();
       	    printf("#I#Collect again#\n");
    	}
    	else
    	{
    		for (uint8_t i = 0; i < L1.sensorCount; i++)
    		{
    			Radio_State = TX;
    			TX_cmd = WK;
    			Current_Check = false;

    			if (!(CLI_Radio_State & (1 << i)))
    			{
    				Wait_UART_Start(L1.sensorIDs[i]);

    				while(!Current_Check)
    				{
    					switch(Radio_State)
    					{
    					case TX:
    						if(TX_cmd == WK)
    						{
    							Get_RandnumtoRadio(CLI_WKUP);
    							TxPkt.PktType = CLI_WKUP;
    							RX_DATA = false;
    							printf("#I#%d#Waking up#\n", L1.sensorIDs[i]);
    						}
    						else if(TX_cmd == TIME_CA)
    						{
    							printf("#I#%d#Calibrating time#\n", L1.sensorIDs[i]);
    							Get_TimetoRadio(L1.sensorIDs[i], TIM_CAL);
    							TxPkt.PktType = TIM_CAL;
    							RX_DATA = false;
    						}
    						else if(TX_cmd == FID_TX)
    						{
    							printf("#I#%d#Sending FreIDs#\n", L1.sensorIDs[i]);
    							TxPkt.Address = L1.sensorIDs[i];

    							TX_FreIDs = true;
    							FreIDs_To_Radio();

    							// Send stop symbol
    							TX_FreIDs = false;
    							TxPkt.PktType = FID_STOP;
    							RX_DATA = false;
    						}
    						else if(TX_cmd == DATA_ST)
    						{
    							Get_RandnumtoRadio(ST_DATA);
    							TxPkt.PktType = ST_DATA;
    							RX_DATA = true;
    							RX_num = 0;
    							DataResend_count = 0;
    							printf("#I#%d#Collecting data#\n", L1.sensorIDs[i]);
    						}
    						else if(TX_cmd == DATA_RESENT)
    						{
    							TxPkt.PktType = RST_DATA;
    							RX_DATA = true;
    							RX_num = 0;
    							printf("#I#%d#Resending data#\n", L1.sensorIDs[i]);
    						}
    						else if(TX_cmd == DATA_DONE)
    						{
    							TxPkt.PktType = DONE_DATA;
    							RX_DATA = false;
    							// Set sensor flag bit to indicate the collection is complete for this sensor
    							CLI_Radio_State |= (1U << i);
    							Current_Check = true;
    							printf("#I#%d#NodeDone#\n", L1.sensorIDs[i]);
    						}

    					    TxPkt.Address = L1.sensorIDs[i];
    					    Buffer[0] = TxPkt.PktType;
    					    Buffer[1] = TxPkt.Address;

    					    DelayMs(RX_TIME_MARGIN);
    					    Radio.Send(Buffer, BUFFER_SIZE);
    					    // Wait for completing sending
    					    while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
    					    rxStartTime = HAL_GetTick();
    					    break;
    					case RX:
    						if (((HAL_GetTick() - rxStartTime) > RX_CMD_WAIT_TIME) && (!RX_DATA ))   // 3s timeout  for CMD
    						{
    							printf("#I#%d#CMD timeout#\n", L1.sensorIDs[i]);
    					        Current_Check = true;
    					    }
    						else if(((HAL_GetTick() - rxStartTime) > RX_DATA_WAIT_TIME) && (RX_DATA)) // 40s timeout for data
    						{
    							printf("#I#%d#DATA timeout#\n", L1.sensorIDs[i]);
    							Current_Check = true;
    						}
    					    break;
    					case RX_TIMEOUT:
    						Current_Check = true;
    					    break;
    					case TX_TIMEOUT:
    						Current_Check = true;
    					    break;
    					case RX_ERROR:
    						Current_Check = true;
    					    break;
    					default:
    					    break;
    					}
    					if(Radio.IrqProcess != NULL) Radio.IrqProcess();
    					if(Current_Check)
    					{
    						printf("#I#%d#NodeStop#\n", L1.sensorIDs[i]);
    						HAL_Delay(3000);    // wait for 3s and then start processing next node
    					}
    				}
    			}
    		}
    	}
    }
}

void OnTxDone( void )
{
	if(!TX_FreIDs)
	{
		Radio_State = RX;
		Radio.Rx( RX_TIMEOUT_VALUE );
	}

	//Clear buffer by setting all elements to 0
	memset(Buffer, 0, sizeof(Buffer));
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    BufferSize = size;

    memcpy( Buffer, payload, BufferSize );
    RxPkt.PktType = Buffer[0];
    RxPkt.Address = Buffer[1];

    //ACK received, check local time
    if(RxPkt.PktType == ACK_OK && RxPkt.Address == TxPkt.Address)
    {
    	printf("#I#%d#AKReceived#\n", RxPkt.Address);
    	Sensor_Node_Local_time();
    	Radio_State = TX;
    	TX_cmd = TIME_CA;
    	TimeCheck_count = 0;
    }
    else if(RxPkt.PktType == TIM_CK && RxPkt.Address == TxPkt.Address)
    {
    	Time_Calibration_Check();
    	// Check is time and date is correct.
    	if(!TimeCheck_status)
    	{
    		if(TimeCheck_count>=MAXTIMECOM)
    		{
    			printf("#I#%d#TimeCalFailed#%d#\n", RxPkt.Address, MAXTIMECOM);
    			Current_Check = true;
    		}
    		else
    		{
    			TX_cmd = TIME_CA;
    		}
    	}
    	else
    	{
    		TX_cmd = FID_TX;
    		FreIDResebd_count = 0;
    	}
    	Radio_State = TX;
    }
    else if(RxPkt.PktType == FID_OK && RxPkt.Address == TxPkt.Address)
    {
    	printf("#I#%d#FreIDsTXOK#\n", RxPkt.Address);
    	Sensor_Node_Local_time();
    	Radio_State = TX;
    	TX_cmd = DATA_ST;
    }
    else if(RxPkt.PktType == FID_RST && RxPkt.Address == TxPkt.Address)
    {
    	printf("#I#%d#FreIDsREST#\n", RxPkt.Address);
    	Sensor_Node_Local_time();
    	FreIDResebd_count++;
    	Radio_State = TX;
    	TX_cmd = FID_TX;

    	if(FreIDResebd_count>=MAXDATARXCOUNT)
    	{
    		FreIDResebd_count = 0;
    		Current_Check = true;
    		printf("#I#%d#FreIDsFailed#%d#\n", RxPkt.Address, MAXDATARXCOUNT);
    	}
    }
    else if(RxPkt.PktType == CLI_DATA && RxPkt.Address == TxPkt.Address)
    { // Data streaming starts
    	memcpy( DatatoUART[RX_num], payload, BufferSize );
    	RssiValue[RX_num] = rssi;
    	SnrValue[RX_num] = snr;
    	RX_num++;
    }
    else if(RxPkt.PktType == DATA_STOP && RxPkt.Address == TxPkt.Address)
    { // Data streaming stops
    	printf("#D#%d#Recdata#%d#\n", RxPkt.Address, RX_num);
    	Sensor_Node_Local_time();
    	Print_Radio_RX_Data();
    	if(Fre_Length != RX_num)
    	{ // Length of received data is incorrect, request to send data again
    		DataResend_count++;
    		printf("#D#%d#WrongLength#%d#\n", RxPkt.Address, Fre_Length);
    		TX_cmd = DATA_RESENT;
    	}
    	else
		{ // Correct length -> Parity check
    		if(!Check_ReceivedData_Error())
    		{ // No errors
    			printf("#D#%d#DataNoErrors#\n", RxPkt.Address);
    		    TX_cmd = DATA_DONE;
    		}
    		else  // Error found
    		{
    			DataResend_count++;
    		    printf("#D#%d#DataWithErrors#\n", RxPkt.Address);
    		    TX_cmd = DATA_RESENT;
    		}
		}
    	if(DataResend_count>=MAXDATARXCOUNT)
    	{
    		printf("#I#%d#ResendFailed#%d#\n", RxPkt.Address, MAXDATARXCOUNT);
    	    TX_cmd = DATA_DONE;
    	}
    	Radio_State = TX;
    }
    else
    {
    	printf("#I#Receive error: Node ID or CMD not matched.\n");
    	Current_Check = true;
    }
    memset(Buffer, 0, sizeof(Buffer));  //Clear buffer by setting all elements to 0
}

void OnTxTimeout( void )
{
    // Radio.Sleep( );
	printf("#I#Radio TX TIMEOUT.\n");
    Radio_State = TX_TIMEOUT;
}

void OnRxTimeout( void )
{
    // Radio.Sleep( );
	printf("#I#Radio RX TIMEOUT.\n");
    Radio_State = RX_TIMEOUT;
}

void OnRxError( void )
{
    // Radio.Sleep( );
	printf("#I#Radio RX ERROR.\n");
    Radio_State = RX_ERROR;
}

void Print_Radio_RX_Data( void )
{
	for (uint16_t i = 0; i < Fre_Length; i++)
	{
		uint16_t fre_index = DatatoUART[i][2] + (DatatoUART[i][3]<<8);
		uint16_t Mag = DatatoUART[i][4] + (DatatoUART[i][5]<<8);
		uint16_t Phs = DatatoUART[i][6] + (DatatoUART[i][7]<<8);
		printf("#D#%d#Savedata#%4d %4d %5.3f %4d %5.3f %3d %4d %4d#\n",
				RxPkt.Address, fre_index, Mag, Mag*3.3/4095, Phs, Phs*3.3/4095,
				DatatoUART[i][8], RssiValue[i], SnrValue[i]);
	}
}

uint8_t Calculate_Parity(uint8_t value)
{
   /*
    *  Return parity bit of 1 byte data
    *  Even parity: 1010: return 0, 1011: return 1
    *
    */
    uint8_t parity = 0;
    while (value)
    {
        parity ^= 1;
        value &= (value - 1);
    }
    return parity;
}

bool Check_ReceivedData_Error( void )
{
	/*
	 * Check if the received data parities match
	 * Return false if all matched;
	 * Stop checking and return true if one byte not matched.
	 *
	 */
    for (uint16_t j = 0; j < Fre_Length; j++)
    {
    	uint8_t parity_byte = 0;
    	for (uint8_t i = 0; i < 8; i++)
    	{
    		uint8_t parity_bit = Calculate_Parity(DatatoUART[j][i]);
    		parity_byte |= (parity_bit << i);
    	}
    	if (DatatoUART[j][8] != parity_byte) return true;
    	// printf("#I#%d#\n", parity_byte);
    }
    return false;
}

bool Check_All_Data_Collection_Done(uint8_t value, uint8_t n)
{
	// the maximum number of sensor is 8 (uint8_t)
	if ((n == 8) && (value == 0xff)) return true;

	uint8_t mask = (1U << n) - 1;

	// Apply the mask and compare
	return (value & mask) == mask;
}

// Function to find bit indices
BitIndices findBitIndices(uint8_t value, uint8_t n)
{
    BitIndices result = {{0}, {0}, 0, 0}; // Initialize result structure
    for (uint8_t i = 0; i < n; i++)
    {
        if (value & (1 << i))
        {
            // If bit i is set
            result.one_index[result.onesCount++] = i;
        }
        else
        {
            // If bit i is not set
            result.zero_index[result.zerosCount++] = i;
        }
    }
    return result;
}

void FreIDs_To_Radio( void )
{
	//Send start symbol
	Buffer[0] = FID_ST;
	Buffer[1] = TxPkt.Address;
	Buffer[2] = (Frelen >> 8) & 0xFF;
	Buffer[3] = Frelen & 0xFF;
	DelayMs(RX_TIME_MARGIN);
	Radio.Send(Buffer, BUFFER_SIZE);
	// Wait for completing sending
	while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
	if(Radio.IrqProcess != NULL) Radio.IrqProcess();

	// Send Frequency IDs
	for (uint16_t i=0; i<Fre_RX_count; i++)
	{
		Buffer[0] = FID_DA;
		Buffer[1] = TxPkt.Address;
		for (uint8_t j = 0; j < 6; j++)
		{
			Buffer[2 + j] = fre_IDs[i][j];
		}
		if (i == Fre_RX_count-1) Buffer[8] = (Frelen % 3 == 0) ? 3 : Frelen % 3;
		else Buffer[8] = 3;

		Radio.Send(Buffer, BUFFER_SIZE);
		// Wait for completing sending
		while((SX126xGetIrqStatus() & IRQ_TX_DONE ) != IRQ_TX_DONE);
		if(Radio.IrqProcess != NULL) Radio.IrqProcess();
	}
}

void Time_Calibration_Check( void )
{
	TimeCheck_status = false;
	TimeCheck_count++;

	Rtimedate.year = Buffer[2];
	Rtimedate.month = Buffer[3];
	Rtimedate.date = Buffer[4];
	Rtimedate.weekday = Buffer[5];
	Rtimedate.hour = Buffer[6];
	Rtimedate.minute = Buffer[7];
	Rtimedate.second = Buffer[8];

	if(ctimedate.year == Rtimedate.year &&
	   ctimedate.month == Rtimedate.month &&
	   ctimedate.date == Rtimedate.date &&
	   ctimedate.weekday == Rtimedate.weekday)
	{
		int c_seconds = ctimedate.hour * 3600 + ctimedate.minute * 60 + ctimedate.second;
		int R_seconds = Rtimedate.hour * 3600 + Rtimedate.minute * 60 + Rtimedate.second;
		int diff = abs(c_seconds - R_seconds);
		if(diff <= TIMEDIFF)
		{
			TimeCheck_status = true;
			printf("#I#%d#TimeMatched#Diff:%ds within %ds#\n",RxPkt.Address,diff,TIMEDIFF);
		}
		else
		{
			printf("#I#TimeNotMatched#Count:%dDiff:%ds#\n",TimeCheck_count,diff);
		}
	}
}

void Sensor_Node_Local_time( void )
{
	ctimedate.year = Buffer[2];
	ctimedate.month = Buffer[3];
	ctimedate.date = Buffer[4];
	ctimedate.weekday = Buffer[5];
	ctimedate.hour = Buffer[6];
	ctimedate.minute = Buffer[7];
	ctimedate.second = Buffer[8];
	printf("#I#TimeFMSensor:%02d/%02d/%02d %02d:%02d:%02d Weekday:%d#\n",
			ctimedate.month, ctimedate.date,ctimedate.year,
			ctimedate.hour, ctimedate.minute, ctimedate.second, ctimedate.weekday);
}

void UART_Sync_Check( void )
{
	bool sync_state = false;
	while(!sync_state)
	{
		if(Get_Received_Line(UART_RXBuffer))
		{
			if(strcmp((char *)UART_RXBuffer, "#?#") == 0)
			{
				uint8_t UART_TXBuffer[UART_BUFF_SIZE];
				// Start the formatted string with your custom protocol header
				int offset = sprintf((char *)UART_TXBuffer, "#!#Name,Lati,Long,IDs#%s_%.6f_%.6f_",
						              L1.locationName, L1.latitude, L1.longitude);
				// Append each sensor ID
				for (uint8_t i = 0; i < L1.sensorCount-1; i++)
				{
					offset += sprintf((char *)UART_TXBuffer + offset, "%d,", L1.sensorIDs[i]);
				}
				sprintf((char *)UART_TXBuffer + offset, "%d#\n",L1.sensorIDs[L1.sensorCount-1]);
	    		printf("%s", UART_TXBuffer);
	    		sync_state = true;
			}
		}
	}
}


void FrequencyIDs_From_UART( void )
{
	bool FreID_state = false;
	uint16_t Frelen_sum = 0;
	Fre_RX_count = 0;
	while(!FreID_state)
	{
		if(Get_Received_Line(UART_RXBuffer))
		{
			if(UART_RXBuffer[1] == 'F')
			{
				Frelen = (UART_RXBuffer[3]-'0')*1000 + (UART_RXBuffer[4]-'0')*100
						 +(UART_RXBuffer[5]-'0')*10  + (UART_RXBuffer[6]-'0');
			}
			else if (UART_RXBuffer[1] == 'C')
			{
				Frelen_sum += (UART_RXBuffer[3] - '0');

				for (uint8_t i = 0; i < 3; i++)
				{
					uint8_t baseIndex = 5 + 4 * i; // Calculate base index for each block of values
				    fre_IDs[Fre_RX_count][2*i] = UART_RXBuffer[baseIndex] - '0';
				    fre_IDs[Fre_RX_count][2*i+1] = ((UART_RXBuffer[baseIndex + 1] - '0') * 100 +
				                                    (UART_RXBuffer[baseIndex + 2] - '0') * 10 +
				                                    (UART_RXBuffer[baseIndex + 3] - '0'));
				}

				Fre_RX_count++;
			}
			else if (UART_RXBuffer[1] == 'P')
			{
				if(Frelen_sum == Frelen)
				{
					printf("#Y#\n");
					FreID_state = true;
				}
				else
				{
					printf("#I#FreLenNotMactched#%d#%d#\n", Frelen, Frelen_sum);
					Frelen = 0;
				    Frelen_sum = 0;
				    Fre_RX_count = 0;
				}
			}
		}
	}
	if(fre_IDs[0][1]=='a' && fre_IDs[0][3]=='l' && fre_IDs[0][5]=='f')
	{
		Fre_Length = Band_Max_len;
	}
	else
	{
		Fre_Length = Frelen;
	}
}


void DateTime_UART_To_Radio_Format( void )
{
	// year_month_date_weekday_hour_minute_second
	// First 3 bytes: #M#
	// data format:#M#xx_xx_xx_xx_xx_xx_xx#\n
	ctimedate.year = (UART_RXBuffer[3] - '0') * 10 + (UART_RXBuffer[4] - '0');
	ctimedate.month = (UART_RXBuffer[6] - '0') * 10 + (UART_RXBuffer[7] - '0');
	ctimedate.date = (UART_RXBuffer[9] - '0') * 10 + (UART_RXBuffer[10] - '0');
	ctimedate.weekday = (UART_RXBuffer[12] - '0') * 10 + (UART_RXBuffer[13] - '0');
	ctimedate.hour = (UART_RXBuffer[15] - '0') * 10 + (UART_RXBuffer[16] - '0');
	ctimedate.minute = (UART_RXBuffer[18] - '0') * 10 + (UART_RXBuffer[19] - '0');
	ctimedate.second = (UART_RXBuffer[21] - '0') * 10 + (UART_RXBuffer[22] - '0');

	Buffer[2] = ctimedate.year;
	Buffer[3] = ctimedate.month;
	Buffer[4] = ctimedate.date;
	Buffer[5] = ctimedate.weekday;
	Buffer[6] = ctimedate.hour;
	Buffer[7] = ctimedate.minute;
	Buffer[8] = ctimedate.second;

	printf("#I#TimeToSensor:%02d/%02d/%02d %02d:%02d:%02d Weekday:%d#\n",
			ctimedate.month, ctimedate.date, ctimedate.year,
			ctimedate.hour, ctimedate.minute, ctimedate.second, ctimedate.weekday);
}

void Radio_Test_Data_Format( void )
{
	// First 3 bytes: #N#
	// data format:#N#XXXXXXXXXXXXXXXXXXXXX#\n 3 characters for one random number
	for(uint8_t i=0; i<BUFFER_SIZE-2; i++)
	{
		data_RXTX_Test[i] = (UART_RXBuffer[3*i+3]-'0')*100+
				            (UART_RXBuffer[3*i+4]-'0')*10+
							(UART_RXBuffer[3*i+5]-'0');
		Buffer[i+2] = data_RXTX_Test[i];
	}

	printf("#I#RadioTestData:%d,%d,%d,%d,%d,%d,%d\n",
			data_RXTX_Test[0], data_RXTX_Test[1], data_RXTX_Test[2],
			data_RXTX_Test[3], data_RXTX_Test[4], data_RXTX_Test[5],
			data_RXTX_Test[6]);
}

void Get_RandnumtoRadio( uint8_t cmd )
{
	printf("#R#%d#%d#\n", BUFFER_SIZE-2, cmd);
	while(1)
	{
		// year_month_date_weekday_hour_minute_second
		// printf("Please enter date and time in the format: xx_xx_xx_xx_xx_xx_xx\r\n");
		if(Get_Received_Line(UART_RXBuffer)) // data format:#M#xx_xx_xx_xx_xx_xx_xx#\n
		{
			if(UART_RXBuffer[1] == 'N')
			{
				Radio_Test_Data_Format();
				break;
	    	}
		}
	}
}

void Get_TimetoRadio( uint8_t NodeID, uint8_t cmd)
{
	printf("#T#%d#%d#\n", NodeID, cmd);
	while(1)
	{
		if(Get_Received_Line(UART_RXBuffer)) // data format:#M#xx_xx_xx_xx_xx_xx_xx#\n
		{
			if(UART_RXBuffer[1] == 'M')
			{
				DateTime_UART_To_Radio_Format();
		    	break;
			}
		}
	}
}

void Wait_UART_Start( uint8_t NodeID )
{
	while(1)
	{
		if(Get_Received_Line(UART_RXBuffer))
		{
			if(UART_RXBuffer[1] == 'A')
			{
				printf("#S#%d#----------- Talking to sensor -----------#\n", NodeID);
				break;
			}
		}
	}
}
