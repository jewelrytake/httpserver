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

std::string Network::TCPConnection::ToString()
{
	return m_stringRepresentation;
}