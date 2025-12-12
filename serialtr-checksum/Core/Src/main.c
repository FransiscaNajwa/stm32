#include "main.h"
#include <stdio.h>

UART_HandleTypeDef huart1;
uint8_t buttonState = 0;
uint8_t txData = '0';

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    while (1)
    {
        buttonState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

        if (buttonState == GPIO_PIN_SET)
            txData = '1';
        else
            txData = '0';

        uint8_t packet[3];
        packet[0] = 0xAA;                    // Start Byte
        packet[1] = txData;                  // Payload
        packet[2] = packet[0] ^ packet[1];   // Checksum XOR

        HAL_UART_Transmit(&huart1, packet, 3, HAL_MAX_DELAY);
        HAL_Delay(200);
    }
}
