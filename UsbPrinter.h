#ifndef __USBPRINTER_H__
#define __USBPRINTER_H__

#include "libusb.h"


#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x7540



int OpenPrinter();
int ClosePrinter();
int WriteData(uint8_t *data, int length);
int ReadData(uint8_t *data, int length);

#endif 