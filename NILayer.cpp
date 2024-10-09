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

void CNILayer::InitPacketCapture() //��Ŷ�� ĸó�ϰų� �ۼ����� �غ� �ϴ� �Լ��Դϴ�.
{
	char errorBuffer[PCAP_ERRBUF_SIZE]; //���� ����

	if (m_NIC_index == -1) { //���õ� NICī�� index�� ���� ��
		AfxMessageBox(_T("Not exist NICard"));
		return;
	}

	m_networkHandler = pcap_open_live(m_NIC_List[m_NIC_index]->name, 1500, PCAP_OPENFLAG_PROMISCUOUS, 2000, errorBuffer);//NICī�带 �̿��ؼ� ��Ʈ��ũ �ڵ鷯�� ������.
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
	pcap_if_t* pAllNetworkDevices; // ��ǻ�Ϳ� �����ϴ� ��� ��Ʈ��ũ ��ġ���� ����ų ������
	pcap_if_t* p_NIC_finder;       // ��ġ ����� Ž���ϱ� ���� �ӽ� ������

	char errbuf[PCAP_ERRBUF_SIZE]; // ���� �޽��� ���� ����

	for (int j = 0; j < NI_COUNT_NIC; j++)
	{
		m_NIC_List[j] = NULL; // NIC LIst �ʱ�ȭ
	}

	// pcap_findalldevs�� �̿��ؼ� ��ǻ�Ϳ� �����ϴ� NIC ��� �˻� �� NIC�� ���� ��� ���� �޽��� ���
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

	// �˻��� ��Ʈ��ũ ����͵��� m_NIC_List �迭�� ����
	int i = 0;
	for (p_NIC_finder = pAllNetworkDevices; p_NIC_finder; p_NIC_finder = p_NIC_finder->next)//���Ϳ� �ִ� ��� ��Ʈ��ũ ��� ��ġ�鼭 NIC�� ã���� p_NIC_finder�� assign�ϰ� m_NIC_List�� �߰�
	{
		m_NIC_List[i++] = p_NIC_finder;
	}
}


BOOL CNILayer::Send(unsigned char* ppayload, int nlength)//������ ����
{
	if (pcap_sendpacket(m_networkHandler, ppayload, nlength))
	{
		AfxMessageBox(_T("��Ŷ ���� ����"));
		return FALSE;
	}
	return TRUE;
}

BOOL CNILayer::Receive(unsigned char* ppayload) //�����͸� ���޹���. 
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
	CNILayer* pNILayer = (CNILayer*)pParam; //������ �Լ����� CNILayer ��ü�� �����ϱ� ���� ���

	while (pNILayer->m_threadFlag) // ��Ŷ üũ
	{
		// ��Ŷ �о����
		result = pcap_next_ex(pNILayer->m_networkHandler, &header, &packet_data);

		if (result == 0) {
			//	AfxMessageBox("��Ŷ ����");
		}
		else if (result == 1) {
			//	AfxMessageBox(_T("��Ŷ ����"));
			pNILayer->Receive((u_char*)packet_data);
		}
		else if (result < 0) {
			//	AfxMessageBox("��Ŷ ����");
		}
	}

	return 0;
}

UINT CNILayer::NILayerChattingThread(LPVOID pParam)
{
	CNILayer* pNILayer = (CNILayer*)pParam;

	return 0;
}

CString CNILayer::GetNICardAddress(char* adapter_name) //NIC �޾ƿ���
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
