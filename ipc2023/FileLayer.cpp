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

UINT CFileLayer::FileThread(LPVOID pParam)
{
	using namespace std;
	// pParam�� ThreadParams Ÿ������ ĳ����
	ThreadParams* pParams = (ThreadParams*)pParam;

	// �μ� ����
	unsigned char* ppayload = pParams->ppayload;
	int nlength = pParams->nlength;

	const int CHUNK_SIZE = 1488;
	int chunkCount = (nlength + CHUNK_SIZE - 1) / CHUNK_SIZE; // �� ���� ���� ���
	std::vector<FILE_FRAGMENT> payloadChunks;

	// �����͸� 1488����Ʈ�� ������ FILE_FRAGMENT ����ü�� ����
	for (int i = 0; i < chunkCount; ++i) {
		FILE_FRAGMENT fragment = {};
		fragment.fapp_totlen = nlength;
		fragment.fapp_seq_num = i;

		// ������ Ÿ�� ����
		if (chunkCount == 1) {
			fragment.fapp_type = 0x00; // ����ȭ���� ����
		}
		else if (i == 0) {
			fragment.fapp_type = 0x01; // ����ȭ�� ù �κ�
		}
		else if (i == chunkCount - 1) {
			fragment.fapp_type = 0x03; // ����ȭ�� ������ �κ�
		}
		else {
			fragment.fapp_type = 0x02; // ����ȭ�� �߰� �κ�
		}

		// ���� ����Ʈ�� CHUNK_SIZE �� ���� ���� �����Ͽ� ������ ������ ó��
		int bytesToWrite = min(CHUNK_SIZE, nlength - i * CHUNK_SIZE);

		// �����͸� fapp_data�� ����
		memcpy(fragment.fapp_data, ppayload + i * CHUNK_SIZE, bytesToWrite);

		// �޽��� ���� ���� (�ʿ信 ���� ���� ����)
		fragment.fapp_msg_type = 0x01; // ��: �Ϲ� ������

		// ���Ϳ� ���� �߰�
		payloadChunks.push_back(fragment);
	}

	// ���� payloadChunks ���͸� ����Ͽ� �������� ������ ���� �Ǵ� ó�� ����

	// �������� �Ҵ��� �޸� ����
	delete pParams;

	return 0; // ������ ����
}






