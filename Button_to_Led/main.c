/**
 * @project Button to Led (Polling)
 * @author Erwin Lejeune <erwin.lejeune15@gmail.com>
 * @date May, 2019
 * @copyright Copyright (C) MIT Licence.
 *
 * @brief STMicroelectronics STM32F4xx GPIO Test
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_gpio.h"

// ---------------------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

UART_HandleTypeDef huart2;

//uint8_t data[2000];
//uint8_t count = 0;
uint8_t welk[] = "Welcome to the blinky LED on STM32F4";
uint8_t foo[] = "led on\r\n";
uint8_t bar[] = "led off\r\n";
uint8_t buf[3];


void UART4_IRQHandler(void) {
	HAL_UART_IRQHandler(&huart2);
}

/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit(&huart2, bar, sizeof (bar), 500);
	HAL_UART_Receive_IT(&huart2, buf, 3);
}*/

int main(int argc, char* argv[])
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_Init();

	HAL_NVIC_EnableIRQ(6);
	HAL_NVIC_EnableIRQ(7);
	HAL_NVIC_EnableIRQ(8);
	HAL_NVIC_EnableIRQ(9);
	HAL_NVIC_EnableIRQ(10);
	HAL_NVIC_EnableIRQ(23);
	HAL_NVIC_EnableIRQ(40);
	HAL_NVIC_EnableIRQ(38);

	MX_GPIO_Init();
	MX_USART2_UART_Init();

	HAL_UART_Transmit(&huart2, welk, sizeof (welk), 500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

	GPIO_PinState state_but = GPIO_PIN_RESET;

	while (1) {
		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) && (state_but == GPIO_PIN_RESET)) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			state_but = GPIO_PIN_SET;
			HAL_UART_Transmit(&huart2, foo, sizeof (foo), 500);
		}

		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) && (state_but == GPIO_PIN_SET)){
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
			state_but = GPIO_PIN_RESET;
			HAL_UART_Transmit(&huart2, bar, sizeof (bar), 500);
		}
		HAL_Delay(500);
	  }

	// -------------------------------- //


}

static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  //__HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2&PA3*/
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
