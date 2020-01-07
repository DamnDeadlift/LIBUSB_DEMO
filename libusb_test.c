#include <stdio.h>
#include <stdlib.h>
#include "libusb.h"

#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x7540

// typedef struct Usb_Device
// {
//     uint16_t vendor_id;
//     uint16_t product_id;
// }Usb_Device_t;

int main()
{
    int ret = 0;
    ssize_t device_number = 0; // the number of usb device
    libusb_device **list;      // device list
    libusb_device *my_device;  //the specified usb device
    struct libusb_device_descriptor desc;
    struct libusb_device_descriptor my_device_desc; ////the specified usb device descriptor
    struct libusb_config_descriptor *config;
    uint8_t device_class = 0;
    uint8_t endpoint_attributes = 0, endpoint_address = 0, endpoint_number = 0;
    uint8_t interface_number = 0;
    struct libusb_device_handle *device_handle;

    //0x1b,0x6d ： 半切；
    // 0x10,0x04 n ：实时状态传送
    // 0x1b, 0x69 ：全切
    uint8_t send_buffer[32] = {0x10, 0x04, 1};
    uint8_t baud_hex_buffer[4][32] = {{0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x39, 0x36, 0x30, 0x30, 0x22},
                                      {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x31, 0x39, 0x32, 0x30, 0x30, 0x22},
                                      {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x33, 0x38, 0x34, 0x30, 0x30, 0x22},
                                      {0x1d, 0x28, 0x45, 0x08, 0x00, 0x0b, 0x01, 0x22, 0x31, 0x31, 0x35, 0x32, 0x30, 0x30, 0x22}}; //baud:9600

    uint8_t baud_dec_buffer[4] = {9600, 19200, 38400, 115200};
    int actual_length = 0;

    //struct libusb_device_descriptor *dev;

    int i = 0, j = 0, k = 0, l = 0;

    //This function must be called before calling any other libusb function.
    ret = libusb_init(NULL); //a default context will be created
    if (ret != 0)
    {
        printf("init failed\n");
        return 0;
    }

    //Returns a list of USB devices currently attached to the system.
    device_number = libusb_get_device_list(NULL, &list); //Must be later freed with libusb_free_device_list().
    printf("found %d usb device\n", device_number);

    //Get the USB device descriptor.
    for (i = 0; i < device_number; i++)
    {
        ret = libusb_get_device_descriptor(list[i], &desc);
        if (ret != 0)
        {
            printf("get device descriptor failed\n");
            return 0;
        }
        printf("idVendor = %#.4x, idProduct = %#.4x, bNumConfigurations = %d \n", desc.idVendor, desc.idProduct, desc.bNumConfigurations);

        //find specified usb device
        if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
        {
            printf("found my usb device\n");
            my_device = list[i];
            my_device_desc = desc;
        }
    }

    //get device class and endpoint
    //device >> config >> interface >> endpoint
    for (i = 0; i < my_device_desc.bNumConfigurations; i++)
    {
        ret = libusb_get_config_descriptor(my_device, i, &config);
        if (ret != 0)
        {
            printf("get config descriptor failed\n");
            return 0;
        }
        printf("number of interface: %d\n", config->bNumInterfaces);

        for (j = 0; j < config->bNumInterfaces; j++)
        {
            for (k = 0; k < config->interface[j].num_altsetting; k++)
            {
                // get device class
                device_class = config->interface[j].altsetting[k].bInterfaceClass;
                printf("device class = %d\n", device_class);

                endpoint_number = config->interface[j].altsetting[k].bNumEndpoints;
                printf("endpoint number = %d\n", endpoint_number);

                interface_number = config->interface[j].altsetting[k].bInterfaceNumber;

                //get device endpoint
                for (l = 0; l < endpoint_number; l++)
                {
                    endpoint_attributes = config->interface[j].altsetting[k].endpoint[l].bmAttributes;
                    endpoint_address = config->interface[j].altsetting[k].endpoint[l].bEndpointAddress;
                    printf("endpoint %d : \n", l);
                    printf("bmAttributes = %d, bEndpointAddress = %d\n", endpoint_attributes, endpoint_address);
                }
            }
        }
    }

    device_handle = libusb_open_device_with_vid_pid(NULL, my_device_desc.idVendor, my_device_desc.idProduct);
    if (device_handle == NULL)
    {
        printf("open device failed\n");
        return 0;
    }

    //must claim the interface you wish to use before you can perform I/O on any of its endpoints.
    ret = libusb_claim_interface(device_handle, 0);
    if (ret < 0)
    {
        printf("claim ret = %d\n", ret);
        ret = libusb_detach_kernel_driver(device_handle, interface_number);
        printf("detach kernel driver ret = %d\n", ret);
        if (ret < 0)
        {
            return -1;
        }

        ret = libusb_claim_interface(device_handle, 0);
        if (ret < 0)
        {
            printf("after detach kernel driver, claim interface failed again\n");
            return -1;
        }
    }

    ret = libusb_bulk_transfer(device_handle, 3, baud_buffer, 32, &actual_length, 200);
    printf("transfer ret = %d, actual_length = %d\n", ret, actual_length);

    ret = libusb_bulk_transfer(device_handle, 3, send_buffer, 32, &actual_length, 200);
    printf("transfer ret = %d\n", ret);

    memset(send_buffer, 0, 32);

    ret = libusb_bulk_transfer(device_handle, 0x81, &send_buffer, 32, &actual_length, 500);
    printf("receive ret = %d\n", ret);

    for (i = 0; i < actual_length; i++)
    {
        printf("%d", send_buffer[i]);
    }
    printf("\n");

    //

    return 0;
}
