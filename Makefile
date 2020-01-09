TARGET = libusb_demo
CC = gcc
CFLAGS =  -g -Wall -lrt 
LIBS = -L./lib -lusb-1.0

$(TARGET): libusb_demo.c UsbPrinter.o 
	@$(CC) $(CFLAGS) $(LIBS) libusb_demo.c UsbPrinter.o  -o $(TARGET)


UsbPrinter.o: UsbPrinter.c UsbPrinter.h
	@$(CC) -c $(CFLAGS) $(LIBS) UsbPrinter.c 

clean:
	rm -rf *.o \
	rm -rf $(TARGET)