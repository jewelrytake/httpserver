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
		TCPConnection() : m_socket(TCPSocket()) {};
		void Close();
		std::string ToString();
		TCPSocket m_socket;
		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		uint8_t m_buffer[Network::g_maxPacketSize] = { 0 };
	private:
		IPAddress m_ip;
		std::string m_stringRepresentation;
	};
}