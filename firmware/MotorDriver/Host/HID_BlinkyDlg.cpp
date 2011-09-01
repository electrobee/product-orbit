// HID_BlinkyDlg.cpp : implementation file
//

//-----------------------------------------------------------------------------
// HEADER FILES
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "HID_Blinky.h"
#include "HID_BlinkyDlg.h"
#include ".\HID_Blinkydlg.h"
#include "dbt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//-----------------------------------------------------------------------------
static DWORD WINAPI InterruptThreadProc(LPVOID lpParameter);

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Report IDs
//-----------------------------------------------------------------------------
#define OUT_Blink_Pattern  0x01
#define OUT_Blink_Enable   0x02
#define OUT_Blink_Rate     0x03
#define IN_Blink_Selector  0x04
#define IN_Blink_Stats     0x05
#define FEATURE_Blink_Dimmer 0x06

//-----------------------------------------------------------------------------
// Report Sizes
//-----------------------------------------------------------------------------
#define OUT_Blink_PatternSize      0x08
#define OUT_Blink_EnableSize       0x01
#define OUT_Blink_RateSize         0x02
#define IN_Blink_SelectorSize      0x01
#define IN_Blink_StatsSize         0x02
#define FEATURE_Blink_DimmerSize   0x01

//-----------------------------------------------------------------------------
// HID_BLINKY USB Identification Information
//-----------------------------------------------------------------------------
#define HID_Blinky_VID 0x10C4
#define HID_Blinky_PID 0x82CD

CHID_BlinkyDlg::CHID_BlinkyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHID_BlinkyDlg::IDD, pParent)
	, m_Selection(_T(""))
	, m_BlinkRate(100)
	, m_Stat1(_T(""))
	, m_Stat2(_T(""))
	, m_T1LED1(FALSE)
	, m_T1LED2(FALSE)
	, m_T2LED1(FALSE)
	, m_T2LED2(FALSE)
	, m_T3LED1(FALSE)
	, m_T3LED2(FALSE)
	, m_T4LED1(FALSE)
	, m_T4LED2(FALSE)
	, m_T5LED1(FALSE)
	, m_T5LED2(FALSE)
	, m_T6LED1(FALSE)
	, m_T6LED2(FALSE)
	, m_T7LED1(FALSE)
	, m_T7LED2(FALSE)
	, m_T8LED1(FALSE)
	, m_T8LED2(FALSE)
	, m_CustomBlinkingEnabled(FALSE)
	, m_RadioCustom(TRUE)
	, m_RadioStandard(FALSE)
	, m_Blink_Enable(FALSE)
	, m_RadioModeSelect(0)
	, m_Slider(0)
	, m_DeviceStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHID_BlinkyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Selection);
	DDX_Text(pDX, IDC_EDIT3, m_BlinkRate);
	DDX_Text(pDX, IDC_Stat1, m_Stat1);
	DDX_Text(pDX, IDC_Stat2, m_Stat2);
	DDX_Check(pDX, IDC_T1LED1, m_T1LED1);
	DDX_Check(pDX, IDC_T1LED2, m_T1LED2);
	DDX_Check(pDX, IDC_T2LED1, m_T2LED1);
	DDX_Check(pDX, IDC_T2LED2, m_T2LED2);
	DDX_Check(pDX, IDC_T3LED1, m_T3LED1);
	DDX_Check(pDX, IDC_T3LED2, m_T3LED2);
	DDX_Check(pDX, IDC_T4LED4, m_T4LED1);
	DDX_Check(pDX, IDC_T4LED2, m_T4LED2);
	DDX_Check(pDX, IDC_T5LED1, m_T5LED1);
	DDX_Check(pDX, IDC_T5LED2, m_T5LED2);
	DDX_Check(pDX, IDC_T6LED1, m_T6LED1);
	DDX_Check(pDX, IDC_T6LED2, m_T6LED2);
	DDX_Check(pDX, IDC_T7LED1, m_T7LED1);
	DDX_Check(pDX, IDC_T7LED2, m_T7LED2);
	DDX_Check(pDX, IDC_T8LED1, m_T8LED1);
	DDX_Check(pDX, IDC_T8LED2, m_T8LED2);
	DDX_Check(pDX, IDC_CHECK_Blink_Enable, m_Blink_Enable);
	DDX_Slider(pDX, IDC_SLIDER1, m_Slider);
	DDX_Text(pDX, IDC_EDIT2, m_DeviceStatus);
}

BEGIN_MESSAGE_MAP(CHID_BlinkyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Connect, OnBnClickedConnect)
	ON_WM_TIMER()
