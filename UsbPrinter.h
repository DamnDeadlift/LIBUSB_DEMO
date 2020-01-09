#ifndef __USBPRINTER_H__
#define __USBPRINTER_H__

#include "libusb.h"


#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x7540

uint8_t endpoint_out_address;
uint8_t endpoint_in_address;
libusb_device_handle *device_handle;
uint8_t binterface_number;
libusb_device **list;      // device list

int OpenPrinter();
int ClosePrinter();
int WriteData(uint8_t *data, int length);
int ReadData(uint8_t *data, int length);

#endif 