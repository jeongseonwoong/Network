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
    m_networkHandler = NULL;    // pcap_t에 선언된 네트워크 "핸들러": 네트워크 "어댑터"를 열어 패킷을 수신하거나 패킷을 전송할 수 있게 하는데 사용
    m_NIC_index = iNumAdapter;    // 네트워크 어댑터 리스트에서 사용할 NIC의 index를 생성자로 받아와 멤버변수에 저장한다.
    m_threadFlag = TRUE;       // 스레드 실행 여부를 제어
    SetNetworkAdapterList(NULL); // 네트워크 어댑터 리스트(NIC 리스트)를 초기화 하는 함수 호출
}

CNILayer::~CNILayer()
{
}

void CNILayer::InitPacketCapture() // 네트워크 인터페이스 드라이버를 시작하고, 패킷 캡처를 활성화하는 함수.
{
    char errorBuffer[PCAP_ERRBUF_SIZE]; //에러 버퍼

    if (m_NIC_index == -1) { //선택된 NIC카드 index가 없을 때 오류 메시지 출력
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }

    m_networkHandler = pcap_open_live(m_NIC_List[m_NIC_index]->name, 1500, PCAP_OPENFLAG_PROMISCUOUS, 2000, errorBuffer);
    //1. NIC리스트에서 m_NIC_index에 해당되는 NIC를 연다 
    //2. 프레임 최대 데이터 크기를 1500바이트로 설정한다.
    //3. 프로미스큐어스 모드로 설정하여 네트워크에 지나가는 모든 패킷을 가로채어 저장하거나 분석할 수 있게한다.
    //4. 타임아웃을 2초로 설정하여 요청에 대한 응답이 2초안에 오지 않으면 타임아웃이 되도록 한다.
    //5. 함수가 실패하면 errorBuffer에 오류 메시지를 저장하고, 성공하면 네트워크 핸들러를 반환한다.
    if (!m_networkHandler) {
        AfxMessageBox(errorBuffer);// 만약 네트워크 핸들러를 가져오지 못했다면 에러메시지 출력
        return;
    }
    m_threadFlag = TRUE;//쓰레드를 실행시킬지 여부에 대한 flag를 TRUE로 설정.
    AfxBeginThread(ReadThread, this); //네트워크 트래픽을 캡처하기 위한 백그라운드 스레드를 시작하여 ReadThread 함수를 실행시킴.
}

pcap_if_t* CNILayer::GetNetworkAdapter(int iIndex)//사용할 네트워크 어댑터(NIC)에 해당되는 index를 입력으로 받아 해당 NIC를 NIC List로부터 리턴하는 함수.
{
    return m_NIC_List[iIndex];
}

void CNILayer::SetNetworkAdapterNumber(int iNum) //사용할 네트워크 어댑터에 해당되는 index를 지역변수 m_NIC_indx에 설정하는 함수.
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
        m_NIC_List[j] = NULL; // NIC LIst를 모두 NULL로 초기화시킨다.
    }

    // pcap_findalldevs을 이용해서 컴퓨터에 존재하는 NIC 목록 검색 후 NIC를 찾는데 실패하면 -1을 리턴하고 오류 메시지를 errbuf에 저장하고 에러메세지를 출력한다. 
    if (pcap_findalldevs(&pAllNetworkDevices, errbuf) == -1)
    {
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }
    if (!pAllNetworkDevices)  //pAllNetworkDevice 그러니까 컴퓨터에 존재하는 모든 네트워크 장비들을 가리킬 포인터의 값이 NULL일 경우 에러메시지를 출력한다.
    {
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }

    // 검색한 네트워크 어댑터 결과들을 m_NIC_List 배열에 저장
    int i = 0;
    for (p_NIC_finder = pAllNetworkDevices; p_NIC_finder; p_NIC_finder = p_NIC_finder->next)//컴터에 있는 모든 네트워크 장비를 거치면서 NIC를 찾으면 p_NIC_finder에 assign하고 m_NIC_List에 추가
    {
        m_NIC_List[i++] = p_NIC_finder;
    }
}

CString CNILayer::GetNICardAddress(char* adapter_name)
{
    PPACKET_OID_DATA NIC_OidData;
    LPADAPTER lpAdapter;

    // MAC 주소를 저장할 공간 할당
    NIC_OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
    if (NIC_OidData == NULL)
    {
        return "None"; // 메모리 할당 실패 시 None 반환
    }

    NIC_OidData->Oid = OID_802_3_CURRENT_ADDRESS; // MAC 주소 요청
    NIC_OidData->Length = 6; // MAC 주소의 길이 (6바이트)
    ZeroMemory(NIC_OidData->Data, 6); // 데이터를 0으로 초기화

    // 주어진 어댑터 이름으로 어댑터 열기
    lpAdapter = PacketOpenAdapter(adapter_name);

    CString NICardAddress;

    // MAC 주소 요청 성공 시 MAC 주소 포맷
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

    // 어댑터 닫고 메모리 해제
    PacketCloseAdapter(lpAdapter);
    free(NIC_OidData);

    // MAC 주소 반환
    return NICardAddress;
}


BOOL CNILayer::Send(unsigned char* ppayload, int nlength)
{
    // pcap_sendpacket을 사용해 패킷 전송
    if (pcap_sendpacket(m_networkHandler, ppayload, nlength))
    {
        // 전송 실패 시 메시지 박스 출력
        AfxMessageBox(_T("패킷 전송 실패"));
        return FALSE;
    }
    // 전송 성공 시 TRUE 반환
    return TRUE;
}

BOOL CNILayer::Receive(unsigned char* ppayload)
{
    BOOL isSuccess = FALSE;

    // 상위 계층의 Receive 함수로 패킷 전달
    isSuccess = mp_aUpperLayer[0]->Receive(ppayload);
    return isSuccess; // 성공 여부 반환
}


UINT CNILayer::ReadThread(LPVOID pParam)
{
    struct pcap_pkthdr* header; // 패킷의 메타데이터
    const u_char* packet_data; // 패킷 데이터
    int result;

    CNILayer* pNILayer = (CNILayer*)pParam;

    // 스레드 실행 중 패킷을 지속적으로 읽음
    while (pNILayer->m_threadFlag)
    {
        // pcap_next_ex로 패킷 수신
        result = pcap_next_ex(pNILayer->m_networkHandler, &header, &packet_data);

        if (result == 1) // 패킷이 성공적으로 수신된 경우
        {
            // 패킷 데이터를 Receive 함수로 전달
            pNILayer->Receive((u_char*)packet_data);
        }
    }

    return 0;
}



