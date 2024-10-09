// FileLayer.h: interface for the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
#define AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_
#define MAX_APP_DATA 1500

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
	BOOL	fapp_type ;

	CFileLayer(char* pName);
	virtual ~CFileLayer();

	static UINT FileThread(LPVOID pParam);
	typedef struct FILE_Fragmet
	{
		unsigned long fapp_totlen; // 총 길이
		unsigned short fapp_type; // 데이터 타입 단편화에 대한 정보를 담을 수 있음 ex) 0x00 – 단편화 되지 않음, 0x01 – 단편화 첫부분, 0x02 – 단편화 중간, 등등.

		unsigned char fapp_msg_type; // 메시지 종류
		unsigned char unused; // 사용 안함
		unsigned long fapp_seq_num; // fragmentation 순서 ❖ seq_num에서 이 단편화된 조각이 몇 번째 조각인지 저장 

		unsigned char fapp_data[MAX_APP_DATA];
	} FILE_FRAGMENT, * PFILE_FRAGMENT;


	typedef struct _ThreadParams {
		unsigned char* ppayload;
		int nlength;
		CFileLayer* pFileLayer;
	} ThreadParams;


};

#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
