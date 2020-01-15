#include "UsbPrinter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// uint8_t baud_hex_buffer[4][32] = {{0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x39, 0x36, 0x30, 0x30, 0x22},
//                                       {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x31, 0x39, 0x32, 0x30, 0x30, 0x22},
//                                       {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x33, 0x38, 0x34, 0x30, 0x30, 0x22},
//                                       {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x31, 0x31, 0x35, 0x32, 0x30, 0x30, 0x22}}; //baud:9600

// uint8_t baud_dec_buffer[4] = {9600, 19200, 38400, 115200};

uint8_t endpoint_out_address = 0;
uint8_t endpoint_in_address = 0;
libusb_device_handle *device_handle = NULL;
uint8_t binterface_number = 0;
libusb_device **list;      // device list

int OpenPrinter()
{
    int ret = 0;
    ssize_t device_number = 0; // the number of usb device
    
    libusb_device *printer;  //the specified usb device
    struct libusb_device_descriptor desc;
    struct libusb_device_descriptor printer_desc; ////the specified usb device descriptor
    struct libusb_config_descriptor *config;
    uint8_t device_class = 0;
    uint8_t endpoint_attributes = 0, endpoint_address = 0, endpoint_number = 0;
    
    int i = 0, j = 0, k = 0, l = 0;

    //This function must be called before calling any other libusb function.
    ret = libusb_init(NULL);
    if(ret != 0)
    {
        debug("init failed");
        return ret;
    }

    //Returns a list of USB devices currently attached to the system.
    device_number = libusb_get_device_list(NULL, &list);

    for (i = 0; i < device_number; i++)
    {
        ret = libusb_get_device_descriptor(list[i], &desc);
        if (ret != 0)
        {
            debug("get device descriptor failed\n");
            return ret;
        }
        
        if(desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
        {
            printer = list[i];
            printer_desc = desc;
            break;
        }

        if(i == device_number - 1)
        {
            debug("specified vendor id or product id not found\n");
            return -1;
        }
       
    }

    //device >> config >> interface >> endpoint
    for (i = 0; i < printer_desc.bNumConfigurations; i++)
    {
        ret = libusb_get_config_descriptor(printer, i, &config);
        if (ret != 0)
        {
            debug("get config descriptor failed\n");
            return ret;
        }
        debug("number of interface: %d\n", config->bNumInterfaces);

        for (j = 0; j < config->bNumInterfaces; j++)
        {
            for (k = 0; k < config->interface[j].num_altsetting; k++)
            {
                // get device class
                device_class = config->interface[j].altsetting[k].bInterfaceClass;
                debug("device class = %d\n", device_class);

                endpoint_number = config->interface[j].altsetting[k].bNumEndpoints;
                debug("endpoint number = %d\n", endpoint_number);

                binterface_number = config->interface[j].altsetting[k].bInterfaceNumber;

                //get device endpoint
                for (l = 0; l < endpoint_number; l++)
                {
                    endpoint_attributes = config->interface[j].altsetting[k].endpoint[l].bmAttributes;
                    endpoint_address = config->interface[j].altsetting[k].endpoint[l].bEndpointAddress;
                    debug("endpoint %d : \n", l);
                    debug("bmAttributes = %d, bEndpointAddress = %d\n", endpoint_attributes, endpoint_address);
                    debug("\n");
                    if(endpoint_address < 127)
                    {
                        endpoint_out_address = endpoint_address;
                    }
                    else
                    {
                        endpoint_in_address = endpoint_address;
                    }
                    
                }
            }
        }
    }

    // open device
    ret = libusb_open(printer, &device_handle);
    if(ret != 0)
    {
        debug("open device failed\n");
        return ret;
    }

    //must claim the interface you wish to use before you can perform I/O on any of its endpoints.
    ret = libusb_claim_interface(device_handle, binterface_number);
    if (ret != 0)
    {
        debug("claim ret = %d\n", ret);
        ret = libusb_detach_kernel_driver(device_handle, binterface_number);
        debug("detach kernel driver ret = %d\n", ret);
        if (ret != 0)
        {
            return ret;
        }

        ret = libusb_claim_interface(device_handle, 0);
        if (ret != 0)
        {
            debug("after detach kernel driver, claim interface failed again\n");
            return ret;
        }
    }

    return ret;

}

int WriteData(uint8_t *data, int length)
{
    int ret, actual_length;
    
    if(device_handle == NULL)
    {
        debug("NOT OPEN DEVICE\n");
        return -99;
    }

    ret = libusb_bulk_transfer(device_handle, endpoint_out_address, data, length, &actual_length, 1000);
    if(ret != 0)
    {
        debug("Write Data failed, ret = %d\n",ret);
        return ret;
    }
    return actual_length;
}

int ReadData(uint8_t *data, int length)
{
    int ret, actual_length;

    if(device_handle == NULL)
    {
        debug("NOT OPEN DEVICE\n");
        return -99;
    }

    //clear data
    memset(data, 0, length);

    ret = libusb_bulk_transfer(device_handle, endpoint_in_address, data, length, &actual_length, 1000);
    if(ret != 0)
    {
        debug("Read Data failed, ret = %d\n",ret);
        return ret;
    }
    return actual_length;
}

int ClosePrinter()
{
    int ret;

    libusb_free_device_list(list, 1);

    libusb_close(device_handle);

    ret = libusb_release_interface(device_handle, binterface_number);
    if (ret != 0)
    {
        debug("release interface failed\n");    
    }

    libusb_exit(NULL);

    return ret;
    
    

}