ON_BN_CLICKED(IDC_DISCONNECT, OnBnClickedDisconnect)
ON_BN_CLICKED(IDC_Rate, OnBnClickedRate)
ON_BN_CLICKED(IDC_GetStats, OnBnClickedGetstats)
ON_BN_CLICKED(IDC_UpdateCustomBlinking, OnBnClickedUpdatecustomblinking)
ON_BN_CLICKED(IDC_RADIOStandard, OnBnClickedRadiostandard)
ON_BN_CLICKED(IDC_RADIOCustom, OnBnClickedRadiocustom)
ON_BN_CLICKED(IDC_CHECK_Blink_Enable, OnBnClickedCheckBlinkEnable)
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
ON_WM_DEVICECHANGE()
ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
ON_BN_CLICKED(IDC_HELP_CONNECTION, OnBnClickedHelpConnection)
ON_BN_CLICKED(IDC_HELP_STANDARD_BLINK, OnBnClickedHelpStandardBlink)
ON_BN_CLICKED(IDC_IDC_HELP_CUSTOM_BLINK, OnBnClickedIdcHelpCustomBlink)
ON_BN_CLICKED(IDC_HELP_STATS, OnBnClickedHelpStats)
ON_BN_CLICKED(IDC_BLINK_CONTROLS, OnBnClickedBlinkControls)
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
// OnInitDialog
//-----------------------------------------------------------------------------
//
// Routine executes when the program begins.  It configures the dialog
// box to be notified whenever devices are attached or removed from the
// USB, initializes dialog box variables, and configures a timer to
// overflow at a rate of 100 ms.
//
BOOL CHID_BlinkyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	RegisterNotification();            // dialog box will be notified whenever
	                                   // a USB device is attached or removed
	                                   // from the USB

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			   // Set big icon
	SetIcon(m_hIcon, FALSE);		   // Set small icon

	Init_Variables();				   // Initialize on-screen variables
	                                   // and device-specific information
	SetTimer (1,100,NULL);			   // Create timer to be used to
	                                   // update on-screen information

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//-----------------------------------------------------------------------------
// OnPaint
//-----------------------------------------------------------------------------
//
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CHID_BlinkyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//-----------------------------------------------------------------------------
// OnQueryDragIcon
//-----------------------------------------------------------------------------
//
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
//
HCURSOR CHID_BlinkyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//-----------------------------------------------------------------------------
// OnBnClickedConnect
//-----------------------------------------------------------------------------
//
// Routine is called when the "Activate" button is pressed.  Routine
// checks to make sure that a device is attached, and then attempts
// to connect to an attached device.  If connected, the routine
// creates a thread that polls for received INTERRUPT IN reports.
//
void CHID_BlinkyDlg::OnBnClickedConnect()
{
	DWORD results;

	// Check to see if any HID Blinky devices are attached to USB
	results = GetNumHidDevices(HID_Blinky_VID, HID_Blinky_PID);
	// Number of attached devices with matching PID and VID is stored
	// in results
	if (results != 0)
	{
		// Attempt to connect to attached device
		results = HID_Blinky.Open(results - 1, HID_Blinky_VID, HID_Blinky_PID);

		// If successfully attached, initialize system for operation
		if (results == HID_DEVICE_SUCCESS)
		{
			// Update "Device Status" text
			m_DeviceStatus = "Connected, Active";
			UpdateData (FALSE);

			// Set Get Report and Set Report time-out values to 100 ms
			HID_Blinky.SetTimeouts (100,100);

			// Create thread that polls for INTERRUPT IN reports
			HID_RX_THREAD = CreateThread (NULL,NULL,
				InterruptThreadProc,this,0,0);

			// Configure attached and connected HID Blinky for operation
			Init_Device ();
		}
	}

}

//-----------------------------------------------------------------------------
// OnBnClickedDisconnect
//-----------------------------------------------------------------------------
//
// Routine executes whenever the "Deactivate" button is pressed.  Routine
// closes the device, terminates the thread used to poll for INTERRUPT IN
// reports, and updates the device status text.
//
void CHID_BlinkyDlg::OnBnClickedDisconnect()
{
	if (HID_Blinky.IsOpened())
	{
		if (HID_Blinky.Close() == HID_DEVICE_SUCCESS)
		{
			// Terminate the receive reports thread
			RXthreadmaycontinue = FALSE;
			TerminateThread (HID_RX_THREAD, NULL);

			// Update "Device Status" text box
			m_DeviceStatus = "Connected, Idle";
			UpdateWindow = TRUE;
		}
		else
		{
			// Do nothing
		}
	}
}

//-----------------------------------------------------------------------------
// OnBnClickedRate
//-----------------------------------------------------------------------------
//
// Routine executes whenever the "Update Rate" button is pressed.
//
void CHID_BlinkyDlg::OnBnClickedRate()
{
	Update_BlinkRate ();
}

