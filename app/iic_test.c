
#include "iic_test.h"
#include "debug.h"




void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    //��ʼ�� IIC_GPIO ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //��ʼ��IIC_SCL
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin = TEST_IIC_SCL_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(TEST_IIC_IO_PORT, &GPIO_InitStruct);

    //��ʼ��IIC_SDA
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
    //ʹ��IIC
    I2C_Cmd(TEST_IIC_PORT, ENABLE);
}

void I2C_WriteByte(uint8_t device_addr, uint8_t cmd, uint8_t data)
{
    //FlagStatus bitstatus = RESET
    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //���I2C�����Ƿ�æ
    I2C_GenerateSTART(TEST_IIC_PORT,  ENABLE); //��I2C1
    //ErrorStatus status = ERROR,   ERROR�Ǹ�ö�����ͣ�ֵΪ0
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_MODE_SELECT)); //EV5,��ģʽ
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr,
                        I2C_Direction_Transmitter); //����STM32��IIC�豸�Լ��ĵ�ַ��ÿ�����ӵ�IIC�����ϵ��豸����һ���Լ��ĵ�ַ����Ϊ����Ҳ�����⡣
    while (!I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  //EV6
    I2C_SendData(TEST_IIC_PORT,  cmd);   //�Ĵ�����ַ
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //EV8,�ȴ���������
    I2C_SendData(TEST_IIC_PORT,  data);   //��������
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //�ж��Ƿ�����ɣ�
    //EV8, �ȴ������������
    I2C_GenerateSTOP(TEST_IIC_PORT,  ENABLE);  //�ر�I2C����
}


uint8_t I2C_ByteRead(uint8_t device_addr, uint8_t cmd)
{
    uint8_t data = 0xFF;
    //����Start�ź�
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //�ȴ�EV5�¼���IIC��ʼ�ź��Ѿ����� ��I2C_SR1��SBλ��1��
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //����7λ��EEPROM��ַ��
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Transmitter);
    //�ȴ�EV6�¼�����ʾ��ַ�Ѿ�����
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);

    //д��EEPROM�ڴ桰��Ԫ��ַ��
    I2C_SendData(TEST_IIC_PORT, cmd);
    //�ȴ�EV8�¼������ݼĴ���DRΪ�� ,��ַ�����Ѿ�����
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

    //���·���Start�ź�
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //�ȴ�EV5�¼�
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //����7λ��EEPROM��ַ��
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Receiver); //ע�ⷽ��
    //�ȴ�EV6�¼������գ�����ʾ��ַ�Ѿ�����
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); //ע�ⷽ��

    //������Ӧ��
    I2C_AcknowledgeConfig(TEST_IIC_PORT, DISABLE);
    //����Stop�ź�
    I2C_GenerateSTOP(TEST_IIC_PORT, ENABLE);
    //�ȴ�EV7�¼��� BUSY, MSL and RXNE flags
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);

    data = I2C_ReceiveData(TEST_IIC_PORT);
    //���³�ʼ�� Ϊ�´���׼��
    I2C_AcknowledgeConfig(TEST_IIC_PORT, ENABLE);
    return data;
}


void IIC_TxData(uint8_t device_addr, uint8_t cmd, uint8_t *tx_data, uint16_t size)
{
    uint16_t i = 0;

    //FlagStatus bitstatus = RESET
    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //���I2C�����Ƿ�æ
    I2C_GenerateSTART(TEST_IIC_PORT,  ENABLE); //��I2C1
    //ErrorStatus status = ERROR,   ERROR�Ǹ�ö�����ͣ�ֵΪ0
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_MODE_SELECT)); //EV5,��ģʽ
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr,
                        I2C_Direction_Transmitter); //����STM32��IIC�豸�Լ��ĵ�ַ��ÿ�����ӵ�IIC�����ϵ��豸����һ���Լ��ĵ�ַ����Ϊ����Ҳ�����⡣
    while (!I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  //EV6
    I2C_SendData(TEST_IIC_PORT,  cmd);   //�Ĵ�����ַ
    while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //EV8,�ȴ���������

    for (i = 0; i < size; i++)
    {
        I2C_SendData(TEST_IIC_PORT,  tx_data[i]);   //��������
        while (!I2C_CheckEvent(TEST_IIC_PORT,  I2C_EVENT_MASTER_BYTE_TRANSMITTING)); //�ж��Ƿ������
    }

    //EV8,�ȴ������������
    I2C_GenerateSTOP(TEST_IIC_PORT,  ENABLE);  //�ر�I2C����
}

void IIC_RxData(uint8_t device_addr, uint8_t* cmd, uint16_t cmd_size,uint8_t *rx_data, uint16_t data_size)
{
    uint16_t i = 0;

    //while (I2C_GetFlagStatus(TEST_IIC_PORT,  I2C_FLAG_BUSY));  //���I2C�����Ƿ�æ
    //����Start�ź�
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //�ȴ�EV5�¼���IIC��ʼ�ź��Ѿ����� ��I2C_SR1��SBλ��1��
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //����7λ��EEPROM��ַ��
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Transmitter);
    //�ȴ�EV6�¼�����ʾ��ַ�Ѿ�����
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);

    for(i=0;i<cmd_size;i++)
    {
        //д��EEPROM�ڴ桰��Ԫ��ַ��
        I2C_SendData(TEST_IIC_PORT, cmd[i]);
        //�ȴ�EV8�¼������ݼĴ���DRΪ�� ,��ַ�����Ѿ�����
        while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
    }

    //���·���Start�ź�
    I2C_GenerateSTART(TEST_IIC_PORT, ENABLE);
    //�ȴ�EV5�¼�
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

    //����7λ��EEPROM��ַ��
    I2C_Send7bitAddress(TEST_IIC_PORT, device_addr, I2C_Direction_Receiver); //ע�ⷽ��
    //�ȴ�EV6�¼������գ�����ʾ��ַ�Ѿ�����
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); //ע�ⷽ��

    for (i = 0; i < data_size; i++)
    {
        //�ȴ�EV7�¼��� BUSY, MSL and RXNE flags
        while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
        if (i == (data_size - 1))
        {
            //������Ӧ��
            I2C_AcknowledgeConfig(TEST_IIC_PORT, DISABLE);
        }
        else
        {
            I2C_AcknowledgeConfig(TEST_IIC_PORT, ENABLE);
        }
        rx_data[i] = I2C_ReceiveData(TEST_IIC_PORT);
    }
    while (I2C_CheckEvent(TEST_IIC_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
    //����Stop�ź�
    I2C_GenerateSTOP(TEST_IIC_PORT, ENABLE);

    //���³�ʼ�� Ϊ�´���׼��
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
