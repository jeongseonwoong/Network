#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "pch.h"
#include "ChatAppLayer.h"
#include "EthernetLayer.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CChatAppLayer::CChatAppLayer(char* pName)
	: CBaseLayer(pName),
	UI_Interaction(NULL)//UI�� ��ȣ�ۿ��� ���� ������
{
	ResetChatAppHeader();
}

CChatAppLayer::~CChatAppLayer()
{
}

void CChatAppLayer::ResetChatAppHeader() // ChatApp ��� �ʱ�ȭ
{
	m_chatAppHeader.data_packet_length = 0x0000;
	m_chatAppHeader.data_packet_stage = DATA_TYPE_BEGIN;

	memset(m_chatAppHeader.data_packet_payload, 0, APP_DATA_SIZE);
}

BOOL CChatAppLayer::Send(unsigned char* ppayload, int nlength)
{
	m_ppayload = ppayload; // ppayload : �Է� ������
	m_length = nlength; //nlength: ������ ����

	if (nlength <= APP_DATA_SIZE) {
		((CEthernetLayer*)GetUnderLayer())->SetEnetHeaderFrameType(0x2080);
		m_chatAppHeader.data_packet_length = nlength;
		memcpy(m_chatAppHeader.data_packet_payload, ppayload, nlength); // GetBuff�� data ����
		mp_UnderLayer->Send((unsigned char*)&m_chatAppHeader, m_length + APP_HEADER_SIZE);
	}
	else {
		AfxBeginThread(ChatThread, this); // ���ο� ������ ���� �� ChatThread�Լ��� ����
	}
	return TRUE;
}

BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{

	CHAT_APP_HEADER_POINTER header = (CHAT_APP_HEADER_POINTER)ppayload;// ChatApp ��� ������ -> ppayload .
	static unsigned char* DataBuffer; // �����͸� ���� ���� ����

	if (header->data_packet_length <= APP_DATA_SIZE) {
		DataBuffer = (unsigned char*)malloc(header->data_packet_length); //������ ũ�⸸ŭ �޸� �Ҵ�
		memset(DataBuffer, 0, header->data_packet_length);   //������ ũ�� �ʱ�ȭ
		memcpy(DataBuffer, header->data_packet_payload, header->data_packet_length); //������ ����
		DataBuffer[header->data_packet_length] = '\0';//������

		mp_aUpperLayer[0]->Receive((unsigned char*)DataBuffer); // ���� �������� ������ �ø�
		return TRUE;
	}
	// �Ʒ� �������� �Ѱܹ��� ������ �м� �� ChatDlg �������� �Ѱ���
	if (header->data_packet_stage == DATA_TYPE_BEGIN) // ��Ŷ��� ������ ������ �� ù��° ��Ŷ�� ��
	{
		// ��Ŷ ũ�⸸ŭ �޸� �Ҵ�	
		DataBuffer = (unsigned char*)malloc(header->data_packet_length);
		memset(DataBuffer, 0, header->data_packet_length);  // ������ ũ�� �ʱ�ȭ
	}
	else if (header->data_packet_stage == DATA_TYPE_CONT) // ��Ŷ��� ������ ������ �� �߰���° ��Ŷ�� ��
	{
		// �����͸� ���� ��ҿ� ����� ������ �߰�
		strncat((char*)DataBuffer, (char*)header->data_packet_payload, strlen((char*)header->data_packet_payload));
		DataBuffer[strlen((char*)DataBuffer)] = '\0'; //������ 
	}
	else if (header->data_packet_stage == DATA_TYPE_END) // ��Ŷ��� ������ ������ �� ������ ��Ŷ�� ��
	{
		// ���ۿ� ���� �����͸� �ٽ� DataBuffer�� �ְ�
		memcpy(DataBuffer, DataBuffer, header->data_packet_length);
		DataBuffer[header->data_packet_length] = '\0';

		// ������ ���� �޽����� ChatDlg�� �Ѱ���
		mp_aUpperLayer[0]->Receive((unsigned char*)DataBuffer);
		free(DataBuffer);
	}
	else
		return FALSE;

	return TRUE;
}

UINT CChatAppLayer::ChatThread(LPVOID pParam)
{
	BOOL isSuccess = FALSE;
	CChatAppLayer* pChat = (CChatAppLayer*)pParam;
	((CEthernetLayer*)(pChat->GetUnderLayer()))->SetEnetHeaderFrameType(0x2080);
	int data_length = APP_DATA_SIZE; // data�� ����
	int data_packet_num; // ������ �����͸� �� ���� ��Ŷ���� ������� �ϴ����� ��Ÿ���� ����
	int data_index;	 // data�� ��Ŷ���� ���� ���� ��, ���� ��Ŷ�� �����͸� ������ ó���ߴ����� �����ϴ� ����
	// ���� ��Ŷ�� ���� ���� ù index�� �ȴ�.
	int finished_index = 0;

	// ��Ŷ�� �� ������ ����
	if (pChat->m_length < APP_DATA_SIZE) // APP_DATA_SIZE ���� ���� ��� �� ���� �����ϸ� ��
		data_packet_num = 1;
	else // �׷��� ���� ���, ������ ���̷� APP_DATA_SIZE�� ���� �� 1�� ���� ������ data_packet_num�� ����
		data_packet_num = (pChat->m_length / APP_DATA_SIZE) + 1;

	for (int i = 0; i <= data_packet_num + 1; i++)
	{
		// ���� data�� ���̸� ����
		if (data_packet_num == 1) { // ���� Ƚ���� �� ���̸�, ������ ���� ��ŭ ����
			data_length = pChat->m_length;
		}
		else { // ���� Ƚ���� �� �� �̻�
			if (i == data_packet_num) // ������ ������ ��, ���� �������� ���̸�ŭ ����
				data_length = pChat->m_length % APP_DATA_SIZE;
			else // ó��, �߰� ������ �� ���, APP_DATA_SIZE��ŭ ����
				data_length = APP_DATA_SIZE;
		}

		memset(pChat->m_chatAppHeader.data_packet_payload, 0, data_length);

		if (i == 0) // ó���κ�, �������� �� ���̸� ����
		{
			pChat->m_chatAppHeader.data_packet_length = pChat->m_length;  //����
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_BEGIN; //ó�� ��Ŷ ����, Ÿ��: 0x00
			memset(pChat->m_chatAppHeader.data_packet_payload, 0, data_length); //�ʱ�ȭ
			data_length = 0;
		}
		else if (i != 0 && i <= data_packet_num) // �߰� ��Ŷ ó��
		{
			data_index = data_length * 2;
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_CONT; //�߰� ��Ŷ ����, Ÿ��: 0x01
			pChat->m_chatAppHeader.data_packet_sequence = i - 1;

			CString str;
			str = pChat->m_ppayload;
			str = str.Mid(finished_index, finished_index + data_index);//finished_index���� finished_index + data_index ���� ������ ����

			memcpy(pChat->m_chatAppHeader.data_packet_payload, str, data_length);
			finished_index += data_index;
		}
		else // ������ ��Ŷ ó��
		{
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_END; //������ ��Ŷ ����, Ÿ��: 0x02
			memset(pChat->m_ppayload, 0, data_length); //�ʱ�ȭ
			data_length = 0;
		}
		isSuccess = pChat->mp_UnderLayer->Send((unsigned char*)&pChat->m_chatAppHeader, data_length + APP_HEADER_SIZE); //Ethernet���̾��� send�Լ��� ������ ����
	}

	return isSuccess;
}