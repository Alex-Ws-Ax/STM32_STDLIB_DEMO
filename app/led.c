#include "stm32f10x_gpio.h"
#include "led.h"
#include "debug.h"

void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOD Periph clock enable */
    RCC_APB2PeriphClockCmd(LED_RCC_PORT, ENABLE);

    /* Configure PD0 and PD2 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED_PORT,LED_PIN);
}

void Led_ON(void)
{
    GPIO_ResetBits(LED_PORT,LED_PIN);
    DEBUG(DEBUG_INFO,"led on\n");
}

void Led_OFF(void)
{
    GPIO_SetBits(LED_PORT,LED_PIN);
    DEBUG(DEBUG_INFO,"led off\n");
}


