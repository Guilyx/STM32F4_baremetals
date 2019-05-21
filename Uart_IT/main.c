/**
 * @project UART Test for STM32
 * @author Erwin Lejeune <erwin.lejeune15@gmail.com>
 * @date May, 2019
 * @copyright Copyright (C) MIT Licence.
 *
 * @brief STMicroelectronics STM32F4xx UART
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void MX_UART4_UART_Init();
UART_HandleTypeDef huart2;

uint8_t data[2000];
uint8_t count = 0;
uint8_t foo[] = "hello world\r\n";
uint8_t bar[] = "interrupted\r\n";
uint8_t buf[3];


void UART4_IRQHandler(void) {

	HAL_UART_IRQHandler(&huart2);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit(&huart2, bar, sizeof (bar), 500);
	HAL_UART_Receive_IT(&huart2, buf, 3);
}

int main(int argc, char* argv[])
{



	HAL_Init();

	HAL_NVIC_EnableIRQ(52);

    MX_UART4_UART_Init();

	/*for (int i=0; i<2000; i++)
	 {
	   data[i] = 'x';
	 }*/

	HAL_UART_Transmit(&huart2, foo, sizeof (foo), 500);
    HAL_UART_Receive_IT(&huart2, buf, 3);


	while(1);
}

static void MX_UART4_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = UART4;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    //printf("HUART Not ok");
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
