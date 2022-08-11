#ifndef __DEBUG_H_
#define __DEBUG_H_
#include <stdio.h>


#define DEBUG_EN       1

#define DEBUG_NONE 0x00
#define DEBUG_INFO 0x01
#define DEBUG_WARN 0x02
#define DEBUG_ERR  0x04
#define DEBUG_ALL  0xFF


#if DEBUG_EN
#define DEBUG(flag, fmt, ...)       do\
                                    {\
                                        if(debug_level & flag)\
                                        {\
                                            printf(fmt, ##__VA_ARGS__);\
                                        }\
                                    }while(0)
#else
#define DEBUG(fmt, ...)
#endif








void DebugInit(void);
extern unsigned short debug_level;










#endif
