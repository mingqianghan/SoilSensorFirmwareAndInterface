/*
 * board_usart.c
 *
 *  Created on: Jun 26, 2023
 *      Author: mingqiang
 */

#include "board_usart.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar( int ch )
#else
#define PUTCHAR_PROTOTYPE int fputc( int ch, FILE *f )
#endif /* __GNUC__ */


extern UART_HandleTypeDef huart2;

#define RX_BUFFER_SIZE 256

uint8_t Rx_Buffer[RX_BUFFER_SIZE];
volatile uint8_t Rx_Idx = 0;
volatile bool Line_Ready = false;
uint8_t Rx_char;

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit( &huart2, ( uint8_t * )&ch, 1, 0xFFFF );

  return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (Rx_char == '\n')
        {
            Rx_Buffer[Rx_Idx] = '\0'; // Null-terminate the string
            Line_Ready = true; // Signal that a complete line is ready
            Rx_Idx = 0; // Reset index for new data
        }
        else
        {
            if (Rx_Idx < (RX_BUFFER_SIZE - 1))
            {
            	Rx_Buffer[Rx_Idx++] = Rx_char;
            }
            else
            {
                // Handle buffer overflow
                memset(Rx_Buffer, 0, RX_BUFFER_SIZE);
                Rx_Idx = 0;
                Line_Ready = false;
            }
        }
        HAL_UART_Receive_IT(&huart2, &Rx_char, 1);
    }
}

void UART_Start_Receive()
{
    HAL_UART_Receive_IT(&huart2, &Rx_char, 1);
}

bool Get_Received_Line(uint8_t *buffer)
{
    if (Line_Ready) {
        strncpy((char*)buffer, (char*)Rx_Buffer, RX_BUFFER_SIZE);
        Line_Ready = false; // Reset the line ready flag after copying
        return true;
    }
    else
    {
    	return false;
    }
}

#ifdef USE_FULL_ASSERT
/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed( uint8_t* file, uint32_t line )
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif



