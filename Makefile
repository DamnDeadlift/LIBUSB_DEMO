CC = gcc
CFLAGS = -g -Wall -lrt 

TARGET = libusb_test.out
LIBS = -L./lib -lusb-1.0
SRC = libusb_test.c
INC = libusb.h
#OBJ = libusb_test.o

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INC) $(LIBS) -o $(TARGET) 

clean:
	rm -rf *.o \
	rm -rf $(TARGET)