//-----------------------------------------------------------------------------
// OnBnClickedGetstats
//-----------------------------------------------------------------------------
//
// Routine is called when the "Get Stats" button is pressed.  It calls
// the routine that retrieves the information and then outputs
// it to the screen.
//
void CHID_BlinkyDlg::OnBnClickedGetstats()
{
	unsigned char Stat1, Stat2;

	Update_Stats (&Stat1, &Stat2);     // Retrieve data from device

	m_Stat1 = "";                      // Format output strings and
	m_Stat2 = "";                      // Output their values
	m_Stat1.Format("%d %%",Stat1);
	m_Stat2.Format("%d %%",Stat2);
	UpdateWindow = TRUE;			   // Set OnTimer routine to update
	                                   // output when called
}

//-----------------------------------------------------------------------------
// OnBnClickedUpdatecustomblinking
//-----------------------------------------------------------------------------
//
// Routine is called whenever the Update Pattern button is pressed.
// If Custom blinking mode is selected, the device is configured
// to blink with Custom settings.
//
void CHID_BlinkyDlg::OnBnClickedUpdatecustomblinking()
{
	if (BlinkModeSelect == CUSTOM)
		Init_CustomBlinking ();		   // Outputs pattern to device
}

//-----------------------------------------------------------------------------
// OnBnClickedRadiostandard
//-----------------------------------------------------------------------------
//
// Routine is executed whenever the "Standard" radio button in the dialog
// box is pressed.  If the system is switching from Custom to
// Standard, the device is configured operate in Standard mode.
//
void CHID_BlinkyDlg::OnBnClickedRadiostandard()
{
	// If the device is currently configured to operate in
	// Custom mode, switch to Standard mode and reconfigure device
	if (BlinkModeSelect == CUSTOM)
	{
		BlinkModeSelect = STANDARD;
		Init_StandardBlinking ();
	}

}

//-----------------------------------------------------------------------------
// OnBnClickedRadiocustom
//-----------------------------------------------------------------------------
//
// Routine is executed whenever the "Custom" radio button in the dialog
// box is pressed.  If the system is switching from standard to
// custom, the device is configured operate in Custom mode.
//
void CHID_BlinkyDlg::OnBnClickedRadiocustom()
{
	// If the device is currently configured to operate in
	// Standard mode, switch to Custom mode and reconfigure device
	if (BlinkModeSelect == STANDARD)
	{
		BlinkModeSelect = CUSTOM;
		Init_CustomBlinking ();
	}
}

//-----------------------------------------------------------------------------
// OnBnClickedCheckBlinkEnable
//-----------------------------------------------------------------------------
//
// Routine is called whenever the "Enable Blinking" box is either checked
// or unchecked.  The routine first captures the checked/unchecked value,
// then initializes the device for either Custom or Standard mode,
// and then either enables or disables device LED blinking.
//
void CHID_BlinkyDlg::OnBnClickedCheckBlinkEnable()
{
	UpdateData(TRUE);                  // capture m_Blink_Enable value

	// Configure HID_Blinky to either custom or standar mode if
	// blinking is enabled
	if (m_Blink_Enable == TRUE)
	{
		if (BlinkModeSelect == CUSTOM)
			Init_CustomBlinking ();
		else
			Init_StandardBlinking ();
	}
	// Transmit blink enable setting to the device
	Update_BlinkEnable();
}


//-----------------------------------------------------------------------------
// OnNMReleasedcaptureSlider1
//-----------------------------------------------------------------------------
//
// Routine is called whenever the slider is released by the mouse.
// The slider value is captured by calling UpdateData, and then
// Update_BlinkDimmer is called to transmit the report containing
// the data.
//
void CHID_BlinkyDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateData(TRUE);                  // Captures slider value in m_Slider
	Update_BlinkDimmer ();             // Transmit new value to HID_Blinky
}

//-----------------------------------------------------------------------------
// OnBnClickedCancel
//-----------------------------------------------------------------------------
// Routine shuts down notification and the timers whenever the 'Cancel' button
// is pressed.
//
void CHID_BlinkyDlg::OnBnClickedCancel()
{
	UnregisterNotification();             // Close notification handle
	KillTimer(1);                         // Kill timer used to update screen
	OnCancel();                           // Call base class function
}


//-----------------------------------------------------------------------------
// NOTIFICATION ROUTINES
//-----------------------------------------------------------------------------
//
// These routines enable the dialog box to monitor and respond to USB
// devices being connected to and disconnected from the USB.

