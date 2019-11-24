#include <UDPSocket.hpp>
#include <assert.h>

Network::UDPSocket::UDPSocket(int type, int protocol, SOCKET handle, IPVersion version):
	m_type(type), m_protocol(protocol), m_handle(handle), m_ipVersion(version)
{}

Network::UDPSocket::~UDPSocket() = default;

bool Network::UDPSocket::Create()
{
	assert(m_ipVersion == IPVersion::IPv4 || m_ipVersion == IPVersion::IPv6);
	if (m_handle != INVALID_SOCKET)
	{
		return false;
	}
	m_handle = socket((m_ipVersion == IPVersion::IPv4) ? AF_INET : AF_INET6, m_type, m_protocol); //attempt to create socket

	if (m_handle == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		return false;
	}
	//if (SetBlocking(false) != PResult::P_Success)
	//{
	//	return PResult::P_GenericError;
	//}
	//if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success)
	//{
	//	return PResult::P_GenericError;
	//}
	return true;
}

bool Network::UDPSocket::Close()
{
	if (m_handle == INVALID_SOCKET)
	{
		return false;
	}
	if (closesocket(m_handle) != 0)
	{
		int error = WSAGetLastError();
		return false;
	}
	m_handle = INVALID_SOCKET;
	return true;
}

SOCKET Network::UDPSocket::GetHandle() const
{
	return m_handle;
}

Network::IPVersion Network::UDPSocket::GetIPVersion() const
{
	return m_ipVersion;
}

bool Network::UDPSocket::Bind(IPAddress endpoint)
{
	return false;
}

bool Network::UDPSocket::Connect(IPAddress endpoint)
{
	return false;
}

bool Network::UDPSocket::Send(Packet& packet)
{
	return false;
}

bool Network::UDPSocket::Recv(Packet& packet)
{
	return false;
}
