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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MCP23S17.h"
#include "DS3231.h"
#include "DHT22.h"
#include "VFDDisplay.h"
#include "stdbool.h"
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
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
MCP23S17SPI RMI01_Hours;
MCP23S17SPI RMI02_Minutes;
MCP23S17SPI RMI03_Seconds;

DS3231_RTC RTCM;

DHT22_Data THSens;

uint8_t Time[3] = {0};
uint8_t Date[4] = {0};
bool hrformat = 0;
bool fAmPm = 0;
//Nyomógomb kezeléshez
typedef enum{

	BTN_NONE,
	BTN_SET,
	BTN_DOWN,
	BTN_UP,
	BTN_ALARM_OFF,

}ButtonState;

typedef enum {
    STATE_SHOW_TIME,       // Normál működés
    STATE_MENU_MAIN,       // Főmenü
    STATE_MENU_SUB      	// Almenü
} SystemState;

SystemState current_state = STATE_SHOW_TIME;
uint8_t main_menu_index = 0;
uint8_t sub_menu_index = 0;


volatile uint8_t fl_btn_pressed = 0;
volatile ButtonState current_btn = BTN_NONE;
volatile ButtonState event_btn = BTN_NONE;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM14_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void AppInit(void);
void AppRun(void);
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
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM14_Init();
  MX_ADC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  AppInit();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  AppRun();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4799;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 499;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 48;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 999;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DHT22_Pin|DCF77PON_Pin|CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DHT22_Pin */
  GPIO_InitStruct.Pin = DHT22_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT22_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DCF77PON_Pin CS_Pin */
  GPIO_InitStruct.Pin = DCF77PON_Pin|CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DCF77_SIG_Pin */
  GPIO_InitStruct.Pin = DCF77_SIG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DCF77_SIG_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void AppInit(){

	RMI01_Hours.hspi = &hspi1;
	RMI01_Hours.CS_Port = GPIOA;
	RMI01_Hours.CSPin = GPIO_PIN_3;
	RMI01_Hours.Address = 0;
	RMI01_Hours.EnableAddressing = 1;

	RMI02_Minutes.hspi = &hspi1;
	RMI02_Minutes.CS_Port = GPIOA;
	RMI02_Minutes.CSPin = GPIO_PIN_3;
	RMI02_Minutes.Address = 1;
	RMI02_Minutes.EnableAddressing = 1;

	RMI03_Seconds.hspi = &hspi1;
	RMI03_Seconds.CS_Port = GPIOA;
	RMI03_Seconds.CSPin = GPIO_PIN_3;
	RMI03_Seconds.Address = 2;
	RMI03_Seconds.EnableAddressing = 1;

	RTCM.hi2c = &hi2c1;
	RTCM.CR_EOSC = 0;
	RTCM.CR_BBSQW = 0;
	RTCM.CR_CONV = 0;
	RTCM.CR_RS1 = 0;
	RTCM.CR_RS2 = 0;
	RTCM.CR_INTCN = 0;
	RTCM.CR_A2IE = 0;
	RTCM.CR_A1IE = 0;
	RTCM.HMode = 0;

	THSens.DataPort = GPIOA;
	THSens.DataPin = GPIO_PIN_0;

	MCP23S17_Init(&RMI01_Hours);
	MCP23S17_Init(&RMI02_Minutes);
	MCP23S17_Init(&RMI03_Seconds);

	DS3231_Init(&RTCM);

	DHT22_Init();

	MCP23S17_PortMode(&RMI01_Hours, ADDR_IODIRA, 0x00);
	MCP23S17_PortMode(&RMI01_Hours, ADDR_IODIRB, 0x00);
	MCP23S17_PortMode(&RMI02_Minutes, ADDR_IODIRA, 0x00);
	MCP23S17_PortMode(&RMI02_Minutes, ADDR_IODIRB, 0x00);
	MCP23S17_PortMode(&RMI03_Seconds, ADDR_IODIRA, 0x00);
	MCP23S17_PortMode(&RMI03_Seconds, ADDR_IODIRB, 0x00);

	HAL_TIM_Base_Start_IT(&htim3);
}

