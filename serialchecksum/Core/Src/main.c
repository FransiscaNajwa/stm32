/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "serial_protocol.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* Callback saat paket valid masuk */
void my_packet_cb(uint8_t cmd, uint8_t *payload, uint8_t len)
{
    if (cmd == 0x01)  // contoh command
    {
        // Kirim balasan (ACK) dengan payload yang sama
        SP_SendPacket(0x81, payload, len);
    }
}

/* Main program --------------------------------------------------------------*/
int main(void)
{
    /* MCU Configuration ------------------------------------------------------*/
    HAL_Init();
    SystemClock_Config();     // Clock 72 MHz dari HSE
    MX_GPIO_Init();
    MX_USART1_UART_Init();    // USART1 @115200, 8N1

    /* Initialize protocol serial */
    SP_Init(&huart1, my_packet_cb);

    /* Contoh kirim paket awal saat startup */
    uint8_t hello[] = { 'O', 'K' };
    SP_SendPacket(0x10, hello, 2);

    /* Infinite loop */
    while (1)
    {
        // Aplikasi utama bisa jalan di sini
        HAL_Delay(100);
    }
}

/* HAL UART RX Complete Callback - wajib untuk menerima data */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        SP_HandleRxByte(sp_rx_byte);  // teruskan ke parser protokol
    }
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* System Clock Configuration - HSE 8MHz -> 72MHz */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB busses clocks */
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
}

/* GPIO init function */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /* Add if needed - LED or other IO */
}

/* Error Handler */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
