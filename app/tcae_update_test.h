#ifndef __TCAE_UPDATE_TEST_H
#define __TCAE_UPDATE_TEST_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>






typedef struct
{
    uint32_t    filemask;               // e.g. 0xfeef04bd固定标记，上位机搜索
    uint8_t     chipid[12];             // e.g. "TCAE31xQDI"
    uint32_t    productid;              // e.g. 0x0 通用产品
    uint32_t    version_sw;             // e.g. 0x00010000 = "1.00"
    uint32_t    datetime_sw;            // e.g. 0x20170320
} VERSION_INFO;



typedef struct
{
    uint16_t update_flag;
    uint16_t update_step;
    uint32_t filelen;
    uint32_t version;
    uint32_t datetime;
    uint32_t filecrc;
    uint8_t  chipid[12];
    uint32_t productid;
}UPDATE_FILEINFO_T;





bool iap_update_process(void);





#endif

