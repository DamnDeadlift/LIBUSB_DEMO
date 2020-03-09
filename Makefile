TARGET = libusb_demo
CC = gcc
CFLAGS =  -g -Wall -lrt -std=c99
LIBS = -L./lib -lusb-1.0

$(TARGET): libusb_demo.c UsbPrinter.o 
	@$(CC) $(CFLAGS) $(LIBS) libusb_demo.c UsbPrinter.o  -o $(TARGET)
	@echo "编译完成！"


UsbPrinter.o: UsbPrinter.c UsbPrinter.h
	@$(CC) -c $(CFLAGS) UsbPrinter.c 

clean:
	@rm -rf *.o \
	@rm -rf $(TARGET) 
	@echo "清理完毕！"