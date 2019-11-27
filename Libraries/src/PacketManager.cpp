#include <PacketManager.h>

void Network::PacketManager::Clear()
{
	m_packets = std::queue<std::shared_ptr<Packet>>{}; // Clear out packet queue
}

bool Network::PacketManager::HasPendingPackets()
{
	return (!m_packets.empty());
}

void Network::PacketManager::Append(std::shared_ptr<Packet> p)
{
	m_packets.emplace(std::move(p));
}

std::shared_ptr<Network::Packet> Network::PacketManager::GetCurrentPacket()
{
	return m_packets.front();
}

void Network::PacketManager::Pop()
{
	m_packets.pop();
}