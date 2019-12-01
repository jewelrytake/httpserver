#pragma once
#include "Packet.hpp"
#include <queue>
#include <memory>

namespace Network
{
	enum class PacketTask
	{
		ProcessPacketSize,
		ProcessPacketContents
	};

	struct PacketManager
	{
		void Clear();
		bool HasPendingPackets();
		void Append(std::shared_ptr<Packet> packet);
		std::shared_ptr<Packet> GetCurrentPacket();
		void Pop();
		uint16_t currentPacketSize = 0;
		size_t currentPacketExtractionOffset = 0;
		PacketTask m_currentTask = PacketTask::ProcessPacketSize;
		friend bool operator==(const PacketManager& lhs, const PacketManager& rhs);
		
	private:
		std::queue< std::shared_ptr< Packet > > m_packets;
		
	};
}