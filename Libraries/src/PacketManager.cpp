#include <PacketManager.hpp>


struct PacketManagerHash
{
	std::size_t operator()(Network::PacketManager p) const
	{
		std::hash<size_t> hashVal1;
		std::hash<uint16_t> hashVal2;
		return hashVal1(p.currentPacketExtractionOffset) ^ hashVal1(p.currentPacketSize) ^ hashVal2((uint16_t)p.m_currentTask);
	}
};

bool Network::operator==(const PacketManager& lhs, const PacketManager& rhs)
{
	return lhs.currentPacketExtractionOffset == rhs.currentPacketExtractionOffset 
		&& lhs.currentPacketSize == rhs.currentPacketSize && lhs.m_currentTask == rhs.m_currentTask;
}

void Network::PacketManager::Clear()
{
	m_packets = std::queue<std::shared_ptr<Packet>>{}; // Clear out packet queue
}

bool Network::PacketManager::HasPendingPackets()
{
	return !(m_packets.empty());
}

void Network::PacketManager::Append(std::shared_ptr<Packet> packet)
{
	m_packets.emplace(std::move(packet));
}

std::shared_ptr<Network::Packet> Network::PacketManager::GetCurrentPacket()
{
	return m_packets.front();
}

void Network::PacketManager::Pop()
{
	m_packets.pop();
}
