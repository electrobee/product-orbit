# Use the following 3 lines on Unix and Mac OS X:
USBFLAGS = `libusb-config --cflags`
USBLIBS =  `libusb-config --libs`

# Use the following 3 lines on Windows and comment out the 3 above:
#USBFLAGS =
#USBLIBS = -lhid -lsetupapi

INCLUDES = -I../
LIBFLAGS = -L../

CC = gcc
CFLAGS = -O -Wall -arch i386 $(USBFLAGS)
LIBS = $(LIBFLAGS) -lturntable $(USBLIBS)

SRC = example.c
OBJ = $(SRC:.c=.o)
TARGET = example.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

strip: $(TARGET)
	strip $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

distclean:
	rm -f $(OBJ)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

