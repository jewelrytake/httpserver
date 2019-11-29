#include <TCPSocket.h>
#include <assert.h>

Network::TCPSocket::TCPSocket(IPVersion version, int type, int protocol, SOCKET handle):
	m_type(type), m_protocol(protocol), m_handle(handle), m_ipVersion(version)
{}

Network::TCPSocket::~TCPSocket() = default;

bool Network::TCPSocket::Create()
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
	if (!SetBlocking(false))
	{
		return false;
	}
	return true;
}

bool Network::TCPSocket::Close()
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

SOCKET Network::TCPSocket::GetHandle() const
{
	return m_handle;
}

Network::IPVersion Network::TCPSocket::GetIPVersion() const
{
	return m_ipVersion;
}

bool Network::TCPSocket::Bind(IPAddress& ip)
{
	assert(m_ipVersion == ip.GetVersion());

	if (m_ipVersion == IPVersion::IPv4)
	{
		sockaddr_in addr = ip.GetIPv4Address();
		int result = bind(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return false;
		}
	}
	else //IPv6
	{
		sockaddr_in6 addr = ip.GetIPv6Address();
		int result = bind(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return false;
		}
	}
	return true;
}

bool Network::TCPSocket::Listen(IPAddress& ip, int backlog)
{
	if (!Bind(ip))
		return false;
	
	if (listen(m_handle, backlog) != 0) //If an error occurred
	{
		int error = WSAGetLastError();
		return false;
	}
	return true;
}

bool Network::TCPSocket::Accept(TCPSocket& outSocket, IPAddress* ip)
{
	assert(m_ipVersion == IPVersion::IPv4 || m_ipVersion == IPVersion::IPv6);
	if (m_ipVersion == IPVersion::IPv4)
	{
		sockaddr_in addr = {};//buffer that receives the address of the connecting entity
		int len = sizeof(sockaddr_in);
		//The accept function permits an incoming connection attempt on a socket.
		SOCKET acceptedConnectionHandle = accept(m_handle, (sockaddr*)(&addr), &len);
		if (acceptedConnectionHandle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return false;
		}
		if (ip != nullptr)
		{
			*ip = IPAddress((sockaddr*)&addr);
		}
		outSocket = TCPSocket(IPVersion::IPv4, SOCK_STREAM, IPPROTO_TCP, acceptedConnectionHandle);
	}
	else //IPv6
	{
		sockaddr_in6 addr = {};
		int len = sizeof(sockaddr_in6);
		SOCKET acceptedConnectionHandle = accept(m_handle, (sockaddr*)(&addr), &len);
		if (acceptedConnectionHandle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return false;
		}
		if (ip != nullptr)
		{
			*ip = IPAddress((sockaddr*)&addr);
		}
		outSocket = TCPSocket(IPVersion::IPv4, SOCK_STREAM, IPPROTO_TCP, acceptedConnectionHandle);
	}
	return true;
}

bool Network::TCPSocket::Connect(IPAddress& ip)
{
	assert(m_ipVersion == ip.GetVersion());
	int result = 0;
	if (m_ipVersion == IPVersion::IPv4)
	{
		sockaddr_in addr = ip.GetIPv4Address();
		result = connect(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
	}
	else //IPv6
	{
		sockaddr_in6 addr = ip.GetIPv6Address();
		result = connect(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
	}
	if (result != 0) //if an error occurred
	{
		int error = WSAGetLastError();
		return false;
	}
	return true;
}

int Network::TCPSocket::GetType()
{
	return m_type;
}

int Network::TCPSocket::GetProtocol()
{
	return m_protocol;
}

bool Network::TCPSocket::SetBlocking(bool isBlocking)
{
	unsigned long nonBlocking = 1;
	unsigned long blocking = 0;
	int result = 0;
	if (isBlocking == false)
	{
		result = ioctlsocket(m_handle, FIONBIO, &nonBlocking);
		m_isBlocking = false;
	}
	else
	{
		result = ioctlsocket(m_handle, FIONBIO, &blocking);
		m_isBlocking = true;
	}

	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		return false;
	}
	return true;
}

bool Network::TCPSocket::IsBlocking()
{
	return m_isBlocking;
}
struct TCPSocketHash {
	std::size_t operator()(Network::TCPSocket p) const
	{
		std::hash<int> intVal;
		std::hash<uint16_t> uint16_tVal;
		std::hash<bool> boolVal;
		return intVal(p.GetHandle()) ^ uint16_tVal((uint16_t)p.GetIPVersion()) ^ intVal(p.GetProtocol()) ^ intVal(p.GetType()) ^ boolVal(p.IsBlocking());
	}
};


bool Network::operator==(const TCPSocket& lhs, const TCPSocket& rhs)
{
	return lhs.m_handle == rhs.m_handle &&
		lhs.m_ipVersion == rhs.m_ipVersion &&
		lhs.m_isBlocking == rhs.m_isBlocking &&
		lhs.m_protocol == rhs.m_protocol &&
		lhs.m_type == rhs.m_type;
}
