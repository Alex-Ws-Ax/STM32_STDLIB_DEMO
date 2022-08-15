#include "tcae_update_test.h"
#include "iic_test.h"
#include "debug.h"
#include "delay.h"
#include "iap_app_bin.h"

#define DEVICE_ADDR                     0xA0


#define REG_UPDATE_START               0x90
#define REG_UPDATE_FINISH              0x91
#define REG_UPDATE_FILE                0x92
#define REG_UPDATE_VERSION             0x93
#define REG_UPDATE_CHECK               0x94

#define UPGRADE_MISC_LEN                7
#define UPGRADE_START_LEN               (36+7)


#define     UPGRADE_STATUS_OK       0x55
#define     UPGRADE_STATUS_ERR      0xAA

UPDATE_FILEINFO_T update_info= {0};
uint8_t g_iic_buf[1280] = {0};

void delay_us(uint32_t delay_us)
{    
  volatile unsigned int num;
  volatile unsigned int t;
 
  
  for (num = 0; num < delay_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}
//ºÁÃë¼¶µÄÑÓÊ±
void delay_ms(uint16_t delay_ms)
{    
  volatile unsigned int num;
  for (num = 0; num < delay_ms; num++)
  {
    delay_us(1000);
  }
}


void CalculateCRCStep(const uint8_t *buffer, int length, uint16_t *pcrc)
{
    uint16_t    reg_crc = *pcrc;
    uint16_t    crcchk = 0;

    while(length--)
    {
        reg_crc ^= *buffer++;
        for(crcchk = 0; crcchk < 8; crcchk++)
        {
            if(reg_crc & 0x01)
            {
                reg_crc = (reg_crc >> 1) ^ 0xA001;
            }
            else
            {
                reg_crc = reg_crc >> 1;
            }
        }
    }

    *pcrc = reg_crc;
}

bool iap_app_file_load(const uint8_t * file,uint16_t file_len)
{
    uint32_t  signature = 0;
    uint32_t  index = 0;
    uint16_t  filecrc = 0xFFFF;
    uint16_t  stepsize =1024;
    uint16_t  pos = 0;

    while(index < file_len)
    {
        memcpy((uint8_t *)&signature,(uint8_t *)&file[index],sizeof(signature));
        if(signature == 0xFEEF04BD)
        {
            //TRACE(0,"find it \n");
            pos = index + 4;
            memcpy(update_info.chipid,(uint8_t *)&file[pos],12);
            pos = pos + 12;
            memcpy((uint8_t *)&update_info.productid,(uint8_t *)&file[pos],4);
            pos = pos + 4;
            memcpy((uint8_t *)&update_info.version,(uint8_t *)&file[pos],4);
            pos = pos + 4;
            memcpy((uint8_t *)&update_info.datetime,(uint8_t *)&file[pos],4);
            break;
        }
        index += 4;
    }
    index = 0;
    while(index < file_len)
    {
        CalculateCRCStep((uint8_t *)&file[index],stepsize,(uint16_t *)&filecrc);
        index = index + stepsize;
        if((index + stepsize) > file_len)
        {
            stepsize = file_len - index;
        }
    }

    update_info.filecrc = filecrc;

    update_info.filelen = file_len;

    update_info.update_step = 1024;

    update_info.update_flag = 1;

    DEBUG(DEBUG_INFO,"filelen=%d version=%x datetime=%x crc=%x\n",update_info.filelen,update_info.version,update_info.datetime,update_info.filecrc);

    return true;
}

void update_fileframe_send(const uint8_t *file,uint16_t size,uint8_t index)
{
    uint16_t crc = 0xFFFF;
    uint16_t pos = 0;

    g_iic_buf[pos++] = index;
    g_iic_buf[pos++] = ~index;
    g_iic_buf[pos++] = size & 0xFF;
    g_iic_buf[pos++] = (size>>8) & 0xFF;
    
    memcpy(&g_iic_buf[pos],file,size);
    CalculateCRCStep(&g_iic_buf[pos],size,&crc);
    pos = pos +size;
    memcpy(&g_iic_buf[pos],&crc,sizeof(crc));
    DEBUG(DEBUG_INFO,"framecrc=%x\n",crc);
    IIC_TxData(DEVICE_ADDR,REG_UPDATE_FILE,g_iic_buf,size+UPGRADE_MISC_LEN);
}


//update stop
void update_stop(void)
{
    I2C_WriteByte(DEVICE_ADDR,REG_UPDATE_FINISH,0);
}

bool update_frame_check(uint8_t index)
{
    uint8_t result = 0;
    uint8_t cmd[2] = {0};
    
    cmd[0] = REG_UPDATE_CHECK;
    cmd[1] = index;

    IIC_RxData(DEVICE_ADDR,&cmd,2,&result,1);
    DEBUG(DEBUG_INFO,"result=%x\n",result);

    return (UPGRADE_STATUS_OK==result)?true:false;
}



//read the device version
bool iap_device_version_check(void)
{
    VERSION_INFO device_info = {0};
    uint8_t cmd = REG_UPDATE_VERSION;
    IIC_RxData(DEVICE_ADDR,&cmd,1,g_iic_buf,sizeof(VERSION_INFO));
    memcpy(&device_info,g_iic_buf,sizeof(VERSION_INFO));

    DEBUG(DEBUG_INFO,"chipid =%s  product=%x version=%x datetime=%x\n",device_info.chipid,device_info.productid,device_info.version_sw,device_info.datetime_sw);
    
    return true;
}

//iap start
void iap_update_start(void)
{
    uint16_t crc = 0xFFFF;
    uint8_t pos = 0;

    CalculateCRCStep((uint8_t *)&update_info,sizeof(update_info),&crc);

    g_iic_buf[pos++] = 0x00;
    g_iic_buf[pos++] = 0xFF;
    g_iic_buf[pos++] = sizeof(update_info);
    g_iic_buf[pos++] = 0;
    
    memcpy(&g_iic_buf[pos],&update_info,sizeof(update_info));
    pos = pos + sizeof(update_info);
    memcpy(&g_iic_buf[pos],&crc,sizeof(crc));
    pos = pos + 2;

    IIC_TxData(DEVICE_ADDR,REG_UPDATE_START,g_iic_buf,UPGRADE_START_LEN);
}



bool iap_update_filetrans(const uint8_t * file,uint16_t file_len)
{
    uint16_t step_size = update_info.update_step,total_size = 0,index = 1;
    uint8_t retry = 5;
    bool ret = true;
    
    while(total_size < file_len)
    {
        if((total_size + step_size) > update_info.filelen)
        {
            step_size = update_info.filelen -total_size;
            DEBUG(DEBUG_INFO,"step_size=%d\n",step_size);
        }
        update_fileframe_send(&file[total_size],step_size,index);

        delay_ms(50);//wait for write flash
        if(update_frame_check(index) == true)
        {
            total_size += step_size;
            index++;
            retry = 5;
        }
        else
        {
            retry--;
            if(retry == 0)
            {
                DEBUG(DEBUG_INFO,"error occur stop transfer\n");
                ret = false;
                break;
            }
            continue;
        }
    }
    update_stop();
    return ret;
}

//bool iap_update_process(const uint8_t * iap_file,uint16_t file_len)
bool iap_update_process(void)
{
    IIC_Init();
    iap_device_version_check();

    iap_app_file_load(app_iap_bin,APP_BIN_SIZE);
    iap_update_start();
    delay_ms(2000);//wait for flash earse
    iap_update_filetrans(app_iap_bin,APP_BIN_SIZE);
    delay_ms(5000);//wait for flash earse
    iap_device_version_check();

    return true;
}

