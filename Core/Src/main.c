/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "port_sys.h"
#include "485.h"
#include "pwm.h"
#include "systick.h"
#include "addr.h"
#include "lcd.h"
#include "motor.h"
#include "gwtask_timer.h"
#include "abmn.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
//I2C_HandleTypeDef hi2c1;

//UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//_Global xglobal;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static u32 UsedSeconds=0;
static void ReportRunInfo(void)
{
	
	if(!DebugLogBy485)
		return;
//	UsedSeconds++;
//	if(UsedSeconds<3)
//	{
//		return;
//	}
//	UsedSeconds=0;
	#ifdef MOTOR485
	char motorInfo1[64]={0};
	Motor_GetStatus(0,motorInfo1,63);
	PRINTF("+------------------------\r\n");
	PRINTF("|%s\r\n", motorInfo1);
	char motorInfo2[64]={0};
	Motor_GetStatus(1,motorInfo2,63);
	PRINTF("|%s\r\n", motorInfo2);
	PRINTF("+------------------------\r\n");
	#endif
	
}
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
    SysTick_Init();
	GWTaskTimer_Init();
    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    //MX_GPIO_Init();
    Addr_Init();
    Init485(Addr_Read(),baudrate_read());
	
    #ifdef MOTOR485
	Motor_Init();
    #endif
	#ifdef HDEP_GATHER
	ABMN_Init(localSubAddr);
	Lcd_Init();
	//LCD_Clear(BLACK);
	ABMN_ShowInfo();
	#endif
	
    //PRINTF("485ADDR:%X\r\n", localSubAddr);
    #ifdef MOTOR485
	//Send485("MOTOR485\r\n", 10);
//    char pwmmax[64];
//    sprintf(pwmmax, "MOTOR485,SystemCoreClock:%u", SystemCoreClock);
//    Send485(pwmmax, strlen(pwmmax));
    #endif
	
	GWTaskTimer_SetTimer(1000,false,ReportRunInfo);
	
	//Motor_StartForward(0,5);//,100);
	//Motor_StartBackward(0,5);
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
//	if (xglobal.rx_flag) {
//		xglobal.rx_flag = 0;
//		set_relay(xglobal.reg0);
//	}
        //Delay_ms(1000);
        //Send485(tx_buf,5);
		//char teststr[10]={0};
		//sprintf(teststr,"TEST:%d\r\n",testnum);
		//Send485(teststr, strlen(teststr));
		//PRINTF("TEST:%d\r\n", testnum);
		//testnum++;
		GWTaskTimer_Deal();
        Task_Deal485();
		#ifdef MOTOR485
		//Send485(pwmmax, strlen(pwmmax));
		//Motor_Deal();
		#endif
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
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler(ERRORCODE_SYSCONFIGCLOCK);
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler(ERRORCODE_SYSCONFIGCLOCK);
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;//RCC_PERIPHCLK_USART1|
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;

    //PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler(ERRORCODE_SYSCONFIGCLOCK);
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
//static void MX_GPIO_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//    /* GPIO Ports Clock Enable */
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    __HAL_RCC_GPIOB_CLK_ENABLE();

////  /*Configure GPIO pin Output Level */
////  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
////                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
////                          |GPIO_PIN_11, GPIO_PIN_SET);

////  /*Configure GPIO pin Output Level */
////  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

////  /*Configure GPIO pins : PA1 PA4 PA5 PA6
////                           PA7 PA8 PA9 PA10
////                           PA11 */
////  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
////                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
////                          |GPIO_PIN_11;
////  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
////  GPIO_InitStruct.Pull = GPIO_NOPULL;
////  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

////  /*Configure GPIO pin : PB0 */
////  GPIO_InitStruct.Pin = GPIO_PIN_0;
////  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
////  GPIO_InitStruct.Pull = GPIO_NOPULL;
////  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//    //HAL_GPIO_WritePin(_485EN_PORT, _485EN_IO_PIN, GPIO_PIN_SET);
//    GPIO_InitStruct.Pin = _485EN_IO_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(_485EN_PORT, &GPIO_InitStruct);
//    HAL_GPIO_WritePin(_485EN_PORT, _485EN_IO_PIN, GPIO_PIN_RESET);
//}

/* USER CODE BEGIN 4 */

//void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	//sprintf((char *)tx_buf, "slave recv %d\r\n", xglobal.rx_buf);
//	//uart_tx();
//	if (xglobal.reg0 != xglobal.rx_buf) {
//		xglobal.rx_flag = 1;
//		xglobal.reg0 = xglobal.rx_buf;
//	}
//	HAL_I2C_Slave_Receive_IT(&hi2c1, &xglobal.rx_buf, 1);
//}
//void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	HAL_I2C_Slave_Receive_IT(&hi2c1, &xglobal.rx_buf, 1);
//}

//void IO_reset()
//{
//  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
//                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
//                          |GPIO_PIN_11, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//}

//void set_relay(uint8_t reg0)
//{
//	uint8_t cs_x = 0;
//	uint8_t relay = 0;
//
//	IO_reset();

//	if (reg0 == 0) {
//		return;
//	}
//	reg0 = reg0 - 1;
//	cs_x = reg0 / 16;
//	relay = reg0 % 16;

//	for (int i = 0; i < 4; i++) {
//		uint8_t temp = (relay>>i) & 0x01;
//		switch (i) {
//			case 0:
//				if (temp) {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
//				} else {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
//				}
//				break;
//			case 1:
//				if (temp) {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
//				} else {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//				}
//				break;
//			case 2:
//				if (temp) {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
//				} else {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
//				}
//				break;
//			case 3:
//				if (temp) {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//				} else {
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//				}
//				break;
//		}
//	}

//	switch (cs_x) {
//		case 0:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//			break;
//		case 1:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
//			break;
//		case 2:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
//			break;
//		case 3:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//			break;
//		case 4:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
//			break;
//		case 5:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
//			break;
//		case 6:
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
//			break;
//		case 7:
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//			break;
//		default:
//			break;
//	}
//}
/* USER CODE END 4 */



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
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

