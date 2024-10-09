#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetEthernetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetEthernetHeader()
{
	memset(m_etherNetHeader.destination_address.addrs, 0, 6);
	memset(m_etherNetHeader.source_address.addrs, 0, 6);
	memset(m_etherNetHeader.total_data, 0, ETHER_MAX_DATA_SIZE);
	m_etherNetHeader.frame_type = 0x3412; // 0x0800
}

unsigned char* CEthernetLayer::GetEnetHeaderDstAddress() // 이더넷 목적지 주소
{
	return m_etherNetHeader.destination_address.addrs;
}

unsigned char* CEthernetLayer::GetEnetHeaderSrcAddress() // 이더넷 근원지 주소
{
	return m_etherNetHeader.source_address.addrs;
}

void CEthernetLayer::SetEnetHeaderSrcAddress(unsigned char* pAddress)
{
	// 이더넷 주소 12자리를 가져온다. (NI Layer에서 가져온다.)
	ETHERNET_ADDR ethernet_src;
	ethernet_src.addr0 = pAddress[0];
	ethernet_src.addr1 = pAddress[1];
	ethernet_src.addr2 = pAddress[2];
	ethernet_src.addr3 = pAddress[3];
	ethernet_src.addr4 = pAddress[4];
	ethernet_src.addr5 = pAddress[5];
	memcpy(m_etherNetHeader.source_address.addrs, ethernet_src.addrs, 6);
}

void CEthernetLayer::SetEnetHeaderDstAddress(unsigned char* pAddress)
{
	ETHERNET_ADDR ethernet_dst;
	ethernet_dst.addr0 = pAddress[0];
	ethernet_dst.addr1 = pAddress[1];
	ethernet_dst.addr2 = pAddress[2];
	ethernet_dst.addr3 = pAddress[3];
	ethernet_dst.addr4 = pAddress[4];
	ethernet_dst.addr5 = pAddress[5];
	memcpy(m_etherNetHeader.destination_address.addrs, ethernet_dst.addrs, 6);
}

void CEthernetLayer::SetEnetHeaderFrameType(unsigned short type) {
	m_etherNetHeader.frame_type = type;
}

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	memcpy(m_etherNetHeader.total_data, ppayload, nlength); //PPAYLOAD데이터를 M_ETHERnEThEADER.TOTAL_DATA에 복사

	BOOL isSuccess = FALSE;
	isSuccess = mp_UnderLayer->Send((unsigned char*)&m_etherNetHeader, nlength + ETHER_HEADER_SIZE); //하위 NI레이어로 데이터를 전달

	return isSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	// 하위 계층에서 받은 payload를 현재 계층의 PETHERNET_HEADER로 캐스팅해서 header구조에 맞게 매핑함.
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	BOOL isSuccess = FALSE;
	if (!memcmp((char*)pFrame->destination_address.addrs, (char*)m_etherNetHeader.source_address.addrs, 6) &&
		memcmp((char*)pFrame->source_address.addrs, (char*)m_etherNetHeader.source_address.addrs, 6))
	{
		if (ntohs(pFrame->frame_type) == FRAME_TYPE_APP_DATA) { //Ethernet_Header의 프레임타입이 애플리케이션 데이터일 때
			isSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pFrame->total_data); //상위 ChatApp레이어로 데이터를 전달
		}
	}

	return isSuccess;
}
