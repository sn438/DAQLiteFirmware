/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg);
static uint8_t SD_ReadyWait(void);
extern volatile uint8_t Timer1, Timer2; 

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static  void  SPI_TxByte (uint8_t data);
static uint8_t SPI_RxByte (void);
static  void  SELECT (void);
static void DESELECT(void);
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI1_Init();

  
  /* USER CODE BEGIN 2 */
  uint8_t cmd_arg [ 6 ] ; 
  uint32_t Count =  0xFFFF ; 
  
  /** Send SPI message in Deselect state and put it in standby state. **/ 
  DESELECT ( ) ; 
  
  for ( int i =  0 ; i <  10 ; i ++ ) 
  { 
    SPI_TxByte ( 0xFF ) ; 
  } 
  
  /** SPI Chips Select **/ 
  SELECT ( ) ; 
  
	uint8_t CMD0 = 0x40 + 0;
  /** Initial GO_IDLE_STATE state transition **/ 
  cmd_arg [ 0 ]  =  (CMD0|0x40) ; 
  cmd_arg [ 1 ]  =  0 ; 
  cmd_arg [ 2 ]  =  0 ; 
  cmd_arg [ 3 ]  =  0 ; 
  cmd_arg [ 4 ]  =  0 ; 
  cmd_arg [ 5 ]  =  0x95 ; 
  
  /** Send the command **/ 
  for  ( int i =  0 ; i <  6 ; i ++) 
  { 
    SPI_TxByte ( cmd_arg [ i ] ) ; 
  } 
  
  /** Wait for a response **/
  while  ((SPI_RxByte()  !=  0x01) && Count) 
  { 
    HAL_GPIO_WritePin ( GPIOG , GPIO_PIN_14 ,  GPIO_PIN_RESET ) ;
		HAL_GPIO_WritePin ( GPIOG , GPIO_PIN_13 ,  GPIO_PIN_RESET ) ; 	
		Count--;
  } 
	if (Count != 0) {
		HAL_GPIO_WritePin ( GPIOG , GPIO_PIN_14 ,  GPIO_PIN_SET ) ;
	}
	DESELECT();
  SPI_TxByte(0XFF);
  SELECT(); 
	if (SD_SendCmd(CMD0, 0) == 1) 
  { 
		//HAL_GPIO_WritePin ( GPIOG , GPIO_PIN_14 ,  GPIO_PIN_RESET ) ;
		HAL_GPIO_WritePin ( GPIOG , GPIO_PIN_13 ,  GPIO_PIN_SET ) ; 
	}


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

/** SPI data transfer **/ 
static  void  SPI_TxByte (uint8_t data) 
{ 
  while  ( HAL_SPI_GetState ( & hspi1 )  !=  HAL_SPI_STATE_READY ) ; 
  HAL_SPI_Transmit ( & hspi1 ,  & data ,  1 ,  5000 ) ; 
} 

/** SPI data send / receive return type function **/ 
static uint8_t SPI_RxByte ( void ) 
{ 
  uint8_t dummy , data ; 
  dummy =  0xFF ; 
  data =  0 ; 
  
  while  (( HAL_SPI_GetState ( & hspi1 )  !=  HAL_SPI_STATE_READY ) ) ; 
  HAL_SPI_TransmitReceive ( & hspi1 ,  & dummy ,  & data ,  1 ,  5000 ) ; 
  
  return data ; 
} 




/** SPI Chip Select **/ 
static  void  SELECT ( void ) 
{ 
  HAL_GPIO_WritePin ( GPIOC , GPIO_PIN_7 ,  GPIO_PIN_RESET ) ; 
} 

/** SPI Chip Deselect **/ 
static  void  DESELECT ( void ) 
{ 
  HAL_GPIO_WritePin ( GPIOC , GPIO_PIN_7 ,  GPIO_PIN_SET ) ; 
} 

static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg) 
{
  uint8_t crc, res;
  
  /* SD?? ?? */
  if (SD_ReadyWait() != 0xFF)
    return 0xFF;
  
  /* ?? ?? ?? */
  SPI_TxByte(cmd); 			/* Command */
  SPI_TxByte((uint8_t) (arg >> 24)); 	/* Argument[31..24] */
  SPI_TxByte((uint8_t) (arg >> 16)); 	/* Argument[23..16] */
  SPI_TxByte((uint8_t) (arg >> 8)); 	/* Argument[15..8] */
  SPI_TxByte((uint8_t) arg); 		/* Argument[7..0] */
  
  /* ??? CRC ?? */
  crc = 0;  
  if (cmd == 0x40)
    crc = 0x95; /* CRC for CMD0(0) */
  
  if (cmd == 0x48)
    crc = 0x87; /* CRC for CMD8(0x1AA) */
  
  /* CRC ?? */
  SPI_TxByte(crc);
  
  /* CMD12 Stop Reading ??? ???? ?? ??? ??? ??? */
  if (cmd == 0x4C)
    SPI_RxByte();
  
  /* 10? ?? ?? ???? ????. */
  uint8_t n = 10; 
  do
  {
    res = SPI_RxByte();
  } while ((res & 0x80) && --n);
  
  return res;
}
static uint8_t SD_ReadyWait(void) 
{
  uint8_t res;
  
  /* 500ms ??? ?? */
  Timer2 = 50;
  SPI_RxByte();
  
  do
  {
    /* 0xFF ?? ??? ? ?? SPI ?? */
    res = SPI_RxByte();
  } while ((res != 0xFF) && Timer2);
  
  return res;
}
#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/