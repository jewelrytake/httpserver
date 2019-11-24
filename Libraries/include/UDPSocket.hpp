#pragma once
#include <WinSock2.h>
#include <IPAddress.h>
#include <Packet.hpp>

namespace Network
{
	struct UDPSocket
	{
		UDPSocket(const UDPSocket&) = delete;
		UDPSocket(UDPSocket&&) = delete;
		UDPSocket& operator=(const UDPSocket&) = delete;
		UDPSocket operator=(UDPSocket&&) = delete;

		//API
		UDPSocket(int type = SOCK_DGRAM, int protocol = IPPROTO_UDP,
			SOCKET handle = INVALID_SOCKET, IPVersion version = IPVersion::IPv4);
		~UDPSocket();

		bool Create();
		bool Close();
		SOCKET GetHandle() const;
		IPVersion GetIPVersion() const;
		bool Bind(IPAddress endpoint);
		bool Connect(IPAddress endpoint);
		bool Send(Packet& packet);
		bool Recv(Packet& packet);

	private:
		int m_type;
		int m_protocol;
		SOCKET m_handle;
		IPVersion m_ipVersion;
	};
}