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
				CFile::Remove(_T("IpcBuff.txt")); // ���� ����
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



BOOL CFileLayer::Send(unsigned char* ppayload, int nlength)//EthernetFrame���� ������ ��
{
	string file_name = "IpcBuff.txt";

	TRY
	{
		CFile m_FileDes(file_name.c_str(), CFile::modeCreate | CFile::modeWrite);
	//////////////////////// fill the blank ///////////////////////////////
			// ���� ����
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

BOOL CFileLayer::Receive() //EthernetLayer�κ��� ������
{
	string file_name = "IpcBuff.txt";
	TRY
	{
		CFile m_FileDes(file_name.c_str(), CFile::modeRead);

	//////////////////////// fill the blank ///////////////////////////////
			// ������ ������ �����´�.

			// Ethernet Frame = Header | Data �̹Ƿ�, ���� ��������� ũ���� �����Ͱ� ����ִ��� �𸥴�.
			// �׷��� Ethernet Header ũ��� Ethernet Data�� �ִ� ũ��� Frame�� ũ�⸦ ���Ѵ�.
			int nlength = ETHER_HEADER_SIZE + ETHER_MAX_DATA_SIZE;
			unsigned char* ppayload = new unsigned char[nlength + 1];

			// ������ Frame�� ���̸�ŭ ������ ����(��� ���μ������� ���� ���� Ethernet Frame)��
			// �о�ͼ� ppayload�� �����Ѵ�.
			m_FileDes.Read(ppayload,nlength);
			ppayload[nlength] = '\0';

			// Ethernet �������� ���Ͽ��� ������ Frame�� �Ѱ��ش�. 
			if (!mp_aUpperLayer[0]->Receive(ppayload)) { // �Ѱ����� ���ߴٸ� FALSE
				m_FileDes.Close();
				return FALSE;
			}
			// �����ߴٸ� TRUE�� return
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

