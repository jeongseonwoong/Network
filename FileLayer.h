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
private:
	inline void ResetFileHeader();

public:
	unsigned char* m_ppayload;
	int m_length;

	BOOL	Receive();
	BOOL	Send(unsigned char* ppayload, int nlength);

	static UINT FileThread(LPVOID pParm); //파일 전송 thread

	CFileLayer(char* pName);
	virtual ~CFileLayer();

	typedef struct _FILE_APP {
		unsigned long fapp_totlen; //파일의 총 길이
		unsigned short fapp_type; //단편화 데이터 타입
		unsigned char fapp_msg_type; //메시지(확장자?) 종류
		unsigned char unused; // 사용 X
		unsigned long fapp_seq_num; //단편화된 조각 순서
		unsigned char fapp_data[MAX_APP_DATA]; //파일 데이터
	}FILE_APP_HEADER, *FILE_APP_HEADER_POINTER;

protected:
	FILE_APP_HEADER m_fileHeader;
	enum {
		FAPP_TYPE_BEGIN = 0x00,	// 전송된 나눠진 패킷 처음 부분
		FAPP_TYPE_CONT = 0x01,	// 전송된 나눠진 패킷 중간 부분
		FAPP_TYPE_END = 0x02  // 전송된 나눠진 패킷 끝 부분
	};
};

#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
