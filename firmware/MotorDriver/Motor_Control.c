// Contains functions that control the LEDs.

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include <C8051F320.h>
#include "Motor_Control.h"
#include "USB_ISR.h"
#include "USB_Register.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define SYSCLK             12000000    // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0x00        // Select 4x clock multiplier, for USB
#define USB_INT_OSC_DIV_2  0x10        // Full Speed
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0x00        // Select to use internal oscillator
#define SYS_EXT_OSC        0x01        // Select to use an external oscillator
#define SYS_4X_DIV_2       0x02

sbit MOTOR_STBY	= P0^2;
sbit MOTOR_PWM = P0^3;
sbit MOTOR_IN2 = P0^4;
sbit MOTOR_IN1 = P0^5;

sbit CHA = P0^6;
sbit CHB = P0^7;

#define DEBOUNCE	4

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F32x
//-----------------------------------------------------------------------------
sfr16 TMR2RL   = 0xca;                   // Timer2 reload value
sfr16 TMR2     = 0xcc;                   // Timer2 counter



// ----------------------------------------------------------------------------
// Global Variable Declarations
// ----------------------------------------------------------------------------
/*
unsigned char BLINK_PATTERN[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned int xdata BLINK_RATE;
unsigned char xdata BLINK_SELECTOR;
unsigned char xdata BLINK_ENABLE;
unsigned char xdata BLINK_SELECTORUPDATE;
unsigned char xdata BLINK_LED1ACTIVE;
unsigned char xdata BLINK_LED2ACTIVE;
unsigned char xdata BLINK_DIMMER;
unsigned char xdata BLINK_DIMMER_SUCCESS;
*/

int xdata Position = 0;                 // position of encoder
int xdata Target = 0;
unsigned char xdata V0 = 0xFF;
unsigned char xdata Vmax = 0x00;
signed char xdata Direction = 0;
signed char xdata Brake = 0;

unsigned char DebounceCount = 0;
bit Previous;

bit JustSet = 0;

// ----------------------------------------------------------------------------
// Local Function Prototypes
// ----------------------------------------------------------------------------
void Timer2_Init(void);
void Sysclk_Init(void);                
void Port_Init(void);                  
void Usb_Init(void);                   
void PCA_Init (void);
void Delay(void);

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------
// Last packet received from host
unsigned char xdata OUT_PACKET[4] = {0,0,0,0};

// Next packet to send to host
unsigned char xdata IN_PACKET[4]  = {0,0,0,0};

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR
//-----------------------------------------------------------------------------
//
// Interrupt controls the LED blinking.
//
//-----------------------------------------------------------------------------
void Timer2_ISR (void) interrupt 5
{
   if(Direction != 0 && Brake > 0) {
      Brake--;
      if(Brake == 0) {
         MOTOR_IN1 = 0;
         MOTOR_IN2 = 0;
         MOTOR_STBY = 0;
         TR2 = 0;
         Direction = 0;
      }
   }

   if(PCA0CPH0 > Vmax) {
      PCA0CPH0--;
   }

   TF2H = 0;                              // Clear Timer2 interrupt flag
}

// ----------------------------------------------------------------------------
// Initialization Routines
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// System_Init (void)
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// This top-level initialization routine calls all support routine.
//
// ----------------------------------------------------------------------------
void System_Init(void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Sysclk_Init();                      // Initialize oscillator
   Port_Init();                        // Initialize crossbar and GPIO
   Usb_Init();                         // Initialize USB0

   PCA_Init();
   Timer2_Init();
   
   Position = 0;
   Target = 0;
   V0 = 0xFF;
   Vmax = 0x00;
   Direction = 0;
   Brake = 0;
   DebounceCount = 0;
   JustSet = 0;
   
   MOTOR_IN1 = MOTOR_IN2 = 0;
   MOTOR_STBY = 0;
   MOTOR_PWM = 1;
   
   Previous = CHA;
}

