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

	static UINT		ChatThread(LPVOID pParam);	// ChatApp ������

	CChatAppLayer(char* pName);
	virtual ~CChatAppLayer();

	typedef struct _CHAT_APP_HEADER {
		unsigned long	data_packet_sequence;	// ������ ���� ������ ��Ŷ�� ���� 
		unsigned short	data_packet_length;		// ���� ��Ŷ ����
		unsigned char	data_packet_stage;		// ���� ��Ŷ�� ù��° ��Ŷ����, �߰� ��Ŷ����, ������ ��Ŷ����

		unsigned char	data_packet_payload[APP_DATA_SIZE]; // ��Ŷ �� ������ 

	} CHAT_APP_HEADER, * CHAT_APP_HEADER_POINTER;

protected:
	CHAT_APP_HEADER		m_chatAppHeader;
	enum {
		DATA_TYPE_BEGIN = 0x00,	// ���۵� ������ ��Ŷ ó�� �κ�
		DATA_TYPE_CONT = 0x01,	// ���۵� ������ ��Ŷ �߰� �κ�
		DATA_TYPE_END = 0x02  // ���۵� ������ ��Ŷ �� �κ�
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










