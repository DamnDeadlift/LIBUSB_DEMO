#ifndef __USBPRINTER_H__
#define __USBPRINTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "libusb.h"

//调试信息宏定义开关
//#define USB_DEBUG
#ifdef USB_DEBUG
#define debug_print(...) \
        do {                                                            \
        printf("\033[1;31;47mfunc:%s,line:%d: ", __func__, __LINE__);     \
        printf(__VA_ARGS__);                                    \
        printf("\033[0m");                                      \
    } while (0)                                                 \

#else
#define debug_print(...)
#endif

//the specified vendor id and product id
#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x7540

int OpenPrinter();
int ClosePrinter();
int WriteData(uint8_t *data, int length);
int ReadData(uint8_t *data, int length);

#ifdef __cplusplus
}
#endif

#endif 
