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
		//mac_addr: ���ּ� ������ ���� union ����
		//mac_byte_parts: �� �ּҸ� unsigned char Ÿ���� �̿��Ͽ� ����Ʈ������ ����
#define addr0 mac_addr.mac_byte_parts.e0
#define addr1 mac_addr.mac_byte_parts.e1
#define addr2 mac_addr.mac_byte_parts.e2
#define addr3 mac_addr.mac_byte_parts.e3
#define addr4 mac_addr.mac_byte_parts.e4
#define addr5 mac_addr.mac_byte_parts.e5

#define addrs  mac_addr.s_ether_addr

	} ETHERNET_ADDR, * LPETHERNET_ADDR;

	typedef struct _ETHERNET_HEADER {

		ETHERNET_ADDR	destination_address;// �̴��� ���̾� ������ MAC �ּ�
		ETHERNET_ADDR	source_address;		// �̴��� ���̾� �ٿ��� MAC�ּ�
		unsigned short	frame_type;			// �̴��� �������� � ���� ���� �������ݷ� ���޵��� �����ϴ� �ʵ�(IPv4, IPv6 ��)
		unsigned char	total_data[ETHER_MAX_DATA_SIZE]; // �̴��� ����� ��� ������

	} ETHERNET_HEADER, * PETHERNET_HEADER;

protected:
	ETHERNET_HEADER	m_etherNetHeader;//�̴��� �ش�Ÿ�� ����
	enum {
		FRAME_TYPE_IPv4 = 0x0800, //IPv4(�Ϲ������� ���Ǵ� IP ��Ŷ)
		FRAME_TYPE_ARP = 0x0806,//ARP(Address Resolution Protocol)
		FRAME_TYPE_IPv6 = 0x86DD,  //IPv6
		FRAME_TYPE_APP_DATA = 0x8020  //���ø����̼� ������(�ڵ忡�� ���ǵ� Ư�� ��)
	};
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
