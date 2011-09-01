// This application will communicate with a PC across the USB interface.
// The device will appear to be a mouse, and will manipulate the cursor
// on screen.

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include <C8051F320.h>
#include "USB_Register.h"
#include "Motor_Control.h"
#include "USB_ISR.h"
#include "USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{

   System_Init ();
   Usb_Init ();

   EA = 1;

   while (1)
   {
      /*
      if (BLINK_SELECTORUPDATE)
      {
         BLINK_SELECTORUPDATE = 0;
         SendPacket (IN_BLINK_SELECTORID);
      }
      */
      Motor_Process();
   }
}

