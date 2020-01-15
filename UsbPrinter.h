#ifndef __USBPRINTER_H__
#define __USBPRINTER_H__

#include "libusb.h"
#include <stdlib.h>


#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x7540

#define DEBUG
/*
\033 声明了转义序列的开始，然后是 [ 开始定义颜色。
后面的 1 定义了高亮显示字符。
接着是字体颜色，这里面是31，表示红色。
然后是背景颜色，这里面是40，表示黑色背景。
我们用 \033[0m 关闭转义序列， \033[0m 是终端默认颜色。
通过上面的介绍，就知道了如何输出彩色字符了。
————————————————
字色              背景              颜色
---------------------------------------
30                40              黑色
31                41              紅色
32                42              綠色
33                43              黃色
34                44              藍色
35                45              紫紅色
36                46              青藍色
37                47              白色
————————————————
原文链接：https://blog.csdn.net/u014470361/article/details/81512330
*/

#ifdef DEBUG 
#define debug(...) \
        do {                                                            \
        printf("\033[1;31;40m%s.%d: ", __func__, __LINE__);     \
        printf(__VA_ARGS__);                                    \
        printf("\033[0m");                                      \
    } while (0)                                                 \

#else
#define debug(...) 
#endif

int OpenPrinter();
int ClosePrinter();
int WriteData(uint8_t *data, int length);
int ReadData(uint8_t *data, int length);

#endif 