//-----------------------------------------------------------------------------
// RegisterNotification
//-----------------------------------------------------------------------------
//
// Routine registers the notification handle so that device
// connect/disconnect information  initiates a call
// to the OnDeviceChange routine.  This allows the system to
// respond to devices being connected or unexpectedly removed
// from the USB.
//
void CHID_BlinkyDlg::RegisterNotification()
{
	// Register device notification
	DEV_BROADCAST_DEVICEINTERFACE devIF = {0};
	GUID		hidGuid;

	GetHidGuid(&hidGuid);

	devIF.dbcc_size = sizeof(devIF);
	devIF.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	devIF.dbcc_classguid  = hidGuid;

	m_hNotifyDevNode = RegisterDeviceNotification(GetSafeHwnd(),
		&devIF, DEVICE_NOTIFY_WINDOW_HANDLE);
}

//-----------------------------------------------------------------------------
// UnregisterNotification
//-----------------------------------------------------------------------------
//
// Routine unregisters the notification handle so that device
// connect/disconnect information no longer initiates a call
// to the OnDeviceChange routine.
//
void CHID_BlinkyDlg::UnregisterNotification()
{
	// Unegister device notification
	if(NULL != m_hNotifyDevNode)
	{
		UnregisterDeviceNotification(m_hNotifyDevNode);
		m_hNotifyDevNode = INVALID_HANDLE_VALUE;
	}
}

//-----------------------------------------------------------------------------
// OnDeviceChange
//-----------------------------------------------------------------------------
//
// Routine is called whenever the dialog box receives notification that
// a device has either been attached to or removed from USB.  This fuction
// requires that the function RegisterNotification has been executed
// to operate properly.
//
BOOL CHID_BlinkyDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	switch(nEventType)
	{
		// A device has been inserted and is now available.
		case DBT_DEVICEARRIVAL:
			// Check to see if newly connected device has VID and PID
			// matching HID_Blinky's VID and PID
			if (GetNumHidDevices(HID_Blinky_VID,HID_Blinky_PID))
			{
				// Update Device Status text
				m_DeviceStatus = "Connected, Idle";
				UpdateWindow = TRUE;
			}
			break;

		// A device has been removed from USB.
		case DBT_DEVICEREMOVECOMPLETE:

			// Check that the HID_Blinky device was the removed device
			if (!GetNumHidDevices(HID_Blinky_VID,HID_Blinky_PID))
			{
				// Call routine that will terminate communication and
				// destroy a receive thread, if one has been created
				OnBnClickedDisconnect ();
				m_DeviceStatus = "Disconnected";
				UpdateWindow = TRUE;
			}
			break;

		default:
			break;
	}

	return TRUE;
}




//-----------------------------------------------------------------------------
// HID RECEIVE THREAD CREATION AND INTERRUPT ROUTINES
//-----------------------------------------------------------------------------
//
// These routines enable the dialog box to receive INTERRUPT IN reports
// and process received data.
//

//-----------------------------------------------------------------------------
// HIDcallback
//-----------------------------------------------------------------------------
//
// This routine is called by the thread created to receive INTERRUPT IN
// reports from the device.  The routine checks the report ID and
// processes the received data.
//
void CHID_BlinkyDlg::HIDcallback (BYTE* reportbuffer)
{
	// check report ID and determine
	switch(reportbuffer[0])
	{
		// new blink pattern has been requested, process request
		case(IN_Blink_Selector):
		{
			// if selection data shows that new pattern has been selected
			if(SelectedPattern != (reportbuffer[1] * NumPatterns / 256))
			{
				// take 8-bit potentiometer value, find selection based
				// on number of patterns
				SelectedPattern = reportbuffer[1] * NumPatterns / 256;
				Update_BlinkPattern ();
			}
		}
		break;
		default:
		break;
	}
}

//-----------------------------------------------------------------------------
// HID_InterruptGetReport
//-----------------------------------------------------------------------------
//
// Routine is called by the thread created to poll for recieved INTERRUPT
// IN reports.  The routine returns the results of the poll.  If the
// transfer is successful, data bytes will be stored inside BYTE* reportbuffer.
//
DWORD CHID_BlinkyDlg::HID_InterruptGetReport (BYTE* reportbuffer)
{
	DWORD results;
	return HID_Blinky.GetInputReport_Interrupt(reportbuffer,
		HID_Blinky.GetInputReportBufferLength(),1,&results);
}

