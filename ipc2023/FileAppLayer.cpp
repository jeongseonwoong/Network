#include "stdafx.h"
#include "pch.h"
#include "FileAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// 패킷 전송
BOOL CFileAppLayer::Send(unsigned char* ppayload, int nlength)
{
    if (nlength <= 0) {
        AfxMessageBox(_T("전송할 데이터가 없습니다."));
        return FALSE;
    }

    const int MAX_PACKET_SIZE = 1480; // 예시: 최대 패킷 크기

    // 데이터 단편화 및 전송
    for (int i = 0; i < nlength; i += MAX_PACKET_SIZE) {
        int chunkSize = min(MAX_PACKET_SIZE, nlength - i);
        if (pcap_sendpacket(m_AdapterObject, ppayload + i, chunkSize) != 0) { //chucksize남은 데이터 크기랑 최대패킷크기중 작은 값
            AfxMessageBox(_T("패킷 전송 실패"));
            return FALSE;
        }
    }

    return TRUE; // 데이터 전송 결과
}

// 패킷 수신
BOOL CFileAppLayer::Receive(unsigned char* ppayload)
{
    if (ppayload == nullptr) {
        AfxMessageBox(_T("수신할 데이터가 유효하지 않습니다."));
        return FALSE;
    }

    BOOL success = mp_aUpperLayer[0]->Receive(ppayload); // 수신된 ppayload를 상위 계층으로 전달
    return success; // 패킷 전달 결과
}


UINT CFileAppLayer::PacketReceiverThread(LPVOID param)
{
    CFileAppLayer* layer = static_cast<CFileAppLayer*>(param); // 형변환
    struct pcap_pkthdr* header;                               // 패킷 헤더
    const u_char* data;                                       // 패킷 데이터

    // 스레드가 실행되는 동안 계속 패킷수신
    while (layer->m_thrdSwitch) {
        if (pcap_next_ex(layer->m_AdapterObject, &header, &data) == 1)
            layer->Receive((u_char*)data); // Receive 함수로 전달하여 패킷 수신 
    }
    return 0;
}
