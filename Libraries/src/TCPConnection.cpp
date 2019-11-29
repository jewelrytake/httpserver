#include <TCPConnection.h>
#include <IPAddress.h>

Network::TCPConnection::TCPConnection(TCPSocket socket, Network::IPAddress ip)
	: m_socket(socket), m_ip(ip)
{
	m_stringRepresentation = "[" + ip.GetAddress() + ":" + std::to_string(ip.GetPort()) + "]";
}

void Network::TCPConnection::Close()
{
	m_socket.Close();
}

std::string Network::TCPConnection::ToString() const
{
	return m_stringRepresentation;
}

Network::IPAddress Network::TCPConnection::GetIpAddress() const
{
	return m_ip;
}

struct TCPConnectionHash {
	std::size_t operator()(Network::TCPConnection p) const
	{
		std::hash<int> hashVal1;
		return hashVal1(p.m_socket.GetHandle());
	}
};

bool Network::operator==(const TCPConnection& lhs, const TCPConnection& rhs)
{
	return lhs.m_buffer == rhs.m_buffer &&
		lhs.m_ip == rhs.m_ip &&
		lhs.m_socket == rhs.m_socket &&
		lhs.m_stringRepresentation == rhs.m_stringRepresentation &&
		lhs.pm_incoming == rhs.pm_incoming &&
		lhs.pm_outgoing == rhs.pm_outgoing;
}