//-----------------------------------------------------------------------------
// InterruptThreadProc
//-----------------------------------------------------------------------------
//
// Routine is called from the thread created when the "Connect" button is
// pressed and communication begins.  This routine continuously attempts
// to retrieve IN INTERRUPT reports from the bus.  If a report is received,
// the routine then calls the dialog box HID callback routine to process
// the received data.
//
static DWORD WINAPI InterruptThreadProc(LPVOID lpParameter)
{
	CHID_BlinkyDlg* dlg;
	dlg = (CHID_BlinkyDlg*)lpParameter;
	BYTE reportbuffer[256];
	DWORD status;

	dlg->RXthreadmaycontinue = TRUE;   // Variable will be cleared whenever
									   // device is disconnected

	while (dlg->RXthreadmaycontinue == TRUE)
	{
		// Attempt to retrieve a report
		status = dlg->HID_InterruptGetReport (reportbuffer);
		// If a report has been recieved, call the callback routine
		// to process the data stored in reportbuffer
		if (status == HID_DEVICE_SUCCESS)
		{
			dlg->HIDcallback (reportbuffer);
		}
	}

	return 1;
}




//-----------------------------------------------------------------------------
// OnTimer
//-----------------------------------------------------------------------------
//
// Timer is configured to call this routine every 100 ms.  If UpdateWindow
// has been set, on-screen dialog values are saved and updated.
//
void CHID_BlinkyDlg::OnTimer(UINT nIDEvent)
{
	if (UpdateWindow == TRUE)
	{
		// Save all values to temporary variables
	    CString m_Selection_Temp = m_Selection;
		CString m_Stat1_Temp = m_Stat1;
		CString m_Stat2_Temp = m_Stat2;
		CString m_DeviceStatusTemp = m_DeviceStatus;
		// Retrieve values from text boxes on screen
		UpdateData(TRUE);

		CustomBlinking_SaveState ();   // Format custom blinking pattern buffer

		// Repaint values from temporary variables
		// where needed
		m_Selection = m_Selection_Temp;
		m_Stat1 = m_Stat1_Temp;
		m_Stat2 = m_Stat2_Temp;
		m_DeviceStatus = m_DeviceStatusTemp;
		// Output values to screen
		UpdateData(FALSE);
		UpdateWindow = FALSE;		   // Clear flag
	}

	CDialog::OnTimer(nIDEvent);		   // Call base class function
}



//-----------------------------------------------------------------------------
// CustomBlinking_SaveState
//-----------------------------------------------------------------------------
//
// Routine formats the CustomBlinkingPattern buffer with the data retrieved
// from the check boxes in the dialog screen.  If a box is checked,
// that corresponding bit in the blinking pattern will be set to 1.
//
void CHID_BlinkyDlg::CustomBlinking_SaveState ()
{
	CustomBlinkingPattern[0] = 0;
	if (m_T1LED1 == TRUE)
		CustomBlinkingPattern[0] |= 0x01;
	if (m_T1LED2 == TRUE)
		CustomBlinkingPattern[0] |= 0x02;

	CustomBlinkingPattern[1] = 0;
	if (m_T2LED1 == TRUE)
		CustomBlinkingPattern[1] |= 0x01;
	if (m_T2LED2 == TRUE)
		CustomBlinkingPattern[1] |= 0x02;

	CustomBlinkingPattern[2] = 0;
	if (m_T3LED1 == TRUE)
		CustomBlinkingPattern[2] |= 0x01;
	if (m_T3LED2 == TRUE)
		CustomBlinkingPattern[2] |= 0x02;

	CustomBlinkingPattern[3] = 0;
	if (m_T4LED1 == TRUE)
		CustomBlinkingPattern[3] |= 0x01;
	if (m_T4LED2 == TRUE)
		CustomBlinkingPattern[3] |= 0x02;

	CustomBlinkingPattern[4] = 0;
	if (m_T5LED1 == TRUE)
		CustomBlinkingPattern[4] |= 0x01;
	if (m_T5LED2 == TRUE)
		CustomBlinkingPattern[4] |= 0x02;

	CustomBlinkingPattern[5] = 0;
	if (m_T6LED1 == TRUE)
		CustomBlinkingPattern[5] |= 0x01;
	if (m_T6LED2 == TRUE)
		CustomBlinkingPattern[5] |= 0x02;

	CustomBlinkingPattern[6] = 0;
	if (m_T7LED1 == TRUE)
		CustomBlinkingPattern[6] |= 0x01;
	if (m_T7LED2 == TRUE)
		CustomBlinkingPattern[6] |= 0x02;

	CustomBlinkingPattern[7] = 0;
	if (m_T8LED1 == TRUE)
		CustomBlinkingPattern[7] |= 0x01;
	if (m_T8LED2 == TRUE)
		CustomBlinkingPattern[7] |= 0x02;
}



//-----------------------------------------------------------------------------
// INITIALIZATION ROUTINES
//-----------------------------------------------------------------------------
//
// These routines initialize dialog box variables and configure the device
// to different modes of operation.
//

//-----------------------------------------------------------------------------
// Init_Device
//-----------------------------------------------------------------------------
//
// Routine is called when communication with the device is activated
// by pressing the activate button.
//
void CHID_BlinkyDlg::Init_Device()
{
	if (BlinkModeSelect == STANDARD)
		Init_StandardBlinking ();
	else if (BlinkModeSelect == CUSTOM)
		Init_CustomBlinking ();
}


