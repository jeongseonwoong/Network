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
	UI_Interaction(NULL)//UI랑 상호작용을 위한 포인터
{
	ResetChatAppHeader();
}

CChatAppLayer::~CChatAppLayer()
{
}

void CChatAppLayer::ResetChatAppHeader() // ChatApp 헤더 초기화
{
	m_chatAppHeader.data_packet_length = 0x0000;
	m_chatAppHeader.data_packet_stage = DATA_TYPE_BEGIN;

	memset(m_chatAppHeader.data_packet_payload, 0, APP_DATA_SIZE);
}

BOOL CChatAppLayer::Send(unsigned char* ppayload, int nlength)
{
	m_ppayload = ppayload; // ppayload : 입력 데이터
	m_length = nlength; //nlength: 데이터 길이

	if (nlength <= APP_DATA_SIZE) {
		((CEthernetLayer*)GetUnderLayer())->SetEnetHeaderFrameType(0x2080);
		m_chatAppHeader.data_packet_length = nlength;
		memcpy(m_chatAppHeader.data_packet_payload, ppayload, nlength); // GetBuff에 data 복사
		mp_UnderLayer->Send((unsigned char*)&m_chatAppHeader, m_length + APP_HEADER_SIZE);
	}
	else {
		AfxBeginThread(ChatThread, this); // 새로운 쓰레드 생성 후 ChatThread함수를 실행
	}
	return TRUE;
}

BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{

	CHAT_APP_HEADER_POINTER header = (CHAT_APP_HEADER_POINTER)ppayload;// ChatApp 헤더 포인터 -> ppayload .
	static unsigned char* DataBuffer; // 데이터를 넣을 변수 선언

	if (header->data_packet_length <= APP_DATA_SIZE) {
		DataBuffer = (unsigned char*)malloc(header->data_packet_length); //데이터 크기만큼 메모리 할당
		memset(DataBuffer, 0, header->data_packet_length);   //데이터 크기 초기화
		memcpy(DataBuffer, header->data_packet_payload, header->data_packet_length); //데이터 저장
		DataBuffer[header->data_packet_length] = '\0';//마지막

		mp_aUpperLayer[0]->Receive((unsigned char*)DataBuffer); // 다음 계층으로 데이터 올림
		return TRUE;
	}
	// 아래 계층에서 넘겨받은 데이터 분석 후 ChatDlg 계층으로 넘겨줌
	if (header->data_packet_stage == DATA_TYPE_BEGIN) // 패킷들로 나눠진 데이터 중 첫번째 패킷일 때
	{
		// 패킷 크기만큼 메모리 할당	
		DataBuffer = (unsigned char*)malloc(header->data_packet_length);
		memset(DataBuffer, 0, header->data_packet_length);  // 데이터 크기 초기화
	}
	else if (header->data_packet_stage == DATA_TYPE_CONT) // 패킷들로 나눠진 데이터 중 중간번째 패킷일 때
	{
		// 데이터를 넣을 장소에 헤더의 데이터 추가
		strncat((char*)DataBuffer, (char*)header->data_packet_payload, strlen((char*)header->data_packet_payload));
		DataBuffer[strlen((char*)DataBuffer)] = '\0'; //마지막 
	}
	else if (header->data_packet_stage == DATA_TYPE_END) // 패킷들로 나눠진 데이터 중 마지막 패킷일 때
	{
		// 버퍼에 쌓인 데이터를 다시 DataBuffer에 넣고
		memcpy(DataBuffer, DataBuffer, header->data_packet_length);
		DataBuffer[header->data_packet_length] = '\0';

		// 위에서 만든 메시지를 ChatDlg로 넘겨줌
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
	int data_length = APP_DATA_SIZE; // data의 길이
	int data_packet_num; // 전송할 데이터를 몇 개의 패킷으로 나누어야 하는지를 나타내는 변수
	int data_index;	 // data를 패킷으로 나눠 보낼 때, 현재 패킷이 데이터를 어디까지 처리했는지를 추적하는 변수
	// 다음 패킷을 보낼 때의 첫 index가 된다.
	int finished_index = 0;

	// 패킷의 총 개수를 정함
	if (pChat->m_length < APP_DATA_SIZE) // APP_DATA_SIZE 보다 작을 경우 한 번만 전송하면 됨
		data_packet_num = 1;
	else // 그렇지 않을 경우, 데이터 길이로 APP_DATA_SIZE를 나눈 몫에 1을 더한 값으로 data_packet_num을 결정
		data_packet_num = (pChat->m_length / APP_DATA_SIZE) + 1;

	for (int i = 0; i <= data_packet_num + 1; i++)
	{
		// 보낼 data의 길이를 결정
		if (data_packet_num == 1) { // 보낼 횟수가 한 번이면, 데이터 길이 만큼 보냄
			data_length = pChat->m_length;
		}
		else { // 보낼 횟수가 두 번 이상
			if (i == data_packet_num) // 마지막 전송일 때, 남은 데이터의 길이만큼 보냄
				data_length = pChat->m_length % APP_DATA_SIZE;
			else // 처음, 중간 데이터 일 경우, APP_DATA_SIZE만큼 보냄
				data_length = APP_DATA_SIZE;
		}

		memset(pChat->m_chatAppHeader.data_packet_payload, 0, data_length);

		if (i == 0) // 처음부분, 데이터의 총 길이를 전송
		{
			pChat->m_chatAppHeader.data_packet_length = pChat->m_length;  //길이
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_BEGIN; //처음 패킷 지정, 타입: 0x00
			memset(pChat->m_chatAppHeader.data_packet_payload, 0, data_length); //초기화
			data_length = 0;
		}
		else if (i != 0 && i <= data_packet_num) // 중간 패킷 처리
		{
			data_index = data_length * 2;
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_CONT; //중간 패킷 지정, 타입: 0x01
			pChat->m_chatAppHeader.data_packet_sequence = i - 1;

			CString str;
			str = pChat->m_ppayload;
			str = str.Mid(finished_index, finished_index + data_index);//finished_index부터 finished_index + data_index 까지 데이터 추출

			memcpy(pChat->m_chatAppHeader.data_packet_payload, str, data_length);
			finished_index += data_index;
		}
		else // 마지막 패킷 처리
		{
			pChat->m_chatAppHeader.data_packet_stage = DATA_TYPE_END; //마지막 패킷 지정, 타입: 0x02
			memset(pChat->m_ppayload, 0, data_length); //초기화
			data_length = 0;
		}
		isSuccess = pChat->mp_UnderLayer->Send((unsigned char*)&pChat->m_chatAppHeader, data_length + APP_HEADER_SIZE); //Ethernet레이어의 send함수로 데이터 전달
	}

	return isSuccess;
}