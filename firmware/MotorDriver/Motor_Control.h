// This file includes all of the Report IDs and variables needed by
// USB_ReportHandler.c to process input and output reports,
// as well as initialization routine prototypes.


#ifndef  _BLINK_C_H_
#define  _BLINK_C_H_

/*
extern unsigned char xdata BLINK_SELECTOR;
extern unsigned char BLINK_PATTERN[];
extern unsigned int xdata BLINK_RATE;
extern unsigned char xdata BLINK_ENABLE;
extern unsigned char xdata BLINK_SELECTORUPDATE;
extern unsigned char xdata BLINK_LED1ACTIVE;
extern unsigned char xdata BLINK_LED2ACTIVE;
extern unsigned char xdata BLINK_DIMMER;
extern unsigned char xdata BLINK_DIMMER_SUCCESS;
*/

extern int xdata Position;                 // position of encoder
extern int xdata Target;

extern unsigned char xdata OUT_PACKET[];
extern unsigned char xdata IN_PACKET[];

void System_Init(void);
void Usb_Init(void);

void Motor_Target(int t);
void Motor_Position(int p);
void Motor_SetVmax(unsigned char v);
void Motor_SetV0(unsigned char v);
void Motor_Process();
void Motor_Stop(void);

// ----------------------------------------------------------------------------
// Report IDs
// ----------------------------------------------------------------------------
/*
#define OUT_BLINK_PATTERNID     0x01
#define OUT_BLINK_ENABLEID      0x02
#define OUT_BLINK_RATEID        0x03
#define IN_BLINK_SELECTORID     0x04
#define IN_BLINK_STATSID        0x05
#define FEATURE_BLINK_DIMMERID  0x06
*/

#define OUT_POSITION_ID       0x01
#define OUT_TARGET_ID         0x02
#define IN_POSITION_ID        0x03
#define IN_TARGET_ID          0x06
#define OUT_V0_ID             0x04
#define OUT_VMAX_ID           0x05
#define OUT_STOP_ID           0x07

// ----------------------------------------------------------------------------
// Report Sizes (in bytes)
// ----------------------------------------------------------------------------
/*
#define OUT_BLINK_PATTERNSize    0x08
#define OUT_BLINK_ENABLESize     0x01
#define OUT_BLINK_RATESize       0x02
#define IN_BLINK_SELECTORSize    0x01
#define IN_BLINK_STATSSIZE       0x02
#define FEATURE_BLINK_DIMMERSIZE 0x01
*/

#define OUT_POSITION_SIZE       0x02
#define OUT_TARGET_SIZE         0x02
#define IN_POSITION_SIZE        0x02
#define IN_TARGET_SIZE          0x02
#define OUT_V0_SIZE             0x01
#define OUT_VMAX_SIZE           0x01
#define OUT_STOP_SIZE           0x01

#endif