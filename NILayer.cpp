#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



CNILayer::CNILayer(char* pName, LPADAPTER* pAdapterObject, int iNumAdapter)
	: CBaseLayer(pName)
{
	m_networkHandler = NULL;
	m_NIC_index = iNumAdapter;
	m_threadFlag = TRUE;
	SetNetworkAdapterList(NULL);
}

CNILayer::~CNILayer()
{
}

void CNILayer::InitPacketCapture() //패킷을 캡처하거나 송수신할 준비를 하는 함수입니다.
{
	char errorBuffer[PCAP_ERRBUF_SIZE]; //에러 버퍼

	if (m_NIC_index == -1) { //선택된 NIC카드 index가 없을 때
		AfxMessageBox(_T("Not exist NICard"));
		return;
	}

	m_networkHandler = pcap_open_live(m_NIC_List[m_NIC_index]->name, 1500, PCAP_OPENFLAG_PROMISCUOUS, 2000, errorBuffer);//NIC카드를 이용해서 네트워크 핸들러를 가져옴.
	if (!m_networkHandler) {
		AfxMessageBox(errorBuffer);
		return;
	}
	m_threadFlag = TRUE;
	AfxBeginThread(ReadThread, this);
}

pcap_if_t* CNILayer::GetNetworkAdapter(int iIndex)
{
	return m_NIC_List[iIndex];
}

void CNILayer::SetNetworkAdapterNumber(int iNum)
{
	m_NIC_index = iNum;
}

void CNILayer::SetNetworkAdapterList(LPADAPTER* plist)
{
	pcap_if_t* pAllNetworkDevices; // 컴퓨터에 존재하는 모든 네트워크 장치들을 가리킬 포인터
	pcap_if_t* p_NIC_finder;       // 장치 목록을 탐색하기 위한 임시 포인터

	char errbuf[PCAP_ERRBUF_SIZE]; // 오류 메시지 저장 버퍼

	for (int j = 0; j < NI_COUNT_NIC; j++)
	{
		m_NIC_List[j] = NULL; // NIC LIst 초기화
	}

	// pcap_findalldevs을 이용해서 컴퓨터에 존재하는 NIC 목록 검색 후 NIC가 없을 경우 오류 메시지 출력
	if (pcap_findalldevs(&pAllNetworkDevices, errbuf) == -1)
	{
		AfxMessageBox(_T("Not exist NICard"));
		return;
	}
	if (!pAllNetworkDevices)
	{
		AfxMessageBox(_T("Not exist NICard"));
		return;
	}

	// 검색한 네트워크 어댑터들을 m_NIC_List 배열에 저장
	int i = 0;
	for (p_NIC_finder = pAllNetworkDevices; p_NIC_finder; p_NIC_finder = p_NIC_finder->next)//컴터에 있는 모든 네트워크 장비를 거치면서 NIC를 찾으면 p_NIC_finder에 assign하고 m_NIC_List에 추가
	{
		m_NIC_List[i++] = p_NIC_finder;
	}
}


BOOL CNILayer::Send(unsigned char* ppayload, int nlength)//데이터 전송
{
	if (pcap_sendpacket(m_networkHandler, ppayload, nlength))
	{
		AfxMessageBox(_T("패킷 전송 실패"));
		return FALSE;
	}
	return TRUE;
}

BOOL CNILayer::Receive(unsigned char* ppayload) //데이터를 전달받음. 
{
	BOOL isSuccess = FALSE;

	isSuccess = mp_aUpperLayer[0]->Receive(ppayload);
	return isSuccess;
}

UINT CNILayer::ReadThread(LPVOID pParam)
{
	struct pcap_pkthdr* header;
	const u_char* packet_data;
	int result;

	AfxBeginThread(NILayerChattingThread, (LPVOID)pParam);
	CNILayer* pNILayer = (CNILayer*)pParam; //스레드 함수에서 CNILayer 객체에 접근하기 위해 사용

	while (pNILayer->m_threadFlag) // 패킷 체크
	{
		// 패킷 읽어오기
		result = pcap_next_ex(pNILayer->m_networkHandler, &header, &packet_data);

		if (result == 0) {
			//	AfxMessageBox("패킷 없음");
		}
		else if (result == 1) {
			//	AfxMessageBox(_T("패킷 있음"));
			pNILayer->Receive((u_char*)packet_data);
		}
		else if (result < 0) {
			//	AfxMessageBox("패킷 오류");
		}
	}

	return 0;
}

UINT CNILayer::NILayerChattingThread(LPVOID pParam)
{
	CNILayer* pNILayer = (CNILayer*)pParam;

	return 0;
}

CString CNILayer::GetNICardAddress(char* adapter_name) //NIC 받아오기
{
	PPACKET_OID_DATA NIC_OidData;
	LPADAPTER lpAdapter;

	NIC_OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
	if (NIC_OidData == NULL)
	{
		return "None";
	}

	NIC_OidData->Oid = OID_802_3_CURRENT_ADDRESS;
	NIC_OidData->Length = 6;
	ZeroMemory(NIC_OidData->Data, 6);

	lpAdapter = PacketOpenAdapter(adapter_name);

	CString NICardAddress;

	if (PacketRequest(lpAdapter, FALSE, NIC_OidData))
	{
		NICardAddress.Format("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
			(NIC_OidData->Data)[0],
			(NIC_OidData->Data)[1],
			(NIC_OidData->Data)[2],
			(NIC_OidData->Data)[3],
			(NIC_OidData->Data)[4],
			(NIC_OidData->Data)[5]);
	}

	PacketCloseAdapter(lpAdapter);
	free(NIC_OidData);
	return NICardAddress;
}
