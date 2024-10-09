// ChatAppLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"


class CChatAppLayer
	: public CBaseLayer
{
private:
	inline void		ResetChatAppHeader();
	CObject* UI_Interaction;

public:
	unsigned char* m_ppayload;
	int				m_length;

	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char*, int);

	static UINT		ChatThread(LPVOID pParam);	// ChatApp 쓰레드

	CChatAppLayer(char* pName);
	virtual ~CChatAppLayer();

	typedef struct _CHAT_APP_HEADER {
		unsigned long	data_packet_sequence;	// 나눠진 현재 데이터 패킷의 순서 
		unsigned short	data_packet_length;		// 현재 패킷 길이
		unsigned char	data_packet_stage;		// 현재 패킷이 첫번째 패킷인지, 중간 패킷인지, 마지막 패킷인지

		unsigned char	data_packet_payload[APP_DATA_SIZE]; // 패킷 내 데이터 

	} CHAT_APP_HEADER, * CHAT_APP_HEADER_POINTER;

protected:
	CHAT_APP_HEADER		m_chatAppHeader;
	enum {
		DATA_TYPE_BEGIN = 0x00,	// 전송된 나눠진 패킷 처음 부분
		DATA_TYPE_CONT = 0x01,	// 전송된 나눠진 패킷 중간 부분
		DATA_TYPE_END = 0x02  // 전송된 나눠진 패킷 끝 부분
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










