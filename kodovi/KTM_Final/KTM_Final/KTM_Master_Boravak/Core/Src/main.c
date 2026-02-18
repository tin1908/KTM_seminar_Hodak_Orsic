/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_i2c.h"
#include <stdio.h>
#include <math.h>
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
PUTCHAR_PROTOTYPE {
HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
return ch;
}
GETCHAR_PROTOTYPE {
uint8_t ch = 0;
__HAL_UART_CLEAR_OREFLAG(&huart2);
HAL_UART_Receive(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
return ch;
}
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

/* USER CODE BEGIN PV */
static uint16_t globalGPIOPin;
uint8_t brojac = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
	setvbuf(stdin, NULL, _IONBF, 0);
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
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init(&hi2c2);
  LCD_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

   uint16_t HTU21D_ADC_Raw;
   uint8_t master_tx[3];
   uint8_t master_rx[3];
   uint8_t relej;
  while (1)
  {
	  master_tx[0] = relej;  // relej stanje za slave
	  master_tx[1] = 0x00;   // dummy
	  master_tx[2] = 0x00;

	  //SS Disabled
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	  HAL_Delay(1);  // Kratki delay

	     //Razmjena podataka
	     HAL_SPI_TransmitReceive(&hspi1, master_tx, master_rx, 3, 100);

	     //SS Enabled
	     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

	     uint8_t temp = master_rx[1];
	     uint16_t pritisak  = (master_rx[1] << 8) | master_rx[2];

	  printf("Master poslao: R=%d\n", relej);
	  printf("Master primio: T=%d, P=%d\n", temp, pritisak);
	  // Provjera na Tera Termu

	    float volt = (pritisak * 3.3) / 255.0;
	    float bar = 2.59 * volt - 1.29;
		  if (bar < 0) bar = 0;
		  switch(brojac) {
		  case 0:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);

			  if (bar > 0.1)
			  {
				  relej=1;
			  }
			  else
			  {
				  relej=0;
			  }

			  break;
		  case 1:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);

			  if (bar > 0.75)
			  {
				  relej=1;
			  }
			  else
			  {
				  relej=0;
			  }

			  break;
		  case 2:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);

			  if (bar > 1.2)
			  {
				  relej=1;
			  }
			  else
			  {
				  relej=0;
			  }

			  break;
		  case 3:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);
			  relej=1;

			  break;
		  default:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);
			  relej=0;

			  break;
		 }

		    LCD_SetCursor(0,0);
		    LCD_Printf("R:%d T:%d", relej, temp);
		    LCD_SetCursor(0,1);
		    LCD_Printf("P:%.2f B:%.2f", volt, bar);
		    HAL_Delay(100);


		  		/*  printf("Brojac: %d\r\n", brojac);
		  		  printf("Pritisak: %d\r\n", pritisak);
		  		  printf("Bar: %.4f\r\n", bar);
		  		  printf("Volt: %.2f\r\n", volt);
		  		  printf("Relej: %d\r\n\n", relej);*/

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	globalGPIOPin = GPIO_Pin;
	__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim6);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim -> Instance == TIM6) {
		if (HAL_GPIO_ReadPin(GPIOB, globalGPIOPin) == GPIO_PIN_RESET) {
			if (globalGPIOPin == GPIO_PIN_6) {
				if (0 <= brojac && brojac < 3)
					brojac++;
			}
		}
		if (HAL_GPIO_ReadPin(GPIOC, globalGPIOPin) == GPIO_PIN_RESET) {
			if (globalGPIOPin == GPIO_PIN_7) {
				if (0 < brojac && brojac <= 3)
				brojac--;
			}
		}
		HAL_TIM_Base_Stop_IT(&htim6);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
