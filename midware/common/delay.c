#include "Delay.h"

static u16 fac_us;   //计fac_us个数为1us - SysTick时钟频率
static u32 fac_ms;   //计fac_ms个数为1ms

/*************************************************
*函数名：     SysTick_Init
*函数功能：   SysTick定时器初始化
*输入：       SYSTICK：自己配置的系统时钟，没有配置时，默认配置是72M。 
*返回值：     无
**************************************************/
void SysTick_Init(u8 SYSTICK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //选择8分频
	fac_us = SYSTICK/8;     //因为进行了8分频
	fac_ms = (u16)fac_us*1000; 
	
}

/*************************************************
*函数名：     Delay_us
*函数功能：   微秒级延时函数
*输入：       nus：延时的微秒数
							注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
*返回值：     无
**************************************************/
void Delay_us(u32 nus)
{
	u32 temp;                     //储存CTRL寄存器的值
	SysTick->LOAD = nus*fac_us;   //设置要数到值
	SysTick->VAL  = 0x00;         //清空计数器的值
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //SysTick定时器使能
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16))); //判断是否计数到达
	
	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;  //SysTick定时器失能
	SysTick->VAL  = 0x00;         //清空计数器的值
}

/*************************************************
*函数名：      Delay_ms 
*函数功能：    毫秒级延时函数
*输入：        nus：延时nus毫秒
*返回值：      无
**************************************************/
void Delay_ms(u32 nus)
{
	u32 temp;                     //储存CTRL寄存器的值
	SysTick->LOAD = nus*fac_ms;   //设置要数到值
	SysTick->VAL  = 0x00;         //清空计数器的值
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //SysTick定时器使能
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16))); //判断是否计数到达
	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;  //SysTick定时器失能
	SysTick->VAL  = 0x00;         //清空计数器的值
}
