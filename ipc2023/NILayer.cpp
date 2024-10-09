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
    m_networkHandler = NULL;    // pcap_t�� ����� ��Ʈ��ũ "�ڵ鷯": ��Ʈ��ũ "�����"�� ���� ��Ŷ�� �����ϰų� ��Ŷ�� ������ �� �ְ� �ϴµ� ���
    m_NIC_index = iNumAdapter;    // ��Ʈ��ũ ����� ����Ʈ���� ����� NIC�� index�� �����ڷ� �޾ƿ� ��������� �����Ѵ�.
    m_threadFlag = TRUE;       // ������ ���� ���θ� ����
    SetNetworkAdapterList(NULL); // ��Ʈ��ũ ����� ����Ʈ(NIC ����Ʈ)�� �ʱ�ȭ �ϴ� �Լ� ȣ��
}

CNILayer::~CNILayer()
{
}

void CNILayer::InitPacketCapture() // ��Ʈ��ũ �������̽� ����̹��� �����ϰ�, ��Ŷ ĸó�� Ȱ��ȭ�ϴ� �Լ�.
{
    char errorBuffer[PCAP_ERRBUF_SIZE]; //���� ����

    if (m_NIC_index == -1) { //���õ� NICī�� index�� ���� �� ���� �޽��� ���
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }

    m_networkHandler = pcap_open_live(m_NIC_List[m_NIC_index]->name, 1500, PCAP_OPENFLAG_PROMISCUOUS, 2000, errorBuffer);
    //1. NIC����Ʈ���� m_NIC_index�� �ش�Ǵ� NIC�� ���� 
    //2. ������ �ִ� ������ ũ�⸦ 1500����Ʈ�� �����Ѵ�.
    //3. ���ι̽�ť� ���� �����Ͽ� ��Ʈ��ũ�� �������� ��� ��Ŷ�� ����ä�� �����ϰų� �м��� �� �ְ��Ѵ�.
    //4. Ÿ�Ӿƿ��� 2�ʷ� �����Ͽ� ��û�� ���� ������ 2�ʾȿ� ���� ������ Ÿ�Ӿƿ��� �ǵ��� �Ѵ�.
    //5. �Լ��� �����ϸ� errorBuffer�� ���� �޽����� �����ϰ�, �����ϸ� ��Ʈ��ũ �ڵ鷯�� ��ȯ�Ѵ�.
    if (!m_networkHandler) {
        AfxMessageBox(errorBuffer);// ���� ��Ʈ��ũ �ڵ鷯�� �������� ���ߴٸ� �����޽��� ���
        return;
    }
    m_threadFlag = TRUE;//�����带 �����ų�� ���ο� ���� flag�� TRUE�� ����.
    AfxBeginThread(ReadThread, this); //��Ʈ��ũ Ʈ������ ĸó�ϱ� ���� ��׶��� �����带 �����Ͽ� ReadThread �Լ��� �����Ŵ.
}

pcap_if_t* CNILayer::GetNetworkAdapter(int iIndex)//����� ��Ʈ��ũ �����(NIC)�� �ش�Ǵ� index�� �Է����� �޾� �ش� NIC�� NIC List�κ��� �����ϴ� �Լ�.
{
    return m_NIC_List[iIndex];
}

void CNILayer::SetNetworkAdapterNumber(int iNum) //����� ��Ʈ��ũ ����Ϳ� �ش�Ǵ� index�� �������� m_NIC_indx�� �����ϴ� �Լ�.
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
        m_NIC_List[j] = NULL; // NIC LIst�� ��� NULL�� �ʱ�ȭ��Ų��.
    }

    // pcap_findalldevs�� �̿��ؼ� ��ǻ�Ϳ� �����ϴ� NIC ��� �˻� �� NIC�� ã�µ� �����ϸ� -1�� �����ϰ� ���� �޽����� errbuf�� �����ϰ� �����޼����� ����Ѵ�. 
    if (pcap_findalldevs(&pAllNetworkDevices, errbuf) == -1)
    {
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }
    if (!pAllNetworkDevices)  //pAllNetworkDevice �׷��ϱ� ��ǻ�Ϳ� �����ϴ� ��� ��Ʈ��ũ ������ ����ų �������� ���� NULL�� ��� �����޽����� ����Ѵ�.
    {
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }

    // �˻��� ��Ʈ��ũ ����� ������� m_NIC_List �迭�� ����
    int i = 0;
    for (p_NIC_finder = pAllNetworkDevices; p_NIC_finder; p_NIC_finder = p_NIC_finder->next)//���Ϳ� �ִ� ��� ��Ʈ��ũ ��� ��ġ�鼭 NIC�� ã���� p_NIC_finder�� assign�ϰ� m_NIC_List�� �߰�
    {
        m_NIC_List[i++] = p_NIC_finder;
    }
}

CString CNILayer::GetNICardAddress(char* adapter_name)
{
    PPACKET_OID_DATA NIC_OidData;
    LPADAPTER lpAdapter;

    // MAC �ּҸ� ������ ���� �Ҵ�
    NIC_OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
    if (NIC_OidData == NULL)
    {
        return "None"; // �޸� �Ҵ� ���� �� None ��ȯ
    }

    NIC_OidData->Oid = OID_802_3_CURRENT_ADDRESS; // MAC �ּ� ��û
    NIC_OidData->Length = 6; // MAC �ּ��� ���� (6����Ʈ)
    ZeroMemory(NIC_OidData->Data, 6); // �����͸� 0���� �ʱ�ȭ

    // �־��� ����� �̸����� ����� ����
    lpAdapter = PacketOpenAdapter(adapter_name);

    CString NICardAddress;

    // MAC �ּ� ��û ���� �� MAC �ּ� ����
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

    // ����� �ݰ� �޸� ����
    PacketCloseAdapter(lpAdapter);
    free(NIC_OidData);

    // MAC �ּ� ��ȯ
    return NICardAddress;
}


BOOL CNILayer::Send(unsigned char* ppayload, int nlength)
{
    // pcap_sendpacket�� ����� ��Ŷ ����
    if (pcap_sendpacket(m_networkHandler, ppayload, nlength))
    {
        // ���� ���� �� �޽��� �ڽ� ���
        AfxMessageBox(_T("��Ŷ ���� ����"));
        return FALSE;
    }
    // ���� ���� �� TRUE ��ȯ
    return TRUE;
}

BOOL CNILayer::Receive(unsigned char* ppayload)
{
    BOOL isSuccess = FALSE;

    // ���� ������ Receive �Լ��� ��Ŷ ����
    isSuccess = mp_aUpperLayer[0]->Receive(ppayload);
    return isSuccess; // ���� ���� ��ȯ
}


UINT CNILayer::ReadThread(LPVOID pParam)
{
    struct pcap_pkthdr* header; // ��Ŷ�� ��Ÿ������
    const u_char* packet_data; // ��Ŷ ������
    int result;

    CNILayer* pNILayer = (CNILayer*)pParam;

    // ������ ���� �� ��Ŷ�� ���������� ����
    while (pNILayer->m_threadFlag)
    {
        // pcap_next_ex�� ��Ŷ ����
        result = pcap_next_ex(pNILayer->m_networkHandler, &header, &packet_data);

        if (result == 1) // ��Ŷ�� ���������� ���ŵ� ���
        {
            // ��Ŷ �����͸� Receive �Լ��� ����
            pNILayer->Receive((u_char*)packet_data);
        }
    }

    return 0;
}



