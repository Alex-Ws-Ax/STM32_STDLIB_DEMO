/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "debug.h"
#include "cmd.h"
#include "iic_test.h"
#include "press_test.h"
#include "tcae_update_test.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



REGISTER_CMD(led_on, Led_ON,null);
REGISTER_CMD(led_off, Led_OFF,null);
REGISTER_CMD(iic_test, test_iic_process,null);
REGISTER_CMD(press_test_start, press_test_start,null);
REGISTER_CMD(press_test_stop, press_test_stop,null);
REGISTER_CMD(tcae_iap_test, iap_update_process,null);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    SystemInit();
    Led_Init();
    SysTick_Init(72);
    DebugInit();
    DEBUG(DEBUG_INFO,"init over\n");
    cmd_init();
    while (1)
    {
        press_test_process();
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

