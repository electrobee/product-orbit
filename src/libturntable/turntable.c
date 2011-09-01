#include "turntable.h"

/******************************************************************************
	Private functions
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiddata.h"

#define USB_CFG_VENDOR_ID       0x16D0
#define USB_CFG_DEVICE_ID       0x04D4
#define USB_CFG_VENDOR_NAME     "electrobee"
#define USB_CFG_DEVICE_NAME     "Turntable"

/* ------------------------------------------------------------------------- */

tt_error_t error = TT_OK;
usbDevice_t *tt_dev = NULL;

/* ------------------------------------------------------------------------- */

static char *usbErrorMessage(int errCode)
{
static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:      return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
        case USBOPEN_ERR_IO:          return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

/* ------------------------------------------------------------------------- */

static usbDevice_t  *openDevice(void) {
	usbDevice_t     *dev = NULL;
	int             err;

    if((err = usbhidOpenDevice(&dev, USB_CFG_VENDOR_ID, USB_CFG_VENDOR_NAME, USB_CFG_DEVICE_ID, USB_CFG_DEVICE_NAME, 0)) != 0){
        //fprintf(stderr, "error finding %s: %s\n", USB_CFG_DEVICE_NAME, usbErrorMessage(err));
        return NULL;
    }
    return dev;
}

/* ------------------------------------------------------------------------- */

void ttWrite(char * buffer, int size) {
  int err;

  if(!tt_dev) {
    if(ttOpen() != TT_OK) {
      error = TT_NOT_CONNECTED;
      return;
    }
  }

  if((err = usbhidSetReport(tt_dev, buffer, size)) != 0) {
		//fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
		error = TT_NOT_RESPONDING;
    ttClose();
	}
}

int ttRead(char * buffer, int size) {
	int err;

  if(!tt_dev) {
    if(ttOpen() != TT_OK) {
      error = TT_NOT_CONNECTED;
      return 0;
    }
  }

  if((err = usbhidGetReport(tt_dev, buffer[0], buffer, &size)) != 0) {
		//fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
		error = TT_NOT_RESPONDING;
    ttClose();
	}

	return size;
}

/******************************************************************************
	Public functions
******************************************************************************/

tt_error_t ttOpen() {
  error = TT_OK;

  if(tt_dev) {
    ttClose();
  }

  if((tt_dev = openDevice()) == NULL) {
    error = TT_NOT_CONNECTED;
  }

  return error;
}

void ttClose() {
  if(tt_dev) {
    usbhidCloseDevice(tt_dev);
  }
}

tt_error_t ttError() {
	return error;
}

void ttClearError() {
	error = TT_OK;
}

int16_t ttGetPosition() {
	char buffer[] = {0x03, 0, 0};
	ttRead(buffer, sizeof(buffer));
	return *((int16_t*) &buffer[1]);
}

int16_t ttGetTarget() {
  char buffer[] = {0x06, 0, 0};
  ttRead(buffer, sizeof(buffer));
  return *((int16_t*) &buffer[1]);
}

void ttSetPosition(int16_t position) {
	char buffer[] = {0x01, 0, 0};
	*((int16_t*) &buffer[1]) = position;
	ttWrite(buffer, sizeof(buffer));
}

void ttSetTarget(int16_t target) {
	char buffer[] = {0x02, 0, 0};
	*((int16_t*) &buffer[1]) = target;
	ttWrite(buffer, sizeof(buffer));
}

void ttSetV0(uint8_t v0) {
	char buffer[] = {0x04, 0};
	buffer[1] = v0;
	ttWrite(buffer, sizeof(buffer));
}

void ttSetVmax(uint8_t vmax) {
	char buffer[] = {0x05, 0};
	buffer[1] = vmax;
	ttWrite(buffer, sizeof(buffer));
}

void ttStop() {
  char buffer[] = {0x07, 0};
  ttWrite(buffer, sizeof(buffer));
}
