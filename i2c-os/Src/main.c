/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_tsensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;

osThreadId buttonInterruptHandle;
osThreadId readSensorsHandle;
osThreadId TransmitReadingHandle;
/* USER CODE BEGIN PV */
int8_t sensor_count = 0;

int16_t ACCELERO_XYZ[3];
int16_t MAGNETO_XYZ[3];
float PRESSURE_VALUE;
float TEMP_VALUE;

char ACCELERO_BUFFER[100];
char MAGNETO_BUFFER[100];
char PRESSURE_BUFFER[100];
char TEMP_BUFFER[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
void StartButtonInterrupt(void const * argument);
void StartReadSensors(void const * argument);
void StartTransmitReading(void const * argument);

/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize I2C sensors */
  BSP_ACCELERO_Init();
  BSP_MAGNETO_Init();
  BSP_PSENSOR_Init();
  BSP_TSENSOR_Init();

  /* Set low power mode for accelerometer and magnetometer */
  BSP_ACCELERO_LowPower(1);
  BSP_MAGNETO_LowPower(1);

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of buttonInterrupt */
  osThreadDef(buttonInterrupt, StartButtonInterrupt, osPriorityNormal, 0, 128);
  buttonInterruptHandle = osThreadCreate(osThread(buttonInterrupt), NULL);

  /* definition and creation of readSensors */
  osThreadDef(readSensors, StartReadSensors, osPriorityNormal, 0, 128);
  readSensorsHandle = osThreadCreate(osThread(readSensors), NULL);

  /* definition and creation of TransmitReading */
  osThreadDef(TransmitReading, StartTransmitReading, osPriorityNormal, 0, 128);
  TransmitReadingHandle = osThreadCreate(osThread(TransmitReading), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // read from the sensor
	  BSP_ACCELERO_AccGetXYZ(ACCELERO_XYZ);
	  BSP_MAGNETO_GetXYZ(MAGNETO_XYZ);
	  PRESSURE_VALUE = BSP_PSENSOR_ReadPressure();
	  TEMP_VALUE = BSP_TSENSOR_ReadTemp();

	  // add the values to the buffers
	  sprintf(ACCELERO_BUFFER, "\nAccelerometer values:\nX: %d\nY: %d\nZ: %d\n", ACCELERO_XYZ[0], ACCELERO_XYZ[1], ACCELERO_XYZ[2]);
	  sprintf(MAGNETO_BUFFER, "\nMagneto values:\nX: %d\nY: %d\nZ: %d\n", MAGNETO_XYZ[0], MAGNETO_XYZ[1], MAGNETO_XYZ[2]);
	  sprintf(PRESSURE_BUFFER, "\nPressure reading: %d\n", (int) PRESSURE_VALUE);
	  sprintf(TEMP_BUFFER, "\nTemperature reading: %d\n", (int) TEMP_VALUE);

	  // transmit buffer over USART1
	  HAL_UART_Transmit(&huart1, ACCELERO_BUFFER, 100, 30000);
	  HAL_UART_Transmit(&huart1, MAGNETO_BUFFER, 100, 30000);
	  HAL_UART_Transmit(&huart1, PRESSURE_BUFFER, 100, 30000);
	  HAL_UART_Transmit(&huart1, TEMP_BUFFER, 100, 30000);

	  // Delay to achieve 10 Hz sensor reading rate
	  HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10909CEC;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ErrorLED_GPIO_Port, ErrorLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FineLED_GPIO_Port, FineLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ErrorLED_Pin */
  GPIO_InitStruct.Pin = ErrorLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ErrorLED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FineLED_Pin */
  GPIO_InitStruct.Pin = FineLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FineLED_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	if (GPIO_Pin == BUTTON_Pin) {
//		sensor_count++;
//		if (sensor_count > 3) {
//			sensor_count = 0;
//		}
//		if (sensor_count == 0) {
//			sprintf(ACCELERO_BUFFER, "\nAccelerometer values:\nX: %d\nY: %d\nZ: %d\n", ACCELERO_XYZ[0], ACCELERO_XYZ[1], ACCELERO_XYZ[2]);
//			HAL_UART_Transmit(&huart1, ACCELERO_BUFFER, 100, 30000);
//		}
//		else if (sensor_count == 1) {
//			sprintf(MAGNETO_BUFFER, "\nMagneto values:\nX: %d\nY: %d\nZ: %d\n", MAGNETO_XYZ[0], MAGNETO_XYZ[1], MAGNETO_XYZ[2]);
//			HAL_UART_Transmit(&huart1, MAGNETO_BUFFER, 100, 30000);
//		}
//		else if (sensor_count == 2) {
//			sprintf(PRESSURE_BUFFER, "\nPressure reading: %d\n", (int) PRESSURE_VALUE);
//			HAL_UART_Transmit(&huart1, PRESSURE_BUFFER, 100, 30000);
//		}
//		else {
//			sprintf(TEMP_BUFFER, "\nTemperature reading: %d\n", (int) TEMP_VALUE);
//			HAL_UART_Transmit(&huart1, TEMP_BUFFER, 100, 30000);
//		}
//	}
//}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartButtonInterrupt */
/**
  * @brief  Function implementing the buttonInterrupt thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartButtonInterrupt */
void StartButtonInterrupt(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;) {
	  // Poll to check if the button is pressed
	  osDelay(200);
	  if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET) {
		  sensor_count++;
		  if (sensor_count == 4) {
			  sensor_count = 0;
		  }
	  }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartReadSensors */
/**
* @brief Function implementing the readSensors thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReadSensors */
void StartReadSensors(void const * argument)
{
  /* USER CODE BEGIN StartReadSensors */
  /* Infinite loop */
  for(;;) {
	  osDelay(50);
	  if (sensor_count == 0) {
		  BSP_ACCELERO_AccGetXYZ(ACCELERO_XYZ);
	  }
	  else if (sensor_count == 1) {
		  BSP_MAGNETO_GetXYZ(MAGNETO_XYZ);
	  }
	  else if (sensor_count == 2) {
		  PRESSURE_VALUE = BSP_PSENSOR_ReadPressure();
	  }
	  else if (sensor_count == 3) {
		  TEMP_VALUE = BSP_TSENSOR_ReadTemp();
	  }
  }
  /* USER CODE END StartReadSensors */
}

/* USER CODE BEGIN Header_StartTransmitReading */
/**
* @brief Function implementing the TransmitReading thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTransmitReading */
void StartTransmitReading(void const * argument)
{
  /* USER CODE BEGIN StartTransmitReading */
  /* Infinite loop */
  for(;;) {
	  osDelay(50);
	  if (sensor_count == 0) {
		  sprintf(ACCELERO_BUFFER, "\nAccelerometer values:\nX: %d\nY: %d\nZ: %d\n", ACCELERO_XYZ[0], ACCELERO_XYZ[1], ACCELERO_XYZ[2]);
		  HAL_UART_Transmit(&huart1, ACCELERO_BUFFER, 100, 30000);
	  }
	  else if (sensor_count == 1) {
		  sprintf(MAGNETO_BUFFER, "\nMagneto values:\nX: %d\nY: %d\nZ: %d\n", MAGNETO_XYZ[0], MAGNETO_XYZ[1], MAGNETO_XYZ[2]);
		  HAL_UART_Transmit(&huart1, MAGNETO_BUFFER, 100, 30000);
	  }
	  else if (sensor_count == 2) {
		  sprintf(PRESSURE_BUFFER, "\nPressure reading: %d\n", (int) PRESSURE_VALUE);
		  HAL_UART_Transmit(&huart1, PRESSURE_BUFFER, 100, 30000);
	  }
	  else if (sensor_count == 3) {
		  sprintf(TEMP_BUFFER, "\nTemperature reading: %d\n", (int) TEMP_VALUE);
		  HAL_UART_Transmit(&huart1, TEMP_BUFFER, 100, 30000);
	  }
  }
  /* USER CODE END StartTransmitReading */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
