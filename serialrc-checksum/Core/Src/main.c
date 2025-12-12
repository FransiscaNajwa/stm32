#include "main.h"
#include <stdio.h>

UART_HandleTypeDef huart1;
uint8_t rxPacket[3];

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // LED OFF awal

    while (1)
    {
        if (HAL_UART_Receive(&huart1, rxPacket, 3, 100) == HAL_OK)
        {
            if (rxPacket[0] == 0xAA && rxPacket[2] == (rxPacket[0] ^ rxPacket[1]))
            {
                if (rxPacket[1] == '1')
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // LED ON
                else if (rxPacket[1] == '0')
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   // LED OFF
            }
        }
    }
}
