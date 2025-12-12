#include "main.h"
#include "cmsis_os.h"

/* --- RTOS Handles --- */
osThreadId ledTask1Handle;
osThreadId ledTask2Handle;
osSemaphoreId binarySemHandle;

/* --- Function Prototypes --- */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartLED1(void const * argument);
void StartLED2(void const * argument);

/* --- MAIN --- */
int main(void)
{
  /* Inisialisasi HAL dan sistem clock */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* Inisialisasi Semaphore (biner) */
  osSemaphoreDef(binarySem);
  binarySemHandle = osSemaphoreCreate(osSemaphore(binarySem), 1);  // Nilai awal = 1

  /* Membuat Task LED1 dan LED2 */
  osThreadDef(LED1, StartLED1, osPriorityNormal, 0, 128);
  ledTask1Handle = osThreadCreate(osThread(LED1), NULL);

  osThreadDef(LED2, StartLED2, osPriorityNormal, 0, 128);
  ledTask2Handle = osThreadCreate(osThread(LED2), NULL);

  /* Jalankan Scheduler */
  osKernelStart();

  while (1)
  {
    // Tidak akan pernah sampai sini, karena dikendalikan oleh RTOS
  }
}

/* --- TASK 1 --- */
void StartLED1(void const * argument)
{
  for(;;)
  {
    if (osSemaphoreWait(binarySemHandle, osWaitForever) == osOK)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // LED1 ON
      osDelay(300);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // LED1 OFF
      osSemaphoreRelease(binarySemHandle);
      osDelay(300);
    }
  }
}

/* --- TASK 2 --- */
void StartLED2(void const * argument)
{
  for(;;)
  {
    if (osSemaphoreWait(binarySemHandle, osWaitForever) == osOK)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // LED2 ON
      osDelay(1000);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // LED2 OFF
      osSemaphoreRelease(binarySemHandle);
      osDelay(1000);
    }
  }
}

/* --- GPIO Init --- */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* --- System Clock Configuration --- */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Konfigurasi Osilator HSE dan PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Konfigurasi clock CPU, AHB, dan APB */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* --- Error Handler --- */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
