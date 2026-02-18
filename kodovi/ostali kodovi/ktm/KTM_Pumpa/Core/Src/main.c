/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <math.h>
#include <htu21d.h>
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
uint8_t HTU21D_RX_Data[3];
float HTU21D_Temperature;
float HTU21D_Humidity;
uint16_t HTU21D_ADC_Raw;
uint8_t HTU21D_Temp_Cmd = 0xE3;
uint8_t HTU21D_Humi_Cmd = 0xE5;
#define HTU21D_Adress (0x40 << 1)

#define HTU21D_POLY 0x13100

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//LSB------------------MSB
/*
uint8_t TxAddress[] = {0xEE,0xDD,0xCC,0xBB,0xAA}; //definiranje adrese Tx i Rx
uint8_t TxData[] = "Hello World";
uint8_t RxAddress[] = {0xEE,0xDD,0xCC,0xBB,0xAA};  //mora biti isto kao TxAddress
uint8_t RxData[16];*/

/* CRC Check */
uint8_t HTU21D_CRC8(uint16_t HTU21D_ADC, uint8_t *HTU21D_CRC)

{

/* If ADC/CRC == 0 -> Sensor does not connected */
if(HTU21D_ADC == 0 | *HTU21D_CRC == 0)
	return 0;

for (uint8_t i = 0; i < 16; i++)
{
	HTU21D_ADC = ((HTU21D_ADC & 0x8000) ? ((HTU21D_ADC << 1) ^ HTU21D_POLY) : (HTU21D_ADC <<= 1));
}
HTU21D_ADC >>= 8;

if((uint8_t)HTU21D_ADC == *HTU21D_CRC)
	return 255; /* if OK */
else
	return 0; /* if no OK */
}
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
 /* NRF_Init();

  NRF_TXMode(TxAddress, 10); //ovo mora bit ili jedno ili drugo, npr. ako zelis slat ukljucis Tx a ako zelis primat ukljucis Rx
   NRF_RXMode(RxAddress, 10); //ove adrese i broj kanala mogu ostat isti, jednio sta se mijenja je data koji saljes kasnije u programu
*/
  uint16_t pritisak=0;
  uint8_t relej=0, temp=0;
  float raw;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {




	  HAL_I2C_Mem_Read_IT(&hi2c1, HTU21D_Adress, HTU21D_Temp_Cmd, I2C_MEMADD_SIZE_8BIT, HTU21D_RX_Data, 3);
	  	HTU21D_ADC_Raw = ((uint16_t)(HTU21D_RX_Data[0] << 8) | (HTU21D_RX_Data[1]));
	  	HTU21D_Temperature = (float)(HTU21D_ADC_Raw  / 65536.00)* 175.72 - 46.85;
	  	HAL_Delay(100);

	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, relej);

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  pritisak = HAL_ADC_GetValue(&hadc1);

 /* //probaj s jednog mikroupravljača slat a na drugom primat podatke da vidis jel radi prijenos samo jednog podatka prije svega
	  	  //slanje podatka
	  	  if(NRF_Transmit(pritisak)==1)
	  	  {
	  		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); //provjera ispravnosti blinkanjem PA5 ledice
	  	  }

	  	  //primanje podatka
	  	  if(isDataAvailable(1)==1) //provjera je li data pipe slobodan
	  	  {
	  		  NRF_Receive(relej);
	  	  }*/

	  	  		  printf("Relej: %d\r\n", relej);
	  	  		  printf("P: %d\r\n", pritisak);
	  	  		  printf("R: %d\r\n", raw);
	  	  		  printf("T: %d\r\n", HTU21D_Temperature);
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
