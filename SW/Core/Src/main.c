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
#include "stdbool.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum Button{
	event_Short = 0,
	event_Long,
	event_None
} ButtonEvent_t;

typedef enum {
	MODE_NOMAL = 0, // 일반 시계 모드
	MODE_SET_HOUR, // '시' 변경 모드 ( 깜빡임 효과 )
	MODE_SET_MIN // '분' 변경 모드
} clockMode_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */

clockMode_t clock_mode = MODE_NOMAL;

RTC_TimeTypeDef set_time = {0}; // NOTE: 시간 설정할 때 사용할 구조체

static const uint8_t FND_NUM[] = {
// 				 LED:  7 6 5 4 3 2 1 0
// 				 FND:  G F A B E D C DP
		0x81, // 0 (0b 1 0 0 0 0 0 0 1)
		0xED, // 1 (0b 1 1 1 0 1 1 0 1)
		0x43, // 2 (0b 0 1 0 0 0 0 1 1)
		0x49, // 3 (0b 0 1 0 0 1 0 0 1)
		0x2D, // 4 (0b 0 0 1 0 1 1 0 1)
		0x19, // 5 (0b 0 0 0 1 1 0 0 1)
		0x11, // 6 (0b 0 0 0 1 0 0 0 1)
		0xCD, // 7 (0b 1 1 0 0 1 1 0 1)
		0x01, // 8 (0b 0 0 0 0 0 0 0 1)
		0x09  // 9 (0b 0 0 0 0 1 0 0 1)
		};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void Display_FND(uint8_t hours, uint8_t minutes, clockMode_t mode, bool blink_state);

ButtonEvent_t Button_event(GPIO_TypeDef *port, uint16_t gpio);
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
  MX_USB_PCD_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };

	uint32_t last_rtc_tick = 0;
	uint32_t last_blink_tick = 0;
	bool blink_state = true;


	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	while (1) {

		ButtonEvent_t btn_event = Button_event(CTL_BTN_GPIO_Port, CTL_BTN_Pin);

		// NOTE: Long Key를 눌러서 설정모드로 진입.
		if (btn_event == event_Long) {

			if (clock_mode == MODE_NOMAL) {

				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

				set_time = sTime;

				clock_mode = MODE_SET_HOUR;

			} else if (clock_mode == MODE_SET_HOUR) {
				clock_mode = MODE_SET_MIN;
			} else if (clock_mode == MODE_SET_MIN) {
				HAL_RTC_SetTime(&hrtc, &set_time, RTC_FORMAT_BIN);
				clock_mode = MODE_NOMAL;
			}

		} else if(btn_event == event_Short){
			// NOTE: Long key 이벤트 눌림 후 short key 이벤트를 받으면, 설정 진행.
			if(clock_mode == MODE_SET_HOUR){
				set_time.Hours = (set_time.Hours + 1) % 24;
			} else if(clock_mode == MODE_SET_MIN){
				set_time.Minutes = (set_time.Minutes +1 ) % 60;
				set_time.Seconds = 0;
			}
		}

		if (clock_mode == MODE_NOMAL) {
			if (HAL_GetTick() - last_rtc_tick >= 500) {

				last_rtc_tick = HAL_GetTick();

				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
			}
		}

		if (HAL_GetTick() - last_blink_tick >= 300) {
			last_blink_tick = HAL_GetTick();
			blink_state = !blink_state;
		}

		if (clock_mode == MODE_NOMAL) {
			Display_FND(sTime.Hours, sTime.Minutes, MODE_NOMAL, true);
		} else{
			Display_FND(set_time.Hours, set_time.Minutes, clock_mode, blink_state);
		}


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0x3210) { // NOTE: 기존 시간 초기화 방지 만약에 뭔가 들어가 있으면 계속 기억하고 있음.
		return;
	}
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 10;
  sTime.Minutes = 10;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x3210);
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin
                          |LED4_Pin|LED5_Pin|LED6_Pin|LED7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CH0_Pin|CH1_Pin|CH2_Pin|CH3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin LED3_Pin
                           LED4_Pin LED5_Pin LED6_Pin LED7_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin
                          |LED4_Pin|LED5_Pin|LED6_Pin|LED7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CTL_BTN_Pin */
  GPIO_InitStruct.Pin = CTL_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CTL_BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CH0_Pin CH1_Pin CH2_Pin CH3_Pin */
  GPIO_InitStruct.Pin = CH0_Pin|CH1_Pin|CH2_Pin|CH3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// return event_Short, event_Long, event_None
ButtonEvent_t Button_event(GPIO_TypeDef *port, uint16_t gpio) {
	static bool busy = false;
	static bool event_done = false;
	static uint32_t temp_tick = 0;

	if (!HAL_GPIO_ReadPin(port, gpio)) {

		if (!busy) {
			temp_tick = HAL_GetTick();
			event_done = false;
			busy = true;
		}

		if ((HAL_GetTick() - temp_tick >= 1000) && !event_done) {
			event_done = true;
			return event_Long;
		}
	}

	else {

		if (busy) {
			busy = false;

			if (!event_done) {
				event_done = true;

				if (HAL_GetTick() - temp_tick >= 50) {
					return event_Short;
				}
			}
		}
	}

	return event_None;
}

void Display_FND(uint8_t hours, uint8_t minutes, clockMode_t mode, bool blink_state){
	uint8_t digits[4];
	digits[0] = hours / 10;
	digits[1] = hours % 10;
	digits[2] = minutes / 10;
	digits[3] = minutes % 10;

	GPIO_TypeDef *CH_PORT[4] = { CH0_GPIO_Port, CH1_GPIO_Port, CH2_GPIO_Port, CH3_GPIO_Port };
	uint16_t CH_PIN[4] = { CH0_Pin, CH1_Pin, CH2_Pin, CH3_Pin };

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) { // NOTE: FND 채널 모두 Off.
			HAL_GPIO_WritePin(CH_PORT[j], CH_PIN[j], GPIO_PIN_SET);
		}

		GPIOA->ODR = (GPIOA->ODR & (~0xFF)) | 0xFF; // NOTE: segment 잔상 방지.

		bool show_digit = true;

		if (!blink_state) {
			if ((mode == MODE_SET_HOUR) && (i == 0 || i == 1)) { // NOTE: 시간 설정하고 FND 시간 2개 구간.
				show_digit = false; // NOTE: '시' 설정 중일 때 '시' 자리를 꺼둠
			} else if ((mode == MODE_SET_MIN) && (i == 2 || i == 3)) {
				show_digit = false; // NOTE:'분' 설정 중일 때 '분' 자리를 꺼둠
			}
		}

		if(show_digit){
			GPIOA->ODR = (GPIOA->ODR & (~0xFF)) | FND_NUM[digits[i]];
		} else{
			GPIOA->ODR = (GPIOA->ODR & (~0xFF)) | 0xFF;
		}

		HAL_GPIO_WritePin(CH_PORT[i], CH_PIN[i], GPIO_PIN_RESET);

		HAL_Delay(2);
	}

	for (int j = 0; j < 4; j++) {
		HAL_GPIO_WritePin(CH_PORT[j], CH_PIN[j], GPIO_PIN_SET);
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
