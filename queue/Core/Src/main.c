/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : FreeRTOS Queue Demo (LED bergantian)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "cmsis_os.h"

/* Private variables ---------------------------------------------------------*/
osThreadId producerTaskHandle;
osThreadId consumerTaskHandle;
osMessageQId LedQueueHandle;

/* USER CODE BEGIN PV */
uint16_t ledValue = 0;
/* USER CODE END PV */

/* Function prototypes -------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartProducerTask(void const * argument);
void StartConsumerTask(void const * argument);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* RTOS Queue */
  osMessageQDef(LedQueue, 8, uint16_t);
  LedQueueHandle = osMessageCreate(osMessageQ(LedQueue), NULL);

  /* RTOS Threads */
  osThreadDef(producerTask, StartProducerTask, osPriorityNormal, 0, 128);
  producerTaskHandle = osThreadCreate(osThread(producerTask), NULL);

  osThreadDef(consumerTask, StartConsumerTask, osPriorityNormal, 0, 128);
  consumerTaskHandle = osThreadCreate(osThread(consumerTask), NULL);

  /* Start scheduler */
  osKernelStart();

  while (1)
  {
  }
}

/* PRODUCER TASK -------------------------------------------------------------*/
void StartProducerTask(void const * argument)
{
  for(;;)
  {
    ledValue = !ledValue;   // Toggle nilai 0 ↔ 1
    osMessagePut(LedQueueHandle, ledValue, 0); // Kirim ke queue
    osDelay(1000); // Delay 1 detik
  }
}

/* CONSUMER TASK -------------------------------------------------------------*/
void StartConsumerTask(void const * argument)
{
  osEvent event;
  for(;;)
  {
    event = osMessageGet(LedQueueHandle, osWaitForever);
    if(event.status == osEventMessage)
    {
      uint16_t val = event.value.v;

      if(val == 1)
      {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // LED1 ON
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // LED2 OFF
      }
      else
      {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // LED1 OFF
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // LED2 ON
      }
    }
  }
}

/* CLOCK CONFIG --------------------------------------------------------------*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

/* GPIO INIT -----------------------------------------------------------------*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Awal semua LED mati
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

/* ERROR HANDLER -------------------------------------------------------------*/
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
