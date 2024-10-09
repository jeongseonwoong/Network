#include "stdafx.h"
#include "pch.h"
#include "FileLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLayer::CFileLayer(char* pName)
	: CBaseLayer(pName)
{
}

CFileLayer::~CFileLayer()
{
	TRY
	{
		//////////////////////// fill the blank ///////////////////////////////
				CFile::Remove(_T("IpcBuff.txt")); // 파일 제거
	///////////////////////////////////////////////////////////////////////
	}
		CATCH(CFileException, e)
	{
#ifdef _DEBUG
		afxDump << "File cannot be removed\n";
#endif
	}
	END_CATCH
}

BOOL CFileLayer::Send(unsigned char* ppayload, int nlength)
{
	TRY
	{
		CFile m_FileDes(_T("IpcBuff.txt"), CFile::modeCreate | CFile::modeWrite);
		
	int fileSize = 1488;
	int f_length = nlength;
	unsigned char* f_ppayload = ppayload;

	while (f_length > 0)
	{
		int bytesToWrite = min(fileSize, f_length);
		m_FileDes.Write(f_ppayload, bytesToWrite);
		f_ppayload += bytesToWrite;
		f_length -= bytesToWrite;
	}
		m_FileDes.Close();
	}
		CATCH(CFileException, e)
	{
#ifdef _DEBUG
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		return FALSE;
	}
	END_CATCH

		return TRUE;
}

BOOL CFileLayer::Receive()
{
	TRY
	{
		CFile m_FileDes(_T("IpcBuff.txt"), CFile::modeRead);

			// 파일의 내용을 가져온다.

			// Ethernet Frame = Header | Data 이므로, 현재 어느정도의 크기의 데이터가 들어있는지 모른다.
			// 그래서 Ethernet Header 크기와 Ethernet Data의 최대 크기로 Frame의 크기를 정한다.
			int filesize = 1488;
			int TotalRead = 0;
			int nlength = ETHER_HEADER_SIZE + ETHER_MAX_DATA_SIZE;
			unsigned char* ppayload = new unsigned char[nlength + 1];

			// 정해진 Frame의 길이만큼 파일의 내용(상대 프로세스에게 전송 받은 Ethernet Frame)을
			// 읽어와서 ppayload를 결정한다.
			while (TotalRead < nlength)
			{
				int bytestoRead = min(filesize, nlength - TotalRead);
				int bytesRead = m_FileDes.Read(ppayload + TotalRead, bytestoRead);

				if (bytesRead == 0)
					break;

				TotalRead += bytesRead;
			}
			ppayload[TotalRead] = '\0';

			// Ethernet 계층으로 파일에서 가져온 Frame을 넘겨준다. 
			if (!mp_aUpperLayer[0]->Receive(ppayload)) { // 넘겨주지 못했다면 FALSE
				m_FileDes.Close();
				delete[] ppayload;
				return FALSE;
			}

			// 성공했다면 TRUE를 return
			m_FileDes.Close();
			delete[] ppayload;
	}
		CATCH(CFileException, e)
	{
#ifdef _DEBUG
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		return FALSE;
	}
	END_CATCH

		return TRUE;
}
