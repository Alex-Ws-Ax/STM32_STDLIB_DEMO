#include "stm32f10x_gpio.h"
#include "iic_test.h"
#include "debug.h"

#define     EVENT_IO_PORT           GPIOB
#define     EVENT_IO_PIN            GPIO_Pin_10

uint8_t g_test_flag = 0;

void EVENT_IO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = EVENT_IO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(EVENT_IO_PORT, &GPIO_InitStruct);
}

void press_read_state(void)
{
    uint8_t key_state = 0;
    uint8_t cmd = 0x02;
    
    IIC_RxData(0xA0,&cmd,1,&key_state,1);

    if(key_state == 0)
    {
        DEBUG(DEBUG_INFO, "key release\n");
    }
    else if(key_state == 1)
    {
        DEBUG(DEBUG_INFO, "key single tap\n");
    }
    else if(key_state == 2)
    {
        DEBUG(DEBUG_INFO, "key double tap\n");
    }
    else if(key_state == 4)
    {
        DEBUG(DEBUG_INFO, "key trible tap\n");
    }
    else
    {
        DEBUG(DEBUG_INFO, "key unknow(%x)\n",key_state);
    }

}

void press_init()
{
    EVENT_IO_Config();
    IIC_Init();
}

void press_test_start(void)
{
    press_init();
    g_test_flag = 1;
    DEBUG(DEBUG_INFO, "press start\n");
}

void press_test_stop(void)
{
    I2C_DeInit(TEST_IIC_PORT);
    g_test_flag = 0;
    DEBUG(DEBUG_INFO, "press stop\n");
}


void press_test_process(void)
{
    if(g_test_flag)
    {
        if(GPIO_ReadInputDataBit(EVENT_IO_PORT,EVENT_IO_PIN) == Bit_RESET)
        {
            press_read_state();
        }
    }
    
}
