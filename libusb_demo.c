#include <stdio.h>
#include "UsbPrinter.h"
// #include <stdlib.h>
int main()
{
    int ret = -1;

    //0x1b,0x6d ： 半切；
    // 0x10,0x04 n ：实时状态传送
    // 0x1b, 0x69 ：全切
    uint8_t buf[4] = {0x10, 0x04};
    
    ret = OpenPrinter();
    if(ret != 0)
    {
        printf("open printer failed\n");
        return ret;
    }

    ret = WriteData(buf, 4);
    if(ret < 0)
    {
        printf("write data failed\n");
        ret = ClosePrinter();
        printf("close printer ret = %d\n",ret);
        return ret;
    }
    printf("write %d data \n", ret);

    ret = ReadData(buf, 4);
    if (ret >= 0)
    {
        printf("read %d data \n", ret);

        for (int i = 0; i < ret; i++)
        {
            printf("%d", buf[i]);
        }
        printf("\n");
        
    }
    else
    {
        printf("read data failed,ret = %d\n",ret);
    }
    
    ret = ClosePrinter();
    printf("close printer ret = %d\n",ret);
    


    return 0;
}
