// FileLayer.h: interface for the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
#define AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"

class CFileLayer
	: public CBaseLayer
{
public:
	BOOL	Receive();
	BOOL	Send(unsigned char* ppayload, int nlength);

	CFileLayer(char* pName);
	virtual ~CFileLayer();
	typedef struct _FILE_APP {
		unsigned long fapp_totlen; //파일의 총 길이
		unsigned short fapp_type; //단편화 데이터 타입
		unsigned char fapp_msg_type; //메시지(확장자?) 종류
		unsigned char unused; // 사용 X
		unsigned long fapp_seq_num; //단편화된 조각 순서
		unsigned char fapp_data[MAX_APP_DATA] //파일 데이터
	};
};

#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
