#pragma once
#include "TCPSocket.hpp"
#include "PacketManager.hpp"
#include "Constants.hpp"
#include <IPAddress.hpp>

namespace Network
{
	struct TCPConnection
	{
		TCPConnection(TCPSocket socket, IPAddress ip);
		TCPConnection() : m_socket(TCPSocket()) {};
		void Close();
		std::string ToString() const;
		TCPSocket m_socket;
		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		char m_buffer[Network::g_maxPacketSize] = { 0 };
		IPAddress GetIpAddress() const;
		friend bool operator==(const TCPConnection& lhs, const TCPConnection& rhs);
	private:
		IPAddress m_ip;
		std::string m_stringRepresentation;
	};
}