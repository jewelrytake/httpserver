#pragma once
#include <Include.hpp>

namespace Network
{
	struct TCPClient : public Network::Client
	{
	private:
		bool ProcessPacket(std::shared_ptr<Network::Packet> packet) override;
		void OnConnect() override;
	};
}
