#include <stdio.h>
#include "libusb.h"

#define VENDOR_ID 0x12d1
#define PRODUCT_ID 0x107e

// typedef struct Usb_Device
// {
//     uint16_t vendor_id;
//     uint16_t product_id;
// }Usb_Device_t;

int main()
{
    int ret = 0;
    ssize_t device_number = 0;   // the number of usb device
    libusb_device **list; // save device list
    libusb_device *my_device; //the specified usb device
    struct libusb_device_descriptor desc;
    struct libusb_device_descriptor my_device_desc; ////the specified usb device descriptor
    struct libusb_config_descriptor *config;
    uint8_t device_class = 0;
    uint8_t endpoint_attributes = 0, endpoint_address = 0;
    uint8_t interface_number = 0;
    struct libusb_device_handle *device_handle;

    
    //struct libusb_device_descriptor *dev;

    int i = 0, j = 0 ,k = 0,l = 0;

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
        if(desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
        {
            printf("found my usb device\n");
            my_device = list[i];
            my_device_desc = desc;          
        }
    }

    //get device class and endpoint
    //device >> config >> interface >> endpoint
    for(i = 0; i < my_device_desc.bNumConfigurations; i++)
    {
       ret = libusb_get_config_descriptor(my_device,i,&config);
       if(ret != 0)
       {
           printf("get config descriptor failed\n");
           return 0;
       }
       printf("number of interface: %d\n",config->bNumInterfaces);

       for(j = 0; j < config->bNumInterfaces; j++)
       {
           for(k = 0; k < config->interface[j].num_altsetting; k++)
           {
               // get device class
               device_class = config->interface[j].altsetting[k].bInterfaceClass;
               printf("device class = %d\n",device_class);

               //get device endpoint
                endpoint_attributes = config->interface[j].altsetting[k].endpoint->bmAttributes;
                endpoint_address = config->interface[j].altsetting[k].endpoint->bEndpointAddress;
                interface_number = config->interface[j].altsetting[k].bInterfaceNumber;
                printf("bmAttributes = %d, bEndpointAddress = %d, interface number = %d\n",endpoint_attributes,endpoint_address,interface_number);
           }
       } 

    }


    device_handle = libusb_open_device_with_vid_pid(NULL, my_device_desc.idVendor, my_device_desc.idProduct);
    if(device_handle == NULL)
    {
        printf("open device failed\n");
        return 0;
    }

    //must claim the interface you wish to use before you can perform I/O on any of its endpoints.
    ret = libusb_claim_interface(device_handle, 1);
    printf("claim ret = %d\n",ret);

    //

    return 0;
}
