
// Filetransfer2019Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ipc2023.h"
#include "ipc2023Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



// Cipc2023Dlg 대화 상자



Cipc2023Dlg::Cipc2023Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILETRANSFER2019_DIALOG, pParent)
	, CBaseLayer("ChatDlg")
	, m_bSendReady(FALSE)
{

	m_stMessage = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Protocol Layer Setting
	m_LayerMgr.AddLayer(this);
	m_LayerMgr.AddLayer(new CNILayer("NI"));
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CChatAppLayer("ChatApp"));
	m_LayerMgr.AddLayer(new CFileLayer("FileApp"));

	// 레이어 연결
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ChatApp ( *ChatDlg ) *FileApp ) )"); //레이어 계층

	m_ChatApp = (CChatAppLayer*)mp_UnderLayer;
	m_Eth = (CEthernetLayer*)m_LayerMgr.GetLayer("Ethernet");
	m_NI = (CNILayer*)m_LayerMgr.GetLayer("NI");
	m_FileApp = (CFileLayer*)m_LayerMgr.GetLayer("FileApp");
	//Protocol Layer Setting
}

void Cipc2023Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_Eth, m_ComboEnetName);
	DDX_Text(pDX, IDC_EDIT_Eth_DST, m_unDstEnetAddr);
	DDX_Text(pDX, IDC_EDIT_Eth_SRC, m_unSrcEnetAddr);
	DDX_Text(pDX, IDC_FILE_NAME, m_FileName); //파일 editControl
	DDX_Text(pDX, IDC_EDIT_MSG, m_stMessage);
	DDX_Control(pDX, IDC_LIST_ListChat, m_ListChat);
}

BEGIN_MESSAGE_MAP(Cipc2023Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Setting, &Cipc2023Dlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_MSG_Send, &Cipc2023Dlg::OnBnClickedButtonMsgSend)
	ON_CBN_SELCHANGE(IDC_COMBO_Eth, &Cipc2023Dlg::OnCbnSelchangeComboEth)
	ON_BN_CLICKED(IDC_FILE_ADD, &Cipc2023Dlg::OnBnClickedButtonFileAdd)
	ON_BN_CLICKED(IDC_FILE_SEND, &Cipc2023Dlg::OnBnClickedButtonFileSend)
END_MESSAGE_MAP()


// Cipc2023Dlg 메시지 처리기

BOOL Cipc2023Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDlgState(IPC_INITIALIZING);
	SetDlgState(CFT_COMBO_SET);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Cipc2023Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Cipc2023Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Cipc2023Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cipc2023Dlg::OnBnClickedButtonMsgSend()  // Send(&S)버튼 눌렀을 때 처리해야 하는 작업
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (!m_stMessage.IsEmpty())
	{
		SetTimer(1, 3000, NULL);

		SendData(); //Data 전송

		m_stMessage = "";

		(CEdit*)GetDlgItem(IDC_EDIT_MSG)->SetFocus();
	}
	UpdateData(FALSE);
}


void Cipc2023Dlg::OnBnClickedButtonSetting() //설정 버튼 클릭 시, ip주소들을 받아옴
{
	UpdateData(TRUE);


	if (!m_unDstEnetAddr ||
		!m_unSrcEnetAddr)
	{
		MessageBox(_T("주소를 설정 오류발생"),
			_T("경고"),
			MB_OK | MB_ICONSTOP);

		return;
	}

	if (m_bSendReady) { //송신 준비 완료 상태
		SetDlgState(IPC_ADDR_RESET);
		SetDlgState(IPC_INITIALIZING);
	}
	else {


		int nIndex = m_ComboEnetName.GetCurSel();
		m_NI->SetNetworkAdapterNumber(nIndex); // 선택된 네트워크 어댑터 번호를 설정

		CString inNicName = m_NI->GetNetworkAdapter(nIndex)->name; //어댑터 이름 가져오기

		CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_Eth_SRC);

		pSrcEdit->SetWindowTextA(m_NI->GetNICardAddress((char*)inNicName.GetString()));

		//선택한 adapter에 맞는 ethernet주소


		// 이더넷 헤더의 출발지 및 목적지 주소 설정
		m_Eth->SetEnetHeaderSrcAddress(MacAddrToHexInt(m_unSrcEnetAddr)); //Source Address
		m_Eth->SetEnetHeaderDstAddress(MacAddrToHexInt(m_unDstEnetAddr)); //Destination Address

		// 패킷 시작
		m_NI->InitPacketCapture();


		// 네트워크 주소 설정 완료 및 송신 준비 상태로 변경
		SetDlgState(IPC_ADDR_SET); //주소 설정 완료 상태
		SetDlgState(IPC_READYTOSEND); //송신 중비 완료 상태
	}

	m_bSendReady = !m_bSendReady; //송신 준비 상태 반전
}