//-----------------------------------------------------------------------------
// Init_CustomBlinking
//-----------------------------------------------------------------------------
//
// This routine initializes the device to a custom blinking mode.  The
// values for the blink pattern are retrieved from the dialog box,
// the blink pattern buffer is formated, and then the pattern is
// sent to the device if blinking is enabled.
//
void CHID_BlinkyDlg::Init_CustomBlinking ()
{
    UpdateData (TRUE);                 // Retrieve blink pattern check boxes

	CustomBlinking_SaveState ();       // Format custom pattern buffer

	Update_BlinkPattern ();            // Output blinking pattern
	Update_BlinkRate ();               // Output blinking rate
	Update_BlinkEnable ();             // Output current enable/disable state
}

//-----------------------------------------------------------------------------
// Init_StandardBlinking
//-----------------------------------------------------------------------------
//
// Routine is called to configure the device to blink in standard mode.
// The current blink rate is transmitted, then the selected blink
// pattern is transmitted, and then the current blink enable state
// is transmitted.
//
void CHID_BlinkyDlg::Init_StandardBlinking (void)
{
	Update_BlinkRate ();
	Update_BlinkPattern ();
	Update_BlinkEnable ();
}


//-----------------------------------------------------------------------------
// Init_Variables
//-----------------------------------------------------------------------------
//
// Routine initializes variables and predefined blinking patterns.  It is
// called when the dialog box is created.
//
void CHID_BlinkyDlg::Init_Variables (void)
{
	// This section defines each blinking pattern and its associated
	// text description.  Bit 0 of a pattern corresponds to LED1, with
	// a value of '1' indicating that the LED1 should be turned on.
	// Bit 1 corresponds to LED2.

	PatternString[0] = "50% Duty Cycle";
	Pattern[0][0] = 0x00;
	Pattern[0][0] = 0x03;
	Pattern[0][2] = 0xFF;
	Pattern[0][3] = 0xFF;
	Pattern[0][4] = 0xFF;
	Pattern[0][5] = 0xFF;
	Pattern[0][6] = 0xFF;
	Pattern[0][7] = 0xFF;

	PatternString[1] = "Alternating LEDs";
	Pattern[1][0] = 0x01;
	Pattern[1][1] = 0x02;
	Pattern[1][2] = 0xFF;
	Pattern[1][3] = 0xFF;
	Pattern[1][4] = 0xFF;
	Pattern[1][5] = 0xFF;
	Pattern[1][6] = 0xFF;
	Pattern[1][7] = 0xFF;

	PatternString[2] = "All Permutations";
	Pattern[2][0] = 0x00;
	Pattern[2][1] = 0x01;
	Pattern[2][2] = 0x02;
	Pattern[2][3] = 0x03;
	Pattern[2][4] = 0xFF;
	Pattern[2][5] = 0xFF;
	Pattern[2][6] = 0xFF;
	Pattern[2][7] = 0xFF;

	PatternString[3] = "Absolute Craziness";
	Pattern[3][0] = 0x02;
	Pattern[3][1] = 0x01;
	Pattern[3][2] = 0x03;
	Pattern[3][3] = 0x01;
	Pattern[3][4] = 0x03;
	Pattern[3][5] = 0x00;
	Pattern[3][6] = 0x03;
	Pattern[3][7] = 0x01;

	PatternString[4] = "Pulse";
	Pattern[4][0] = 0x00;
	Pattern[4][1] = 0x00;
	Pattern[4][2] = 0x00;
	Pattern[4][3] = 0x03;
	Pattern[4][4] = 0x00;
	Pattern[4][5] = 0x03;
	Pattern[4][6] = 0x00;
	Pattern[4][7] = 0x00;

	// Save total number of patterns
	NumPatterns = 5;

	// Set default pattern selection
	SelectedPattern = 0;
	m_Selection = PatternString[SelectedPattern];

	// Set initial blinking mode to STANDARD
	BlinkModeSelect = STANDARD;

	// If this call returns a non-zero value, the device is connected
	if (GetNumHidDevices(HID_Blinky_VID, HID_Blinky_PID))
	{
		// Device is connected, but communication has not been
		// "activated"
		m_DeviceStatus = "Connected, Idle";
	}
	else
	{
		// No device is currently attached to USB that matches
		// the PID and VID
		m_DeviceStatus = "Disconnected";
	}

	// Updates on-screen window during next OnTimer servicing
	UpdateWindow = TRUE;
}


//-----------------------------------------------------------------------------
// UPDATE ROUTINES
//-----------------------------------------------------------------------------
//
// These routines handle most of the low-level HID input and and output
// communication by creating buffers, constructing reports, and
// transmitting and receiving them.


