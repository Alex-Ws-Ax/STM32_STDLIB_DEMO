
#include "iic_test.h"
#include "debug.h"




void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    //初始化 IIC_GPIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //初始化IIC_SCL
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin = TEST_IIC_SCL_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(TEST_IIC_IO_PORT, &GPIO_InitStruct);

    //初始化IIC_SDA
    GPIO_InitStruct.GPIO_Pin = TEST_IIC_SDA_PIN;

    GPIO_Init(TEST_IIC_IO_PORT, &GPIO_InitStruct);
}


void IIC_Init(void)
{
    I2C_InitTypeDef iic_init_t = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    iic_init_t.I2C_Ack = I2C_Ack_Enable;
    iic_init_t.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    iic_init_t.I2C_ClockSpeed = TEST_IIC_SPEED;
    iic_init_t.I2C_DutyCycle = I2C_DutyCycle_2;
    iic_init_t.I2C_Mode = I2C_Mode_I2C;
    iic_init_t.I2C_OwnAddress1 = TEST_OWN_ADDR;

    I2C_GPIO_Config();
    I2C_Init(TEST_IIC_PORT, &iic_init_t);
    //使能IIC
    I2C_Cmd(TEST_IIC_PORT, ENABLE);
}

void I2C_WriteByte(uint8_t device_addr, uint8_t cmd, uint8_t data)
{
    //FlagStatus bitstatus = RESET
    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //检查I2C总线是否繁忙
    I2C_GenerateSTART(TEST_IIC_PORT,  ENABLE); //打开I2C1
    //ErrorStatus status = ERROR,   ERROR是个枚举类型，值为0
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_MODE_SELECT)); //EV5,主模式
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr,
                        I2C_Direction_Transmitter); //配置STM32的IIC设备自己的地址，每个连接到IIC总线上的设备都有一个自己的地址，作为主机也不例外。
    while (!I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  //EV6
    I2C_SendData(TEST_IIC_PORT,  cmd);   //寄存器地址
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //EV8,等待发送数据
    I2C_SendData(TEST_IIC_PORT,  data);   //发送数据
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //判断是否发送完成，
    //EV8, 等待发送数据完成
    I2C_GenerateSTOP(TEST_IIC_PORT,  ENABLE);  //关闭I2C总线
}


uint8_t I2C_ByteRead(uint8_t device_addr, uint8_t cmd)
{
    uint8_t data = 0xFF;
    //发送Start信号
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //等待EV5事件：IIC开始信号已经发出 （I2C_SR1内SB位置1）
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //发送7位“EEPROM地址”
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Transmitter);
    //等待EV6事件：表示地址已经发送
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);

    //写入EEPROM内存“单元地址”
    I2C_SendData(TEST_IIC_PORT, cmd);
    //等待EV8事件：数据寄存器DR为空 ,地址数据已经发送
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

    //重新发送Start信号
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //等待EV5事件
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //发送7位“EEPROM地址”
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Receiver); //注意方向
    //等待EV6事件（接收）：表示地址已经发送
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); //注意方向

    //产生非应答
    I2C_AcknowledgeConfig(TEST_IIC_PORT, DISABLE);
    //发送Stop信号
    I2C_GenerateSTOP(TEST_IIC_PORT, ENABLE);
    //等待EV7事件， BUSY, MSL and RXNE flags
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);

    data = I2C_ReceiveData(TEST_IIC_PORT);
    //重新初始化 为下次做准备
    I2C_AcknowledgeConfig(TEST_IIC_PORT, ENABLE);
    return data;
}


void IIC_TxData(uint8_t device_addr, uint8_t cmd, uint8_t *tx_data, uint16_t size)
{
    uint16_t i = 0;

    //FlagStatus bitstatus = RESET
    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //检查I2C总线是否繁忙
    I2C_GenerateSTART(TEST_IIC_PORT,  ENABLE); //打开I2C1
    //ErrorStatus status = ERROR,   ERROR是个枚举类型，值为0
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_MODE_SELECT)); //EV5,主模式
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr,
                        I2C_Direction_Transmitter); //配置STM32的IIC设备自己的地址，每个连接到IIC总线上的设备都有一个自己的地址，作为主机也不例外。
    while (!I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  //EV6
    I2C_SendData(TEST_IIC_PORT,  cmd);   //寄存器地址
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //EV8,等待发送数据

    for (i = 0; i < size; i++)
    {
        I2C_SendData(TEST_IIC_PORT,  tx_data[i]);   //发送数据
        while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //判断是否发送完成
    }

    //EV8,等待发送数据完成
    I2C_GenerateSTOP(TEST_IIC_PORT,  ENABLE);  //关闭I2C总线
}

void IIC_RxData(uint8_t device_addr, uint8_t* cmd, uint16_t cmd_size,uint8_t *rx_data, uint16_t data_size)
{
    uint16_t i = 0;

    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //检查I2C总线是否繁忙
    //发送Start信号
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //等待EV5事件：IIC开始信号已经发出 （I2C_SR1内SB位置1）
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //发送7位“EEPROM地址”
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Transmitter);
    //等待EV6事件：表示地址已经发送
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);

    for(i=0;i<cmd_size;i++)
    {
        //写入EEPROM内存“单元地址”
        I2C_SendData(TEST_IIC_PORT, cmd[i]);
        //等待EV8事件：数据寄存器DR为空 ,地址数据已经发送
        while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
    }

    //重新发送Start信号
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //等待EV5事件
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //发送7位“EEPROM地址”
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Receiver); //注意方向
    //等待EV6事件（接收）：表示地址已经发送
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); //注意方向

    for (i = 0; i < data_size; i++)
    {
        //等待EV7事件， BUSY, MSL and RXNE flags
        while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
        if (i == (data_size - 1))
        {
            //产生非应答
            I2C_AcknowledgeConfig(TEST_IIC_PORT, DISABLE);
        }
        else
        {
            I2C_AcknowledgeConfig(TEST_IIC_PORT, ENABLE);
        }
        rx_data[i] = I2C_ReceiveData(TEST_IIC_PORT);
    }
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
    //发送Stop信号
    I2C_GenerateSTOP(TEST_IIC_PORT, ENABLE);

    //重新初始化 为下次做准备
    I2C_AcknowledgeConfig(TEST_IIC_PORT, ENABLE);
}

void test_iic_process(void)
{
    uint8_t tx_data[5] = {0x12, 0x13, 0x14, 0x15, 0x16};
    uint8_t rx_data[5] = {0};
    uint8_t cmd = 0x00;

    IIC_Init();

    IIC_TxData(0xA0, 0x00, tx_data, 5);
    IIC_RxData(0xA0, &cmd,1, rx_data, 5);
    DEBUG(DEBUG_INFO, "rx_data[0]=%x rx_data[1]=%x rx_data[2]=%x rx_data[3]=%x rx_data[4]=%x\n",
          rx_data[0], rx_data[1], rx_data[2], rx_data[3], rx_data[4]);
}
