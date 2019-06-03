/**
 * @project UART Interrupts to Led
 * @author Erwin Lejeune <erwin.lejeune15@gmail.com>
 * @date May, 2019
 * @copyright Copyright (C) MIT Licence.
 *
 * @brief STMicroelectronics STM32F4xx GPIO/UART
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_usart.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
UART_HandleTypeDef huart2;

GPIO_PinState state_but = GPIO_PIN_RESET;

uint8_t data[2000];
uint8_t count = 0;
uint8_t foo[] = "LED PA9 On\r\n";
uint8_t bar[] = "LED PA9 Off\r\n";
uint8_t foobar[] = "Hey, here we use the USART2 Interrupt to trigger a LED reaction !\r\n";
uint8_t barfoo[] = "Push the button to start simulation...\r\n";
uint8_t buf[3];


void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(&huart2);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(state_but == GPIO_PIN_RESET) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		state_but = GPIO_PIN_SET;
		HAL_UART_Transmit(&huart2, foo, sizeof (foo), 500);
	}

	else if(state_but == GPIO_PIN_SET){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		state_but = GPIO_PIN_RESET;
		HAL_UART_Transmit(&huart2, bar, sizeof (bar), 500);
	}

	HAL_UART_Receive_IT(&huart2, buf, 3);
}

int main(int argc, char* argv[])
{
	HAL_Init();

	HAL_NVIC_EnableIRQ(USART2_IRQn);

	MX_GPIO_Init();
    MX_USART2_UART_Init();

    HAL_UART_Transmit(&huart2, barfoo, sizeof (barfoo), 500);

    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {}

    state_but = GPIO_PIN_SET;

	HAL_UART_Transmit(&huart2, foobar, sizeof (foobar), 500);
    HAL_UART_Receive_IT(&huart2, buf, 3);

	while(1);
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

  /*Configure GPIO pin :
   *USART2 GPIO Configuration
   *PA2     ------> USART2_TX
   *PA3     ------> USART2_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
