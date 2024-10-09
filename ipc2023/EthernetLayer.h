// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CEthernetLayer
	: public CBaseLayer
{
private:
	inline void		ResetEthernetHeader();

public:
	unsigned short	tcp_port;
	void			StartThread();

	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);

	void			SetEnetHeaderDstAddress(unsigned char* pAddress);
	void			SetEnetHeaderSrcAddress(unsigned char* pAddress);
	void			SetEnetHeaderFrameType(unsigned short type);
	unsigned char* GetEnetHeaderDstAddress();
	unsigned char* GetEnetHeaderSrcAddress();

	CEthernetLayer(char* pName);
	virtual ~CEthernetLayer();

	typedef struct _ETHERNET_ADDR
	{
		union {
			struct { unsigned char e0, e1, e2, e3, e4, e5; } mac_byte_parts;
			unsigned char s_ether_addr[6];
		} mac_addr;
		//mac_addr: 맥주소 설정을 위한 union 변수
		//mac_byte_parts: 맥 주소를 unsigned char 타입을 이용하여 바이트단위로 설정
#define addr0 mac_addr.mac_byte_parts.e0
#define addr1 mac_addr.mac_byte_parts.e1
#define addr2 mac_addr.mac_byte_parts.e2
#define addr3 mac_addr.mac_byte_parts.e3
#define addr4 mac_addr.mac_byte_parts.e4
#define addr5 mac_addr.mac_byte_parts.e5

#define addrs  mac_addr.s_ether_addr

	} ETHERNET_ADDR, * LPETHERNET_ADDR;

	typedef struct _ETHERNET_HEADER {

		ETHERNET_ADDR	destination_address;// 이더넷 레이어 목적지 MAC 주소
		ETHERNET_ADDR	source_address;		// 이더넷 레이어 근원지 MAC주소
		unsigned short	frame_type;			// 이더넷 프레임이 어떤 상위 계층 프로토콜로 전달될지 구분하는 필드(IPv4, IPv6 등)
		unsigned char	total_data[ETHER_MAX_DATA_SIZE]; // 이더넷 헤더에 담긴 데이터

	} ETHERNET_HEADER, * PETHERNET_HEADER;

protected:
	ETHERNET_HEADER	m_etherNetHeader;//이더넷 해더타입 변수
	enum {
		FRAME_TYPE_IPv4 = 0x0800, //IPv4(일반적으로 사용되는 IP 패킷)
		FRAME_TYPE_ARP = 0x0806,//ARP(Address Resolution Protocol)
		FRAME_TYPE_IPv6 = 0x86DD,  //IPv6
		FRAME_TYPE_APP_DATA = 0x8020  //애플리케이션 데이터(코드에서 정의된 특정 값)
	};
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
