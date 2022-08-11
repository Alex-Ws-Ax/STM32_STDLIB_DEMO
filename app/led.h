#ifndef __LED_H_
#define __LED_H_



#define     LED_RCC_PORT    RCC_APB2Periph_GPIOC
#define     LED_PORT        GPIOC
#define     LED_PIN         GPIO_Pin_13




#define     LED_ON()        GPIO_SetBits(LED_PORT,LED_PIN)
#define     LED_OFF()       GPIO_ResetBits(LED_PORT,LED_PIN)


void Led_Init(void);
void Led_ON(void);
void Led_OFF(void);




#endif
