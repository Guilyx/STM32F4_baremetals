/**
 * @project Basic Timer Test
 * @author Erwin Lejeune <erwin.lejeune15@gmail.com>
 * @date May, 2019
 * @copyright Copyright (C) MIT Licence.
 *
 * @brief STMicroelectronics STM32F4xx TIM
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"
#include "stm32f4xx_hal_rcc.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void MX_UART4_UART_Init();
static void MX_TIM6_TIM_Init();

UART_HandleTypeDef huart4;
TIM_HandleTypeDef htim6;

int i = 0;
uint8_t datarx[10];
uint8_t welk[] = "Timer 6 Testing...\r\n";
uint8_t conf[] = "TIM6 Configured !\r\n";
uint8_t foo[] = "hello\r\n";
uint8_t bar[] = "world\r\n";
uint8_t on[] = "on\r\n";
uint8_t off[] = "off\r\n";

void TIM6_DAC_IRQHandler()
{
    HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (i == 0) {
    	HAL_UART_Transmit(&huart4, foo, sizeof(foo), 500);
    	i = 1;
    }
    else {
    	HAL_UART_Transmit(&huart4, bar, sizeof(bar), 500);
    	i = 0;
    }
}

int main(int argc, char* argv[])
{
  HAL_Init();

  MX_UART4_UART_Init();

  HAL_UART_Transmit(&huart4, welk, sizeof (welk), 500);

  HAL_Delay(500);

  MX_TIM6_TIM_Init();

  HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

  HAL_UART_Transmit(&huart4, conf, sizeof (conf), 500);

  HAL_TIM_Base_Start_IT(&htim6);

  while (1)
    {
	  int timerValue = __HAL_TIM_GET_COUNTER(&htim6);
	  if (timerValue == 500) {
		  HAL_UART_Transmit(&huart4, foo, sizeof (foo), 500);
	  }
	  if (timerValue == 2000) {
		  HAL_UART_Transmit(&huart4, bar, sizeof (bar), 500);
	  }
    }
}

void MX_TIM6_TIM_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim6.Instance = TIM6;
  __TIM6_CLK_ENABLE();
  htim6.Init.Prescaler = 16000 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2000;

  HAL_TIM_Base_Init(&htim6);
  HAL_TIM_Base_Start(&htim6);


  if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
	    //Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK) {
	    //Error_Handler();
   }


}

static void MX_UART4_UART_Init(void)
{
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    //printf("HUART Not ok");
  }
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
