#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"

//1s=1000ms=1000000us
void SysTick_Init(u8 SYSTICK);  //SysTick时钟初始化函数
void Delay_us(u32 nus);          //微秒级延时函数  
void Delay_ms(u32 nus);          //毫秒级延时函数

#endif
