// NILayer.h: interface for the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include <pcap.h>
#include <Packet32.h>
#include <iphlpapi.h>
#pragma comment (lib, "iphlpapi.lib")




class CNILayer
	: public CBaseLayer
{
protected:
	pcap_t* m_networkHandler; //네트워크 "핸들러": 네트워크 "어댑터"를 열어 패킷을 수신하거나 패킷을 전송할 수 있게 하는데 사용

public:
	BOOL			m_threadFlag; //스레드 실행 여부를 제어
	unsigned char* m_ppayload; //수신받은 데이터

	void			InitPacketCapture();// 네트워크 인터페이스 드라이버를 시작하고, 패킷 캡처를 활성화하는 함수.

	pcap_if_t* GetNetworkAdapter(int iIndex);//네트워크 어뎁터 리스트에서 특정 인덱스의 네트워크 어뎁터를 리턴하는 함수.
	void			SetNetworkAdapterNumber(int iNum);//네트워크 어뎁터에 번호를 붙히는 함수
	void			SetNetworkAdapterList(LPADAPTER* plist);//네트워크 어뎁터 리스트를 만드는 함수

	static UINT		ReadThread(LPVOID pParam);
	static UINT		NILayerChattingThread(LPVOID pParam);

	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);

	CString GetNICardAddress(char* adapter_name);

	CNILayer(char* pName, LPADAPTER* pAdapterObject = NULL, int iNumAdapter = 0);
	virtual ~CNILayer();

protected:
	int			m_NIC_index;//선택된 네트워크 "어댑터"의 인덱스 변수. 선택된 네트워크 "어댑터"를 사용하여 패킷을 송수신하거나 캡처하는 데 사용.
	pcap_if_t* m_NIC_List[NI_COUNT_NIC];//네트워크 인터페이스 카드(NIC=네트워크 어뎁터) 목록을 저장하는 배열
};

#endif // !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
