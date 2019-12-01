#pragma once
#include "TCPConnection.hpp"

namespace Network
{
	struct Server
	{
		bool Start(IPAddress ip);
		void Frame();
		void ShareMessage(TCPConnection& connected, std::shared_ptr<Packet> packet);
	protected:
		virtual void OnConnect(TCPConnection& connected);
		virtual void OnDisconnect(TCPConnection& connected, std::string&& reason);
		virtual bool ProcessPacket(TCPConnection& connected, std::shared_ptr<Packet> packet);
		void CloseConnection(int connectionIndex, std::string&& reason);
		TCPSocket m_listeningSocket;
		//vector of all the connections to server, including listening socket
		std::vector<WSAPOLLFD> m_master_fd{};
		std::vector<WSAPOLLFD> m_use_fd{};
		std::vector<TCPConnection> m_connections;
	};
}