//-----------------------------------------------------------------------------
// Update_BlinkRate
//-----------------------------------------------------------------------------
//
// Routine retrieves the value of m_BlinkRate from the dialog box, formats
// the data into a 16-bit value, creates an OUT_Blink_Rate report, and
// transmits that report across the control pipe.
//
void CHID_BlinkyDlg::Update_BlinkRate()
{
	unsigned char OutputRateBuffer[OUT_Blink_RateSize+1];

	UpdateData(TRUE);

	// Blinking rate must be positive
	if(m_BlinkRate < 0)
	{
		m_BlinkRate = m_BlinkRate*-1;
	}

	// Format buffer
	OutputRateBuffer[0] = OUT_Blink_Rate;
	OutputRateBuffer[1] = m_BlinkRate / 256;
	OutputRateBuffer[2] = m_BlinkRate;

	// Transmit report
	HID_Blinky.SetOutputReport_Control (OutputRateBuffer,OUT_Blink_RateSize+1);
}

//-----------------------------------------------------------------------------
// Update_BlinkPattern
//-----------------------------------------------------------------------------
//
// Routine outputs the blinking pattern to the device, and then updates
// the blinking text on-screen.
//
void CHID_BlinkyDlg::Update_BlinkPattern()
{
	unsigned char OutputBuffer[OUT_Blink_PatternSize+1];

	// If blinking in Standard mode
	if (BlinkModeSelect == STANDARD)
	{
		// Format report buffer with pattern data
		OutputBuffer [0] = OUT_Blink_Pattern;
		for (int index = 1; index < 9; index++)
			OutputBuffer[index] = Pattern[SelectedPattern][index-1];
		// Output report to device
		HID_Blinky.SetOutputReport_Interrupt (OutputBuffer, OUT_Blink_PatternSize+1);
		// Update text string
		m_Selection = PatternString[SelectedPattern];
		UpdateWindow = TRUE;           // set OnTimer routine to update string
	}
	else if (BlinkModeSelect == CUSTOM)
	{
		// Format report buffer with pattern data
		OutputBuffer [0] = OUT_Blink_Pattern;
		for (int index = 1; index < 9; index++)
			OutputBuffer[index] = CustomBlinkingPattern[index-1];
		// Output report to device
		HID_Blinky.SetOutputReport_Interrupt (OutputBuffer, OUT_Blink_PatternSize+1);
		// Update text string
		m_Selection = "???";
		UpdateWindow = TRUE;
	}
}

//-----------------------------------------------------------------------------
// Update_BlinkEnable
//-----------------------------------------------------------------------------
//
// Routine creates report buffer for the Blink Enable output report.  After
// the buffer has been constructed, the report is sent across the control
// pipe.
//
void CHID_BlinkyDlg::Update_BlinkEnable ()
{
	unsigned char OutputEnableBuffer[OUT_Blink_EnableSize+1];
	OutputEnableBuffer [0] = OUT_Blink_Enable;
	OutputEnableBuffer [1] = m_Blink_Enable;
	HID_Blinky.SetOutputReport_Control (OutputEnableBuffer, OUT_Blink_EnableSize+1);
}

//-----------------------------------------------------------------------------
// Update_BlinkDimmer
//-----------------------------------------------------------------------------
//
// Routine sends the newly selected slider value to the device by
// creating a BI-DIRECTIONAL Blink Dimmer FEATURE report, transmitting the
// report, and then retrieving the report.  Routine checks the value
// returned in the report to determine whether the device successfully
// received the new dimmer value.
//
void CHID_BlinkyDlg::Update_BlinkDimmer (void)
{
	// Create the FEATURE report buffer
	unsigned char OutputEnableBuffer[FEATURE_Blink_DimmerSize+1];
	OutputEnableBuffer [0] = FEATURE_Blink_Dimmer;

	// Convert 0-100 to 0-0xFF to be used by the device.
	OutputEnableBuffer [1] = m_Slider*255/100;

	do
	{
		// Send FEATURE report
		HID_Blinky.SetFeatureReport_Control (OutputEnableBuffer, FEATURE_Blink_DimmerSize+1);
		// Receive FEATURE report
		HID_Blinky.GetFeatureReport_Control (OutputEnableBuffer, FEATURE_Blink_DimmerSize+1);
		// Check for successful reception data
	} while (OutputEnableBuffer[1] != 1);
}

