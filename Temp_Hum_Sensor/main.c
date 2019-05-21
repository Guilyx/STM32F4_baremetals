/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_gpio.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

//void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);

uint8_t welk[] = "Welcome to Hiventive's first weather station !\r\n";
uint8_t TooHigh[] = "If either the temperature or the humidity are too high, the led will toggle.\r\n";
uint8_t ledon[] = "Temperature too high !!! ( > 30 )\r\n";
uint8_t ledoff[] = "Humidity too high !!! ( > 80 )\r\n";

uint8_t Temp[] = "New temperature measure... done.   : ";
uint8_t Hum[] =  "New humidity measure... done.      : ";

uint8_t backSpace[] = "\r\n";
uint8_t deg[] = " C";
uint8_t unhum[] = " %";

uint8_t tx_temp[4];
uint8_t tx_hum[4];

uint8_t tx[8];

void UART4_IRQHandler(void) {
	HAL_UART_IRQHandler(&huart2);
}


int main(int argc, char* argv[])
{

	HAL_Init();


	// ------------------------
	// Enable NVIC IRQ here

	/* The GPIO interrupt controller should be used
	 * in that code for the button, unfortunately it 
	 * isn't implemented correctly on the "virtual prototype" 
	 * I'm developing so I'm bound to use polling.
	 */

	// ------------------------

	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();

	HAL_UART_Transmit(&huart2, welk, sizeof (welk), 500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_UART_Receive(&huart2, tx, 5, 1000);
	HAL_UART_Transmit(&huart2, TooHigh, sizeof(TooHigh), 500);

	uint32_t humidity = 0;
	uint32_t tempC = 0;
	//float temperature = 0;

	char txTemp[50];
	char txHum[50];

	_Bool messageSent0 = 0;
	_Bool messageSent1 = 0;

	while (1) {

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
			humidity = si7021_measure_humidity(&hi2c1);
			tempC = si7021_measure_temperature(&hi2c1);

			sprintf(txTemp, "%u.%u", tempC/100, tempC - ((tempC/100)*100));
			int lenTemp = strlen(txTemp);

			sprintf(txHum, "%u", humidity);
			int lenHum = strlen(txHum);

			HAL_UART_Transmit(&huart2, Temp, sizeof(Temp), 500);
			HAL_UART_Transmit(&huart2, txTemp, lenTemp, 500);
			HAL_UART_Transmit(&huart2, deg, sizeof(deg), 500);
			HAL_UART_Transmit(&huart2, backSpace, sizeof(backSpace), 500);
			HAL_UART_Transmit(&huart2, Hum, sizeof(Hum), 500);
			HAL_UART_Transmit(&huart2, txHum, lenHum, 500);
			HAL_UART_Transmit(&huart2, unhum, sizeof(unhum), 500);
			HAL_UART_Transmit(&huart2, backSpace, sizeof(backSpace), 500);
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET){}
		}

		if (tempC > 3000) {
			if (messageSent0 == 0) {
				HAL_UART_Transmit(&huart2, ledon, sizeof(ledon), 500);
			}
			messageSent0 = 1;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			HAL_Delay(10000);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		} else {
			messageSent0 = 0;
		}

		if(humidity > 80){
			if (messageSent1 == 0) {
				HAL_UART_Transmit(&huart2, ledoff, sizeof(ledoff), 500);
			}
			messageSent1 = 1;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			HAL_Delay(10000);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		} else {
			messageSent1 = 0;
		}

		HAL_Delay(500);

	  }

	// -------------------------------- //


}

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0; //0x80 ?
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    // Error_Handler();
  }
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

  /* BUTTON | Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* LED | Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* I2C | Configure GPIO pin :
   * PB6     ------> I2C1_SCL
   * PB7     ------> I2C1_SDA
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* UART | Configure GPIO pin : PA2&PA3*/
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
