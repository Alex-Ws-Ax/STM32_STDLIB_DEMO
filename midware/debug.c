#include "debug.h"
#include "stm32f10x_usart.h"

#define     DEBUG_UART      USART1

unsigned short debug_level = DEBUG_ALL;

void UART_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

}


void DebugInit(void)
{
    USART_InitTypeDef uart_init_t = {0};

    UART_GPIO_Config();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_StructInit(&uart_init_t);
    USART_Init(DEBUG_UART, &uart_init_t);
    USART_Cmd(DEBUG_UART,ENABLE);
}



int fputc(int ch, FILE *p)
{
    USART_SendData(DEBUG_UART, (u8)ch);

    while (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_TXE) == RESET);

    return ch;

}