//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Initialize system clock to maximum frequency.
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_INT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier
   Delay();                            // Delay for clock multiplier to begin

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;              // Select system clock
   CLKSEL |= USB_4X_CLOCK;             // Select USB clock
#endif  /* _USB_LOW_SPEED_ */
}


//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Configure the Crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   P0MDOUT   = 0x3C; 					   // STBY, PWM, IN1, IN2
   P0SKIP    = 0x07;
   XBR0     = 0x00;						    
   XBR1     = 0x41;                        // Enable Crossbar
}

//-----------------------------------------------------------------------------
// USB0_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
//-----------------------------------------------------------------------------
void Usb_Init(void)
{
   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x07);      // Enable Endpoint 0-2 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x07);      // Enable Endpoint 0-2 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset, Resume, and Suspend
                                       // interrupts
#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                     // Enable transceiver; select low speed
   POLL_WRITE_BYTE(CLKREC, 0xA0);      // Enable clock recovery; single-step
                                       // mode disabled; low speed mode enabled
#else
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x80);      // Enable clock recovery, single-step
                                       // mode disabled
#endif /* _USB_LOW_SPEED_ */

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
   EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB
                                       // Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);      // and enable suspend detection
}

//-----------------------------------------------------------------------------
// Timer_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Timer 2 reload, used to check if switch pressed on overflow and
// used for ADC continuous conversion
//
// ----------------------------------------------------------------------------
void Timer2_Init(void)
{
   TMR2CN  = 0x00;                        // Stop Timer2; Clear TF2;

   CKCON  &= ~0xF0;                       // Timer2 clocked based on T2XCLK;
   //TMR2RL  = 22;                          // Initialize reload value
   TMR2RL = 0xE000;
   
   TMR2    = 0xffff;                      // Set to reload immediately

   ET2     = 1;                           // Enable Timer2 interrupts
   //TR2     = 1;                           // Start Timer2
}

// ----------------------------------------------------------------------------
// PCA_Init
// ----------------------------------------------------------------------------
// Configures two PCA channels to act in 8-bit PWM mode.
//
void PCA_Init (void)
{
	PCA0CN    = 0x40;
	PCA0CPM0  = 0x42; // 8 bit PWM

	//PCA0CPH0 = 0xE6;
}

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
// ----------------------------------------------------------------------------
void Delay(void)
{

   int x;
   for(x = 0;x < 500;x)
      x++;

}

void Motor_Target(int t) {
	Target = t;
	if(Target != Position) {
		Brake = 0;
		Direction = Target > Position ? 1 : -1;
		MOTOR_STBY = 1;
		PCA0CPH0 = V0;
		TR2 = 1;
      JustSet = 1;
	} else {
		Direction = 0;
	}
}

void Motor_Position(int p) {
   Position = p;
	Target = p;
	Brake = 0;
}

void Motor_SetVmax(unsigned char v) {
   Vmax = (0xFF - v);
   if(PCA0CPH0 < Vmax) {
      PCA0CPH0 = Vmax;
   }
}

void Motor_SetV0(unsigned char v) {
   V0 = (0xFF - v);
}

void Motor_Stop(void) {
   Target = Position;
}

void Motor_Process(void) {
	if(CHA != Previous) {
		DebounceCount++;
		if(DebounceCount == DEBOUNCE) {
			Previous = CHA;
			DebounceCount = 0;
			Position += Direction;
		}
	} else {
		DebounceCount = 0;
	}

	if(Brake == 0) {
		if((Direction == 1) && (Position < Target)) {
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
		} else if((Direction == -1) && (Target < Position)) {
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
		} else if(Direction != 0 && !JustSet) {
			MOTOR_IN1 = 1; // brake
			MOTOR_IN2 = 1;
			Brake = 100;
		}
	}
   if(JustSet) {
      JustSet = 0;
   }
}

