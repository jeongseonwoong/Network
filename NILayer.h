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
	pcap_t* m_networkHandler; //��Ʈ��ũ "�ڵ鷯": ��Ʈ��ũ "�����"�� ���� ��Ŷ�� �����ϰų� ��Ŷ�� ������ �� �ְ� �ϴµ� ���

public:
	BOOL			m_threadFlag; //������ ���� ���θ� ����
	unsigned char* m_ppayload; //���Ź��� ������

	void			InitPacketCapture();// ��Ʈ��ũ �������̽� ����̹��� �����ϰ�, ��Ŷ ĸó�� Ȱ��ȭ�ϴ� �Լ�.

	pcap_if_t* GetNetworkAdapter(int iIndex);//��Ʈ��ũ ��� ����Ʈ���� Ư�� �ε����� ��Ʈ��ũ ��͸� �����ϴ� �Լ�.
	void			SetNetworkAdapterNumber(int iNum);//��Ʈ��ũ ��Ϳ� ��ȣ�� ������ �Լ�
	void			SetNetworkAdapterList(LPADAPTER* plist);//��Ʈ��ũ ��� ����Ʈ�� ����� �Լ�

	static UINT		ReadThread(LPVOID pParam);
	static UINT		NILayerChattingThread(LPVOID pParam);

	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);

	CString GetNICardAddress(char* adapter_name);

	CNILayer(char* pName, LPADAPTER* pAdapterObject = NULL, int iNumAdapter = 0);
	virtual ~CNILayer();

protected:
	int			m_NIC_index;//���õ� ��Ʈ��ũ "�����"�� �ε��� ����. ���õ� ��Ʈ��ũ "�����"�� ����Ͽ� ��Ŷ�� �ۼ����ϰų� ĸó�ϴ� �� ���.
	pcap_if_t* m_NIC_List[NI_COUNT_NIC];//��Ʈ��ũ �������̽� ī��(NIC=��Ʈ��ũ ���) ����� �����ϴ� �迭
};

#endif // !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
