
// ipc2023Dlg.h: 헤더 파일
//

#pragma once

#include "LayerManager.h"	// Added by ClassView
#include "ChatAppLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "NILayer.h"		// Added by ClassView
#include "FileLayer.h"

// CChattingDlg 대화 상자
class Cipc2023Dlg : public CDialogEx, public CBaseLayer
{
	// 생성입니다.
public:
	Cipc2023Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILETRANSFER2019_DIALOG };
#endif
	afx_msg void OnOffFileButton(BOOL bBool);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	void OnComboEnetAddr();


	DECLARE_MESSAGE_MAP()
public:

	BOOL			Receive(unsigned char* ppayload);
	inline void		SendData();
	unsigned char* MacAddrToHexInt(CString ether);

	CComboBox m_ComboEnetName;
	CString m_unDstEnetAddr;
	CString m_unSrcEnetAddr;
	CString m_stMessage;
	CString m_FileName; //파일 editControl 변수
	CIPAddressCtrl m_unDstIPAddr;
	CIPAddressCtrl m_unSrcIPAddr;
	CListBox m_ListChat;
	CProgressCtrl m_ProgressCtrl;


private:
	CLayerManager	m_LayerMgr;
	enum {
		IPC_INITIALIZING,
		IPC_READYTOSEND,
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_BROADCASTMODE,
		IPC_UNICASTMODE,
		IPC_ADDR_SET,
		IPC_ADDR_RESET,
		CFT_COMBO_SET
	};

	void			SetDlgState(int state);
	inline void		EndofProcess();

	BOOL			m_bSendReady;

	// Object Layer
	CChatAppLayer* m_ChatApp;
	CNILayer* m_NI;
	CEthernetLayer* m_Eth;
	CFileLayer* m_FileApp;

	// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;
public:
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonMsgSend();
	afx_msg void OnCbnSelchangeComboEth();
	afx_msg void OnBnClickedButtonFileAdd(); //파일 추가
	afx_msg void OnBnClickedButtonFileSend(); //파일 전송
};
