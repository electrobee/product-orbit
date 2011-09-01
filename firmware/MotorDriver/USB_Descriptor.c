// Source file for USB firmware. Includes descriptor data.

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "USB_Register.h"
#include "USB_ISR.h"
#include "USB_Descriptor.h"
#include "Motor_Control.h"

//-----------------------------------------------------------------------------
// Macro for two byte field
//-----------------------------------------------------------------------------

#define BIG_ENDIAN

#if defined BIG_ENDIAN
#define LE(x)	((((x)&0x00FF)<<8)|(((x)&0xFF00)>>8))	// convert to little endian
#else
#define LE(x)	(x)										// no conversion
#endif

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

code const device_descriptor DEVICEDESC =
{
   18,                                 // bLength
   0x01,                               // bDescriptorType
   0x1001,                             // bcdUSB
   0x00,                               // bDeviceClass
   0x00,                               // bDeviceSubClass
   0x00,                               // bDeviceProtocol
   EP0_PACKET_SIZE,                    // bMaxPacketSize0
   LE(0x16D0),                         // idVendor
   LE(0x04D4),                         // idProduct
   0x0000,                             // bcdDevice
   0x01,                               // iManufacturer
   0x02,                               // iProduct
   0x00,                               // iSerialNumber
   0x01                                // bNumConfigurations
}; //end of DEVICEDESC

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
code const hid_configuration_descriptor HIDCONFIGDESC =
{

{ // configuration_descriptor hid_configuration_descriptor
   0x09,                               // Length
   0x02,                               // Type
   0x2900,                             // Totallength (= 9+9+9+7+7)
   0x01,                               // NumInterfaces
   0x01,                               // bConfigurationValue
   0x00,                               // iConfiguration
   0x80,                               // bmAttributes
   0x20                                // MaxPower (in 2mA units)
},

{ // interface_descriptor hid_interface_descriptor
   0x09,                               // bLength
   0x04,                               // bDescriptorType
   0x00,                               // bInterfaceNumber
   0x00,                               // bAlternateSetting
   0x02,                               // bNumEndpoints
   0x03,                               // bInterfaceClass (3 = HID)
   0x00,                               // bInterfaceSubClass
   0x00,                               // bInterfaceProcotol
   0x00                                // iInterface
},

{ // class_descriptor hid_descriptor
   0x09,	                              // bLength
   0x21,	                              // bDescriptorType
   0x0101,	                           // bcdHID
   0x00,	                              // bCountryCode
   0x02,	                              // bNumDescriptors
   0x22,                               // bDescriptorType
   LE(HID_REPORT_DESCRIPTOR_SIZE)      // wItemLength (tot. len. of report
                                       // descriptor)
},

// IN endpoint (mandatory for HID)
{ // endpoint_descriptor hid_endpoint_in_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x81,                               // bEndpointAddress
   0x03,                               // bmAttributes
   EP1_PACKET_SIZE_LE,                 // MaxPacketSize (LITTLE ENDIAN)
   10                                  // bInterval
},

// OUT endpoint (optional for HID)
{ // endpoint_descriptor hid_endpoint_out_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x01,                               // bEndpointAddress
   0x03,                               // bmAttributes
   EP2_PACKET_SIZE_LE,                 // MaxPacketSize (LITTLE ENDIAN)
   10                                  // bInterval
}

};

code const hid_report_descriptor HIDREPORTDESC =
{
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x85, OUT_POSITION_ID,         // Report ID
    0x95, OUT_POSITION_SIZE,       //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    
    0x85, OUT_TARGET_ID,           // Report ID
    0x95, OUT_TARGET_SIZE,         //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    
    0x85, OUT_V0_ID,               // Report ID
    0x95, OUT_V0_SIZE,             //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    
    0x85, OUT_VMAX_ID,             // Report ID
    0x95, OUT_VMAX_SIZE,           //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    
    0x85, OUT_STOP_ID,             // Report ID
    0x95, OUT_STOP_SIZE,           //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    
    0x85, IN_POSITION_ID,          // Report ID
    0x95, IN_POSITION_SIZE,        //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    
    0x85, IN_TARGET_ID,            // Report ID
    0x95, IN_TARGET_SIZE,          //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)

/*
    0x85, OUT_BLINK_PATTERNID,       // Report ID
    0x95, OUT_BLINK_PATTERNSize,   //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)

    0x85, OUT_BLINK_ENABLEID,      // Report ID
    0x95, OUT_BLINK_ENABLESize,    //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)

    0x85, OUT_BLINK_RATEID,        // Report ID
    0x95, OUT_BLINK_RATESize,      //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)

    0x85, IN_BLINK_SELECTORID,     // Report ID
    0x95, IN_BLINK_SELECTORSize,   //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)


    0x85, IN_BLINK_STATSID,        // Report ID
    0x95, IN_BLINK_STATSSIZE,      //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)

    0x85, FEATURE_BLINK_DIMMERID,  // Report ID
    0x95, FEATURE_BLINK_DIMMERSIZE,//   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0xB1, 0x02,                    //   FEATURE (Data,Var,Abs)
*/
    0xC0                           //   end Application Collection
};

#define STR0LEN 4

code const unsigned char String0Desc [STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; // End of String0Desc

#define STR1LEN sizeof ("electrobee") * 2

code const unsigned char String1Desc [STR1LEN] =
{
   STR1LEN, 0x03,
   'e', 0,
   'l', 0,
   'e', 0,
   'c', 0,
   't', 0,
   'r', 0,
   'o', 0,
   'b', 0,
   'e', 0,
   'e', 0
}; // End of String1Desc

#define STR2LEN sizeof ("Turntable") * 2

code const unsigned char String2Desc [STR2LEN] =
{
   STR2LEN, 0x03,
   'T', 0,
   'u', 0,
   'r', 0,
   'n', 0,
   't', 0,
   'a', 0,
   'b', 0,
   'l', 0,
   'e', 0
}; // End of String2Desc

unsigned char* const STRINGDESCTABLE [] =
{
   String0Desc,
   String1Desc,
   String2Desc
};