ButtonState Get_Pressed_Button(void){

	uint32_t adc_value = 0;
	HAL_ADC_Start(&hadc);

	if(HAL_ADC_PollForConversion(&hadc, 10) == HAL_OK){
		adc_value = HAL_ADC_GetValue(&hadc);
	}
	HAL_ADC_Stop(&hadc);

	if(adc_value <= 500){

		return BTN_ALARM_OFF;
	}
	else if(adc_value > 1700 && adc_value < 2200){

		return BTN_UP;
	}
	else if(adc_value > 2300 && adc_value < 2600){

		return BTN_DOWN;
	}
	else if(adc_value > 2600 && adc_value < 2800){

		return BTN_SET;
	}

	else {

	return BTN_NONE;

	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM3){

		static ButtonState last_btn = BTN_NONE;
		ButtonState current_read = Get_Pressed_Button();
		current_btn = current_read;

		if(current_read != BTN_NONE && current_read != last_btn){

			event_btn = current_read;
			fl_btn_pressed = 1;
		}
		last_btn = current_read;
	}
}

void AppRun(){

	    static uint32_t last_display_tick = 0;
	    static uint32_t last_blink_tick = 0;
	    static uint32_t last_rtc_read = 0;
	    static uint32_t last_dht_read = 0;
	    static uint8_t blink_state = 0;

	    if (current_state == STATE_SHOW_TIME) {
	        if (HAL_GetTick() - last_rtc_read >= 200) {

	            last_rtc_read = HAL_GetTick();
	            //Idő lekérdezés
	            DS3231_GetTime(&RTCM, Time, &hrformat, &fAmPm);
	            //Dátum lekérdezés
	            DS3231_GetDate(&RTCM, Date);
	        }

	        if(HAL_GetTick() - last_dht_read >= 3000){
	        	last_dht_read =  HAL_GetTick();
	        	//Hőmérséklet lekérdezés
	        	DHT22_Read(&THSens);
	        }
	    }

	    // ========================================================
	    // VILLOGÁSI ÜTEM GENERÁLÁSA (400 ms)
	    // ========================================================
	    if (HAL_GetTick() - last_blink_tick >= 400) {
	        last_blink_tick = HAL_GetTick();
	        blink_state = !blink_state;
	    }

	    // ========================================================
	    // GOMBNYOMÁSOK ÉS MENÜ (Állapotgép)
	    // ========================================================

	    if (fl_btn_pressed == 1) {
	    	fl_btn_pressed = 0;

	            switch (current_state) {

	                //Kijelzés
	                case STATE_SHOW_TIME:
	                    if (event_btn == BTN_SET) {
	                        current_state = STATE_MENU_MAIN;
	                        main_menu_index = 0;
	                    }
	                    break;

	                //Főmenü
	                case STATE_MENU_MAIN:
	                    if (event_btn == BTN_SET) {
	                        current_state = STATE_SHOW_TIME;
	                    }
	                    else if (event_btn == BTN_UP) {
	                        main_menu_index = (main_menu_index == 2) ? 0 : main_menu_index + 1;
	                    }
	                    else if (event_btn == BTN_DOWN) {
	                        main_menu_index = (main_menu_index == 0) ? 2 : main_menu_index - 1;
	                    }
	                    else if (event_btn == BTN_ALARM_OFF) {
	                        current_state = STATE_MENU_SUB;
	                        sub_menu_index = 0;
	                    }
	                    break;

	                //Almenü
	                case STATE_MENU_SUB:
	                    if (event_btn == BTN_ALARM_OFF) {

	                        if (main_menu_index == 0) {
	                            if (sub_menu_index == 0) sub_menu_index = 1;
	                            else {

	                                DS3231_SetTime(&RTCM, Time, hrformat, fAmPm);
	                                current_state = STATE_MENU_MAIN;
	                            }
	                        }
	                        else if (main_menu_index == 1) {
	                            if (sub_menu_index < 2) sub_menu_index++;
	                            else {
	                                DS3231_SetDate(&RTCM, Date);
	                                current_state = STATE_MENU_MAIN;
	                            }
	                        }
	                        else if (main_menu_index == 2) {
	                            if (sub_menu_index < 2) sub_menu_index++;
	                            else {
	                                current_state = STATE_MENU_MAIN;
	                            }
	                        }
	                    }
	                    //Up és Down nyomógombok kezelése
	                    else if (event_btn == BTN_UP || event_btn == BTN_DOWN) {
	                        int8_t dir = (event_btn == BTN_UP) ? 1 : -1;

	                        if (main_menu_index == 0) {
	                            if (sub_menu_index == 0) Time[2] = (Time[2] + dir + 24) % 24;
	                            if (sub_menu_index == 1) Time[1] = (Time[1] + dir + 60) % 60;
	                        }
	                        else if (main_menu_index == 1) {
	                            if (sub_menu_index == 0) Date[3] = (Date[3] + dir + 100) % 100;
	                            if (sub_menu_index == 1) {
	                            	Date[2] = Date[2] + dir;
	                                if (Date[2] > 12) Date[2] = 1;
	                                if (Date[2] < 1) Date[2] = 12;
	                            }
	                            if (sub_menu_index == 2) {
	                            	Date[1] = Date[1] + dir;
	                                if (Date[1] > 31) Date[1] = 1;
	                                if (Date[1] < 1) Date[1]= 31;
	                            }
	                        }
	                        else if (main_menu_index == 2) {
	                            //if (sub_menu_index == 0) alarm_h = (alarm_h + dir + 24) % 24;
	                            //if (sub_menu_index == 1) alarm_m = (alarm_m + dir + 60) % 60;
	                            //if (sub_menu_index == 2) alarm_on = !alarm_on;
	                        }
	                    }
	                    break;
	            }
	        }

	        // ========================================================
	        // KIJELZŐ UTASÍTÁSOK KIKÜLDÉSE (50 ms / 20 FPS)
	        // ========================================================
	        if (HAL_GetTick() - last_display_tick >= 50) {
	            last_display_tick = HAL_GetTick();

	            // A) NORMÁL MÓD: Automatikus rotálás
	            if (current_state == STATE_SHOW_TIME) {
	                if (Time[0] >= 46 && Time[0] <= 49)      VFD_Show(Date[3], Date[2], Date[1], 0, 0, 0);
	                else if (Time[0] >= 50 && Time[0] <= 53) VFD_ShowTemp(THSens.Temperature);
	                else if (Time[0] >= 54 && Time[0] <= 57) VFD_ShowHum(THSens.Humidity);
	                else                                   VFD_Show(Time[2], Time[1], Time[0], 0, 0, 0);
	            }

	            // B) FŐMENÜ neveinek kiírása
	            else if (current_state == STATE_MENU_MAIN) {
	                VFD_ShowMainMenu(main_menu_index);
	            }

	            // C) ALMENÜ (a beállítani kívánt érték villog)
	            else if (current_state == STATE_MENU_SUB) {

	                // Óra szerkesztése
	                if (main_menu_index == 0) {
	                    VFD_Show(Time[2], Time[1], 0,
	                            (sub_menu_index == 0) ? !blink_state : 0,
	                            (sub_menu_index == 1) ? !blink_state : 0,
	                            0);
	                }
	                // Dátum szerkesztése
	                else if (main_menu_index == 1) {
	                    VFD_Show(Date[3], Date[2], Date[1],
	                            (sub_menu_index == 0) ? !blink_state : 0,
	                            (sub_menu_index == 1) ? !blink_state : 0,
	                            (sub_menu_index == 2) ? !blink_state : 0);
	                }
	                // Alarm szerkesztése
	                else if (main_menu_index == 2) {
	                    if (sub_menu_index == 0) {
	                        //VFD_Show(alarm_h, alarm_m, 0, !blink_state, 0, 0); // Óra villog
	                    }
	                    else if (sub_menu_index == 1) {
	                        //VFD_Show(alarm_h, alarm_m, 0, 0, !blink_state, 0); // Perc villog
	                    }
	                    else if (sub_menu_index == 2) {
	                        //VFD_ShowAlarmStatus(alarm_on, blink_state);        // On/OF villog
	                    }
	                }
	            }
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
