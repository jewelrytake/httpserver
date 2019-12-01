#pragma once
#include <Server.hpp>

namespace Network
{
	struct TCPServer : Server
	{
	private:
		void OnConnect(TCPConnection& connected) override;
		void OnDisconnect(TCPConnection& connected, std::string&& reason) override;
		bool ProcessPacket(TCPConnection& connected, std::shared_ptr<Packet> packet) override;
	};
}