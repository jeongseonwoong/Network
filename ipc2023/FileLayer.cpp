// FileLayer.cpp: implementation of the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "FileLayer.h"
#include<vector>
#include<algorithm>

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



BOOL CFileLayer::Send(unsigned char* ppayload, int nlength)//EthernetFrame으로 전송할 때
{
	string file_name = "IpcBuff.txt";

	TRY
	{
		CFile m_FileDes(file_name.c_str(), CFile::modeCreate | CFile::modeWrite);
	//////////////////////// fill the blank ///////////////////////////////
			// 파일 생성
	if (nlength <= 1488) 
	{
		m_FileDes.Write(ppayload, nlength);
		m_FileDes.Close();		
	}
	else {
		ThreadParams* pParams = new ThreadParams;
		pParams->ppayload = ppayload;
		pParams->nlength = nlength;
		pParams->pFileLayer = this;
		AfxBeginThread(FileThread, pParams);
	}

			///////////////////////////////////////////////////////////////////////
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

BOOL CFileLayer::Receive() //EthernetLayer로부터 받을때
{
	string file_name = "IpcBuff.txt";
	TRY
	{
		CFile m_FileDes(file_name.c_str(), CFile::modeRead);

	//////////////////////// fill the blank ///////////////////////////////
			// 파일의 내용을 가져온다.

			// Ethernet Frame = Header | Data 이므로, 현재 어느정도의 크기의 데이터가 들어있는지 모른다.
			// 그래서 Ethernet Header 크기와 Ethernet Data의 최대 크기로 Frame의 크기를 정한다.
			int nlength = ETHER_HEADER_SIZE + ETHER_MAX_DATA_SIZE;
			unsigned char* ppayload = new unsigned char[nlength + 1];

			// 정해진 Frame의 길이만큼 파일의 내용(상대 프로세스에게 전송 받은 Ethernet Frame)을
			// 읽어와서 ppayload를 결정한다.
			m_FileDes.Read(ppayload,nlength);
			ppayload[nlength] = '\0';

			// Ethernet 계층으로 파일에서 가져온 Frame을 넘겨준다. 
			if (!mp_aUpperLayer[0]->Receive(ppayload)) { // 넘겨주지 못했다면 FALSE
				m_FileDes.Close();
				return FALSE;
			}
			// 성공했다면 TRUE를 return
	///////////////////////////////////////////////////////////////////////
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

UINT CFileLayer::FileThread(LPVOID pParam)
{
	using namespace std;
	// pParam을 ThreadParams 타입으로 캐스팅
	ThreadParams* pParams = (ThreadParams*)pParam;

	// 인수 추출
	unsigned char* ppayload = pParams->ppayload;
	int nlength = pParams->nlength;

	const int CHUNK_SIZE = 1488;
	int chunkCount = (nlength + CHUNK_SIZE - 1) / CHUNK_SIZE; // 총 조각 개수 계산
	std::vector<FILE_FRAGMENT> payloadChunks;

	// 데이터를 1488바이트씩 나누어 FILE_FRAGMENT 구조체에 저장
	for (int i = 0; i < chunkCount; ++i) {
		FILE_FRAGMENT fragment = {};
		fragment.fapp_totlen = nlength;
		fragment.fapp_seq_num = i;

		// 조각의 타입 설정
		if (chunkCount == 1) {
			fragment.fapp_type = 0x00; // 단편화되지 않음
		}
		else if (i == 0) {
			fragment.fapp_type = 0x01; // 단편화의 첫 부분
		}
		else if (i == chunkCount - 1) {
			fragment.fapp_type = 0x03; // 단편화의 마지막 부분
		}
		else {
			fragment.fapp_type = 0x02; // 단편화의 중간 부분
		}

		// 남은 바이트와 CHUNK_SIZE 중 작은 값을 선택하여 마지막 조각을 처리
		int bytesToWrite = min(CHUNK_SIZE, nlength - i * CHUNK_SIZE);

		// 데이터를 fapp_data에 복사
		memcpy(fragment.fapp_data, ppayload + i * CHUNK_SIZE, bytesToWrite);

		// 메시지 종류 설정 (필요에 따라 변경 가능)
		fragment.fapp_msg_type = 0x01; // 예: 일반 데이터

		// 벡터에 조각 추가
		payloadChunks.push_back(fragment);
	}

	// 이후 payloadChunks 벡터를 사용하여 조각별로 데이터 전송 또는 처리 가능

	// 동적으로 할당한 메모리 해제
	delete pParams;	

	return 0; // 스레드 종료
}