void Cipc2023Dlg::SendData()
{
	CString MsgHeader;
	if (m_unDstEnetAddr == (unsigned int)0xff)
		MsgHeader.Format(_T("[%s:BROADCAST] "), m_unSrcEnetAddr);
	else
		MsgHeader.Format(_T("[%s:%s] "), m_unSrcEnetAddr, m_unDstEnetAddr);

	m_ListChat.AddString(MsgHeader + m_stMessage);

	// 입력한 메시지를 파일로 저장
	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength + 1];
	memcpy(ppayload, (unsigned char*)(LPCTSTR)m_stMessage, nlength);
	ppayload[nlength] = '\0';

	// 보낼 data와 메시지 길이를 Send함수로 넘겨준다.
	m_ChatApp->Send(ppayload, m_stMessage.GetLength());
}

BOOL Cipc2023Dlg::Receive(unsigned char* ppayload)
{
	CString Msg;
	int len_ppayload = strlen((char*)ppayload); // ppayload의 길이

	// GetBuff 동적할당 후 초기화
	unsigned char* GetBuff = (unsigned char*)malloc(len_ppayload);
	memset(GetBuff, 0, len_ppayload);
	memcpy(GetBuff, ppayload, len_ppayload);
	GetBuff[len_ppayload] = '\0';

	// App Header를 분석하여, 리스트 창에 뿌려줄 내용의 메시지를 구성한다.
	// 보내는 쪽 또는 받는 쪽과 GetBuff에 저장된 메시지 내용을 합친다.

	if (m_unSrcEnetAddr == (unsigned int)0xff)
		Msg.Format(_T("[%s:BROADCAST] %s"), m_unDstEnetAddr, (char*)GetBuff);
	else
		Msg.Format(_T("[%s:%s] %s"), m_unDstEnetAddr, m_unSrcEnetAddr, (char*)GetBuff);

	KillTimer(1);
	m_ListChat.AddString((LPCTSTR)Msg.GetBuffer(0));
	return TRUE;
}

BOOL Cipc2023Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (::GetDlgCtrlID(::GetFocus()) == IDC_EDIT_MSG)
				OnBnClickedButtonMsgSend();					return FALSE;
		case VK_ESCAPE: return FALSE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void Cipc2023Dlg::SetDlgState(int state) // 다이얼로그 초기화 부분
{
	UpdateData(TRUE);
	int i;
	CString device_description;

	//CButton* pChkButton = (CButton*)GetDlgItem(IDC_CHECK_TOALL);
	CButton* pSendButton = (CButton*)GetDlgItem(IDC_BUTTON_MSG_Send);
	CButton* pSetAddrButton = (CButton*)GetDlgItem(IDC_BUTTON_Setting);


	CEdit* pMsgEdit = (CEdit*)GetDlgItem(IDC_EDIT_MSG);
	CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_Eth_SRC);
	CEdit* pDstEdit = (CEdit*)GetDlgItem(IDC_EDIT_Eth_DST);

	CComboBox* pEnetNameCombo = (CComboBox*)GetDlgItem(IDC_COMBO_Eth);

	switch (state)
	{
	case IPC_INITIALIZING: // 첫 화면 세팅
		pSendButton->EnableWindow(FALSE);
		pMsgEdit->EnableWindow(FALSE);
		m_ListChat.EnableWindow(FALSE);
		break;
	case IPC_READYTOSEND: // Send(S)버튼을 눌렀을 때 세팅
		pSendButton->EnableWindow(TRUE);
		pMsgEdit->EnableWindow(TRUE);
		m_ListChat.EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_UNICASTMODE:
		m_unDstEnetAddr.Format(_T("%.2x%.2x%.2x%.2x%.2x%.2x"), 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
		pDstEdit->EnableWindow(TRUE);
		break;
	case IPC_BROADCASTMODE:
		m_unDstEnetAddr.Format(_T("%.2x%.2x%.2x%.2x%.2x%.2x"), 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
		pDstEdit->EnableWindow(FALSE);
		break;
	case IPC_ADDR_SET:	// 설정(&O)버튼을 눌렀을 때
		pSetAddrButton->SetWindowText(_T("재설정(&R)"));
		pSrcEdit->EnableWindow(FALSE);
		pDstEdit->EnableWindow(TRUE);
		//pChkButton->EnableWindow(FALSE);
		pEnetNameCombo->EnableWindow(FALSE);
		m_NI->m_threadFlag = TRUE;
		break;
	case IPC_ADDR_RESET: // 재설정(&R)버튼을 눌렀을 때
		pSetAddrButton->SetWindowText(_T("설정(&O)"));
		pSrcEdit->EnableWindow(TRUE);
		/*if (!pChkButton->GetCheck())
			pDstEdit->EnableWindow(TRUE);
		pChkButton->EnableWindow(TRUE);*/
		pEnetNameCombo->EnableWindow(TRUE);
		m_NI->m_threadFlag = FALSE;
		break;
	case CFT_COMBO_SET:
		for (i = 0; i < NI_COUNT_NIC; i++) {
			if (!m_NI->GetNetworkAdapter(i))
				break;
			device_description = m_NI->GetNetworkAdapter(i)->description;
			device_description.Trim();
			pEnetNameCombo->AddString(device_description);
			pEnetNameCombo->SetCurSel(0);
		}
		break;
	}

	UpdateData(FALSE);
}


void Cipc2023Dlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}

