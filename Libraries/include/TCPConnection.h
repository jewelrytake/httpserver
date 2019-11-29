#pragma once
#include "TCPSocket.h"
#include "PacketManager.h"
#include "Constants.h"
#include <IPAddress.h>

namespace Network
{
	struct TCPConnection
	{
		TCPConnection(TCPSocket socket, IPAddress ip);
		//TODO: mb need to activate this constructor
		//TCPConnection() : m_socket(TCPSocket()) {};
		void Close();
		std::string ToString() const;
		TCPSocket m_socket;
		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		uint8_t m_buffer[Network::g_maxPacketSize] = { 0 };
		IPAddress GetIpAddress() const;
		friend bool operator==(const TCPConnection& lhs, const TCPConnection& rhs);
	private:
		IPAddress m_ip;
		std::string m_stringRepresentation;
	};
}