// HID_BlinkyDlg.h : header file
//
#include "HIDDevice.h"
#include "afxwin.h"
#pragma once


// CHID_BlinkyDlg dialog
class CHID_BlinkyDlg : public CDialog
{
// Construction
public:
	CHID_BlinkyDlg(CWnd* pParent = NULL);	// standard constructor

	DWORD HID_InterruptGetReport (BYTE*);
	void HIDcallback (BYTE* reportbuffer);
	HANDLE HID_RX_THREAD;
	int SelectedPattern;
// Dialog Data
	enum { IDD = IDD_HID_Blinky_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CHIDDevice HID_Blinky;
	HANDLE m_hNotifyDevNode;
	void Init_Device (void);
// Implementation
protected:
	bool UpdateWindow;
	void Init_Variables (void);
	int NumPatterns;
	bool Blink_Enable;
	CString PatternString[10];
	unsigned char Pattern[10][8];
	unsigned char CustomBlinkingPattern[8];
	void CustomBlinking_SaveState (void);
	void Init_CustomBlinking (void);
	void Init_StandardBlinking (void);
	void Update_BlinkRate (void);
	unsigned char BlinkModeSelect;
	void Update_BlinkPattern (void);
	void Update_BlinkDimmer (void);
	void RegisterNotification (void);
	void UnregisterNotification(void);
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	void Update_BlinkEnable (void);
	void Update_Stats (unsigned char* Stat1, unsigned char* Stat2);
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool RXthreadmaycontinue;
	afx_msg void OnBnClickedConnect();
	afx_msg void OnTimer(UINT nIDEvent);
	// selected blink pattern name
	CString m_Selection;
	int m_BlinkRate;
	afx_msg void OnBnClickedDisconnect();
	CString m_Stat1;
	CString m_Stat2;
	afx_msg void OnBnClickedRate();
	afx_msg void OnBnClickedGetstats();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck19();
	afx_msg void OnBnClickedCheck3();
	BOOL m_T1LED1;
	afx_msg void OnBnClickedUpdatecustomblinking();
	BOOL m_T1LED2;
	BOOL m_T2LED1;
	BOOL m_T2LED2;
	BOOL m_T3LED1;
	BOOL m_T3LED2;
	BOOL m_T4LED1;
	BOOL m_T4LED2;
	BOOL m_T5LED1;
	BOOL m_T5LED2;
	BOOL m_T6LED1;
	BOOL m_T6LED2;
	BOOL m_T7LED1;
	BOOL m_T7LED2;
	BOOL m_T8LED1;
	BOOL m_T8LED2;
	BOOL m_CustomBlinkingEnabled;
	afx_msg void OnBnClickedRadiostandard();
	bool m_RadioCustom;
	bool m_RadioStandard;
	afx_msg void OnBnClickedRadiocustom();
	afx_msg void OnBnClickedRadioenable();
	afx_msg void OnBnClickedCheckBlinkEnable();
	BOOL m_Blink_Enable;
	int m_RadioModeSelect;
	int m_Slider;
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	CString m_DeviceStatus;
	afx_msg void OnBnClickedHelpConnection();
	afx_msg void OnBnClickedHelpStandardBlink();
	afx_msg void OnBnClickedIdcHelpCustomBlink();
	afx_msg void OnBnClickedHelpStats();
	afx_msg void OnBnClickedBlinkControls();
};

#define STANDARD 1
#define CUSTOM 0

//-----------------------------------------------------------------------------
// END OF FILE
//-----------------------------------------------------------------------------