//-----------------------------------------------------------------------------
// Update_Stats
//-----------------------------------------------------------------------------
//
// Routine retrieves two bytes of data from the device by calling Get Report
// with the IN_Blink_Stats report ID, and then returns the two bytes
// received within that report.
//
void CHID_BlinkyDlg::Update_Stats (unsigned char* Stat1, unsigned char* Stat2)
{
	unsigned char OutputEnableBuffer[256];
	// For GetReport_Control, the buffer's first element must contain the
	// report ID of the report that the function is supposed to retrieve
	OutputEnableBuffer [0] = IN_Blink_Stats;
	// Retrieve buffer
	HID_Blinky.GetInputReport_Control (OutputEnableBuffer, HID_Blinky.GetOutputReportBufferLength ());

	*Stat1 = OutputEnableBuffer [1];   // Return data received from report
	*Stat2 = OutputEnableBuffer [2];
}


//-----------------------------------------------------------------------------
// HELP MESSAGE BOX ROUTINES
//-----------------------------------------------------------------------------
// The following five routines describe how modules of the demo application
// communicate with the HID device.

//-----------------------------------------------------------------------------
// OnBnClickedHelpConnection
//-----------------------------------------------------------------------------
//
// Text from message box:
// The device status window updates when:
// 1) The dialog object receives notification that a device
//    has been connected or disconnected from the bus.
// 2) The user initializes/terminates communication with the
//    HID object by clicking 'Activate' or 'Deactivate'
//
void CHID_BlinkyDlg::OnBnClickedHelpConnection()
{
	MessageBox("The device status window updates when:\n\n1) The dialog object receives notification that a device \n    has been connected or disconnected from the bus.\n\n2) The user initializes/terminates communication with the\n    HID object by clicking 'Activate' or 'Deactivate'");
}

//-----------------------------------------------------------------------------
// OnBnClickedHelpStandardBlink
//-----------------------------------------------------------------------------
//
// Text from message box:
// Selecting 'Standard' allows the device to choose from
// pre-designed blinking patterns using the potentiometer.
// INTERRUPT IN reports containing potentiometer
// postition are received by the host.
// The host then sends the selected blinking pattern to
// the device using an INTERRUPT OUT report.
//
void CHID_BlinkyDlg::OnBnClickedHelpStandardBlink()
{
	MessageBox ("Selecting 'Standard' allows the device to choose from \npre-designed blinking patterns using the potentiometer.\n\nINTERRUPT IN reports containing potentiometer \npostition are received by the host.\n\nThe host then sends the selected blinking pattern to \nthe device using an INTERRUPT OUT report.");
}

//-----------------------------------------------------------------------------
// OnBnClickedIdcHelpCustomBlink
//-----------------------------------------------------------------------------
//
// Text from message box:
// Selecting 'Custom' allows users to create their own blinking patterns.
// Users first designs the pattern using the array of check boxes.
// Clicking the 'Update Pattern' button sends an INTERRUPT OUT report
// with the custom pattern.
//
void CHID_BlinkyDlg::OnBnClickedIdcHelpCustomBlink()
{
	MessageBox ("Selecting 'Custom' allows users to create their own blinking patterns.\n\nUsers first designs the pattern using the array of check boxes.\n\nClicking the 'Update Pattern' button sends an INTERRUPT OUT report \nwith the custom pattern.");
}

//-----------------------------------------------------------------------------
// OnBnClickedHelpStats
//-----------------------------------------------------------------------------
//
// Text from message box:
// Clicking 'Get Stats' sends a Get Input Report request to the device.
// The device constructs the report and sends it back to the host.
//
void CHID_BlinkyDlg::OnBnClickedHelpStats()
{
	MessageBox ("Clicking 'Get Stats' sends a Get Input Report request to the device.\nThe device constructs the report and sends it back to the host.");
}

//-----------------------------------------------------------------------------
// OnBnClickedBlinkControls
//-----------------------------------------------------------------------------
//
// Text from the message box:
// Checking 'Blink Enabled' transmits a report using
// Set Output Report to command the device to start/stop blinking.
// Pressing 'Update Rate' sends an INTERRUPT OUT report with
// the new blink rate to the device.
// Adjusting the dimmer slider sends values to the device by calling
// Set FEATURE report.  The host then calls Get FEATURE report
// that commands the device to transmit whether or not the last
// feature report was successfully received.
//
void CHID_BlinkyDlg::OnBnClickedBlinkControls()
{
	MessageBox ("Checking 'Blink Enabled' transmits a report using Set Output Report \nto command the device to start/stop blinking.\n\nPressing 'Update Rate' sends an INTERRUPT OUT report with \nthe new blink rate to the device.\n\nAdjusting the dimmer slider sends values to the device by calling\nSet FEATURE report.  The host then calls Get FEATURE report \nthat commands the device to transmit whether or not the last\nfeature report was successfully received.");
}


//-----------------------------------------------------------------------------
// END OF FILE
//-----------------------------------------------------------------------------