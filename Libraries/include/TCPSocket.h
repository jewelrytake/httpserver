#pragma once
#include <WinSock2.h>
#include <IPAddress.h>
#include <Packet.hpp>

namespace Network
{
	struct TCPSocket 
	{

		//API
		TCPSocket(IPVersion version = IPVersion::IPv4, int type = SOCK_STREAM, int protocol = IPPROTO_TCP, SOCKET handle = INVALID_SOCKET);
		~TCPSocket();

		bool Create();
		bool Close();
		SOCKET GetHandle() const;
		IPVersion GetIPVersion() const;
		bool Bind(IPAddress& endpoint);
		bool Listen(IPAddress& endpoint, int backlog = 5);
		bool Accept(TCPSocket& outSocket, IPAddress* ip = nullptr);
		bool Connect(IPAddress& endpoint);
		//bool Send(const void* data, int numberOfBytes, int& bytesSent);
		//bool Recv(void* destination, int numberOfBytes, int& bytesReceived);
		//bool SendAll(const void* data, int numberOfBytes);
		//bool RecvAll(void* destination, int numberOfBytes);
		bool Send(Packet& packet);
		bool Recv(Packet& packet);

	private:
		IPVersion m_ipVersion;
		int m_type;
		int m_protocol;
		SOCKET m_handle;
	};
}