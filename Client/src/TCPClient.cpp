#include <TCPClient.hpp>
#include <iostream>

using namespace Network;

bool TCPClient::ProcessPacket(TCPConnection& connected, std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		*packet >> chatmessage;
		std::cout << "Chat Message: " << chatmessage << '\n';
		break;
	}
	case PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		*packet >> arraySize;
		std::cout << "Array Size: " << arraySize << '\n';
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t element = 0;
			*packet >> element;
			std::cout << "Element[" << i << "] - " << element << '\n';
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << (uint16_t)packet->GetPacketType() << '\n';
		return false;
	}

	return true;
}

void TCPClient::OnConnect()
{
	std::cout << "Successfully connected to the server!" << '\n';
	std::shared_ptr<Packet> helloPacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*helloPacket << std::string("Hello from the client!");
	m_connection.pm_outgoing.Append(helloPacket);
}