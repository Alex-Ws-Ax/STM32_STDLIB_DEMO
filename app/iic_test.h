#ifndef __IIC_TEST_H
#define __IIC_TEST_H
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"


#define     TEST_IIC_PORT       I2C1
#define     TEST_OWN_ADDR       0x32
#define     TEST_IIC_SPEED      20000

#define     TEST_IIC_IO_PORT    GPIOB
#define     TEST_IIC_SDA_PIN    GPIO_Pin_7
#define     TEST_IIC_SCL_PIN    GPIO_Pin_6

#define     TEST_IIC_TIMEOUT    1000000



void test_iic_process(void);



void IIC_Init(void);

uint8_t I2C_ByteRead(uint8_t device_addr, uint8_t cmd);
void I2C_WriteByte(uint8_t device_addr, uint8_t cmd, uint8_t data);

void IIC_RxData(uint8_t device_addr, uint8_t* cmd, uint16_t cmd_size,uint8_t *rx_data, uint16_t data_size);



void IIC_TxData(uint8_t device_addr, uint8_t cmd, uint8_t *tx_data, uint16_t size);

#endif

