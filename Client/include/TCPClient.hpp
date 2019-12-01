#pragma once
#include <Include.hpp>

namespace Network
{
	struct TCPClient : public Network::Client
	{
	private:
		bool ProcessPacket(TCPConnection& connected, std::shared_ptr<Packet> packet) override;
		void OnConnect() override;
	};
}