void Cipc2023Dlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
//	m_ListChat.AddString( ">> The last message was time-out.." ) ;

//	SetTimer(1,2000,NULL);
	KillTimer(1);

	CDialog::OnTimer(nIDEvent);
}





void Cipc2023Dlg::OnComboEnetAddr()
{
	UpdateData(TRUE);

	int nIndex = m_ComboEnetName.GetCurSel();
	m_NI->GetNetworkAdapter(nIndex)->name;

	UpdateData(FALSE);
}



void Cipc2023Dlg::OnOffFileButton(BOOL bBool)
{
	CButton* pFileSendButton = (CButton*)GetDlgItem(IDC_BUTTON_FileSend);
	CButton* pFileSearchButton = (CButton*)GetDlgItem(IDC_BUTTON_File_Add);
	pFileSendButton->EnableWindow(bBool);
	pFileSearchButton->EnableWindow(bBool);
}

unsigned char* Cipc2023Dlg::MacAddrToHexInt(CString ether)
{
	// 콜론(:)으로 구분 되어진 Ethernet 주소를
	// 콜론(:)을 토큰으로 한 바이트씩 값을 가져와서 Ethernet배열에 넣어준다.
	CString cstr;
	unsigned char* arp_ether = (u_char*)malloc(sizeof(u_char) * 6);

	for (int i = 0; i < 6; i++) {
		AfxExtractSubString(cstr, ether, i, ':');
		// strtoul -> 문자열을 원하는 진수로 변환 시켜준다.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(), NULL, 16);
	}
	arp_ether[6] = '\0';

	return arp_ether;
}

void Cipc2023Dlg::OnCbnSelchangeComboEth()
{
	int nIndex = m_ComboEnetName.GetCurSel();
	m_NI->SetNetworkAdapterNumber(nIndex);

	CString inNicName = m_NI->GetNetworkAdapter(nIndex)->name;

	CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_Eth_SRC);

	pSrcEdit->SetWindowTextA(m_NI->GetNICardAddress((char*)inNicName.GetString()));

	//선택한 adapter에 맞는 ethernet주소

	//

	m_Eth->SetEnetHeaderSrcAddress(MacAddrToHexInt(m_unSrcEnetAddr));
}


void Cipc2023Dlg::OnBnClickedButtonFileAdd() //파일 탐색기 열기
{
	CFileDialog dlg(TRUE); // 파일 탐색기 열기

	if (dlg.DoModal() == IDOK) // 사용자가 파일을 선택하고 확인을 눌렀을 경우
	{
		// 선택한 파일의 경로에 대한 정보를 가져와서 화면에 출력
		m_FileName = dlg.GetPathName();
		GetDlgItem(IDC_FILE_NAME)->SetWindowText(m_FileName);
	}
}


void Cipc2023Dlg::OnBnClickedButtonFileSend()
{
	// 파일 열기
	CFile file;
	if (!file.Open(m_FileName, CFile::modeRead | CFile::typeBinary)) {
		AfxMessageBox(_T("파일을 열 수 없습니다."));
		return;
	}

	DWORD fileSize = file.GetLength(); 	// 파일 크기 가져오기

	unsigned char* buffer = new unsigned char[fileSize]; //파일 데이터 담을 버퍼
	
	//파일 읽기
	file.Read(buffer, fileSize);
	file.Close();

	BOOL isSuccess = m_FileApp->Send(buffer, fileSize); //파일 데이터를 FileLayer로
	
	//아래는 파일 전송 확인용
	if (isSuccess) {
		AfxMessageBox(_T("파일 전송 성공"));
	}
	else {
		AfxMessageBox(_T("파일 전송 실패"));
	}

}
