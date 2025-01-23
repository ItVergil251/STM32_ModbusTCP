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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "pwm_changer.h"
#include "ModBusTCP.h"
#include "pc_to_usb.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef uint8_t byte;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

/* USER CODE BEGIN PV */
uint8_t buffer[128];
char tx_data[128];

uint16_t read_data[10];
bool read_coil[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
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
	uint16_t wr_Hreg=0;
	bool wr_Coil = true;
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
  MX_SPI1_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LD_DBG_GPIO_Port,LD_DBG_Pin,1);

  HAL_Delay(1000);
  PRINT_HEADER();

  ModBus_Client client = {
  .CS_pin = SPI1_CS_Pin,
  .CS_port = SPI1_CS_GPIO_Port,
  .spi = &hspi1,
  .wiznet = {
		  .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
          .ip = {10, 27, 27, 35},						// IP address
          .sn = {255, 255, 255, 0},					// Subnet mask
          .gw = {27, 27, 27, 1}						// Gateway address
  	  	  }
  };
  ModBus_Server srvr = {
		  .id=0x01,
		  .ip={10,27,27,156}
  };

  if (client_init(&client)) PRINT_NETINFO(&client.wiznet);

  if (connect_server(&srvr)==0)
  {
	  sprintf(tx_data,"Connection success to:\r\n");
	  send_string(tx_data);
	  sprintf(tx_data,IP_MSG,srvr.ip[0],srvr.ip[1],srvr.ip[2],srvr.ip[3]);
	  send_string(tx_data);
  }
  else
  {
	  send_string("Connection failed\r\n");
  }

  uint16_t address_Hreg = 40000;
  uint16_t address_Coil = 0;
  uint16_t start_Hreg = 40000;
  uint16_t start_Coil = 0;
  uint16_t start_Input = 10000;
  uint16_t start_Ireg = 30000;

  send_string(SEPARATOR);
  send_string(READY_MSG);
  send_string(SEPARATOR);
  memset(buffer,'\0',sizeof(buffer));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if (write_COIL(&srvr,address_Coil,wr_Coil) == 0)
   	  {
    	  sprintf(tx_data,"Write Coil 0x%X4 %d OK\r\n",address_Coil,wr_Coil);
    	  send_string(tx_data);
      }
   	  else
   	  {
   		  sprintf(tx_data,"Write Coil 0x%X4 %d ERR\r\n",address_Coil,wr_Coil);
   		  send_string(tx_data);
	      while(1)
	      {
	    	  HAL_Delay(500);
	       	  HAL_GPIO_TogglePin(LD_DBG_GPIO_Port, LD_DBG_Pin);
	          if (write_COIL(&srvr,address_Coil,wr_Coil) == 0) break;
	      }
   	  }

   	  if (read_Coils(&srvr,read_coil,start_Coil,10) == 0)
   	  {
   		  sprintf(tx_data,"Read 10 Coils from %d OK\r\n",start_Coil);
   		  send_string(tx_data);
	      sprintf(tx_data,"Response: %X-%X-%X-%X-%X-%X-%X-%X-%X-%X\r\n",
	    		       read_coil[0],read_coil[1],read_coil[2],
	                   read_coil[3],read_coil[4],read_coil[5],
	                   read_coil[6],read_coil[7],read_coil[8],
	                   read_coil[9]);
	      send_string(tx_data);
   	  }
   	  else
   	  {
   		  sprintf(tx_data,"Read 10 Coils from %d ERR\r\n",start_Coil);
   		  send_string(tx_data);
   		  while(1)
   		  {
   			  HAL_Delay(500);
   			  HAL_GPIO_TogglePin(LD_DBG_GPIO_Port, LD_DBG_Pin);
   			  if (read_Coils(&srvr,read_coil,start_Coil,10) == 0) break;
   		  }
   	  }

      if (write_HREG(&srvr,address_Hreg,wr_Hreg) == 0)
   	  {
    	  sprintf(tx_data,"Write Hreg %d value %d OK\r\n",address_Hreg,wr_Hreg);
    	  send_string(tx_data);
      }
   	  else
   	  {
   		  sprintf(tx_data,"Write Hreg %d value %d ERR\r\n",address_Hreg,wr_Hreg);
   		  send_string(tx_data);
	      while(1)
	      {
	    	  HAL_Delay(500);
	       	  HAL_GPIO_TogglePin(LD_DBG_GPIO_Port, LD_DBG_Pin);
	          if (write_COIL(&srvr,address_Coil,wr_Hreg) == 0) break;
	      }
   	  }

   	  if (read_HREGs(&srvr,read_data,start_Hreg,10) == 0)
   	  {
   		  sprintf(tx_data,"Read 10 Hregs from %d OK\r\n",start_Hreg);
   		  send_string(tx_data);
	      sprintf(tx_data,"Response: %X4-%X4-%X4-%X4-%X4-%X4-%X4-%X4-%X4-%X4\r\n",
	    		       read_data[0],read_data[1],read_data[2],
	                   read_data[3],read_data[4],read_data[5],
	                   read_data[6],read_data[7],read_data[8],
	                   read_data[9]);
	      send_string(tx_data);
   	  }
   	  else
   	  {
   		  sprintf(tx_data,"Read 10 Hregs from %d ERR\r\n",start_Hreg);
   		  send_string(tx_data);
   		  while(1)
   		  {
   			  HAL_Delay(500);
   			  HAL_GPIO_TogglePin(LD_DBG_GPIO_Port, LD_DBG_Pin);
   			  if (read_HREGs(&srvr,read_data,start_Hreg,10) == 0) break;
   		  }
   	  }

   	  address_Coil = (address_Coil < 8) ? address_Coil + 1 : 0;
   	  address_Hreg = (address_Hreg < 40009) ? address_Hreg + 1 : 40000;
   	  wr_Hreg = (wr_Hreg < UINT16_MAX) ? wr_Hreg + 1 : 0;
   	  wr_Coil = !wr_Coil;
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 5;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
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
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 600-1;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 100-1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */
  HAL_TIM_MspPostInit(&htim10);

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 60-1;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 200-1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim11, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */
  HAL_TIM_MspPostInit(&htim11);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD_DBG_GPIO_Port, LD_DBG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_USR_Pin|SPI1_RST_Pin|SPI1_INT_Pin|SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ENABLE_Pin|FIRE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD_DBG_Pin */
  GPIO_InitStruct.Pin = LD_DBG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD_DBG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_BT_Pin */
  GPIO_InitStruct.Pin = KEY_BT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY_BT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_USR_Pin SPI1_RST_Pin SPI1_INT_Pin SPI1_CS_Pin */
  GPIO_InitStruct.Pin = LD2_USR_Pin|SPI1_RST_Pin|SPI1_INT_Pin|SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ENABLE_Pin FIRE_Pin */
  GPIO_InitStruct.Pin = ENABLE_Pin|FIRE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
