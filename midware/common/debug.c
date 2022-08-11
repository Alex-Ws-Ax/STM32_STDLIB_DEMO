#include <string.h>
#include "debug.h"
#include "stm32f10x_usart.h"
#include "cmd.h"

#define     DEBUG_UART      USART1
#define     MAX_RX_LEN      64

unsigned char  rx_buf[MAX_RX_LEN] = {0};
unsigned char  rx_index = 0;
unsigned short debug_level = DEBUG_ALL;

void UART_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void DebugInit(void)
{
    USART_InitTypeDef uart_init_t = {0};
    NVIC_InitTypeDef nvic_init_t = {0};

    UART_GPIO_Config();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_StructInit(&uart_init_t);
    USART_Init(DEBUG_UART, &uart_init_t);

    USART_ITConfig(DEBUG_UART, USART_IT_RXNE, ENABLE);

    nvic_init_t.NVIC_IRQChannel = USART1_IRQn;
    nvic_init_t.NVIC_IRQChannelPreemptionPriority  = 0;
    nvic_init_t.NVIC_IRQChannelSubPriority         = 0;
    nvic_init_t.NVIC_IRQChannelCmd                 = ENABLE;

    NVIC_Init(&nvic_init_t);
    USART_Cmd(DEBUG_UART, ENABLE);
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(DEBUG_UART, USART_IT_RXNE) == SET)
    {
        unsigned char data = USART_ReceiveData(DEBUG_UART);
        if (rx_index < MAX_RX_LEN)
        {
            if ((data == 0x0A) || (data == 0x0D))
            {
                cmd_parsing((char *)rx_buf);
                rx_index = 0;
                memset(rx_buf, 0, MAX_RX_LEN);
            }
            else
            {
                rx_buf[rx_index++] = data;
            }
        }
        else
        {
            rx_index = 0;
            memset(rx_buf, 0, MAX_RX_LEN);
        }
        USART_ClearITPendingBit(DEBUG_UART, USART_IT_RXNE);
    }

}

int fputc(int ch, FILE *p)
{
    USART_SendData(DEBUG_UART, (u8)ch);

    while (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_TXE) == RESET);

    return ch;
}

