# Use the following 3 lines on Unix and Mac OS X:
USBFLAGS = `libusb-config --cflags`
USBLIBS =  -static `libusb-config --libs`

# Use the following 3 lines on Windows and comment out the 3 above:
#USBFLAGS =
#USBLIBS = -lhid -lsetupapi

INCLUDES =

CC = gcc
CFLAGS = -arch i386 -O -Wall $(USBFLAGS)
LIBS = $(USBLIBS)

SRC = turntable.c hiddata.c
OBJ = $(SRC:.c=.o)
TARGET = libturntable.a

all: $(TARGET)

rebuild:
	make clean && make
	make -C example clean && make -C example

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)

strip: $(TARGET)
	strip $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

distclean:
	rm -f $(OBJ)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

