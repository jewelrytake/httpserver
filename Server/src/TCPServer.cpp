#include <TCPServer.hpp>
#include <iostream>

using namespace Network;

void TCPServer::OnConnect(TCPConnection& connected)
{
	std::cout << connected.ToString() << " - New connection accepted.\n";
	std::shared_ptr<Packet> welcomeMessagePacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*welcomeMessagePacket << "Welcome";
	//Send to the client
	connected.pm_outgoing.Append(welcomeMessagePacket);
	std::shared_ptr<Packet> newUserMessagePacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*newUserMessagePacket << "New user connected!";
	for (auto& connection : m_connections)
	{
		if (&connection == &connected)
			continue;
		connection.pm_outgoing.Append(newUserMessagePacket);
	}
}

void TCPServer::OnDisconnect(TCPConnection& connected, std::string&& reason)
{
	std::cout << "[" << reason << "] Connection lost: " << connected.ToString() << ".\n";
	std::shared_ptr<Packet> connectionLostPacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*connectionLostPacket << "A user disconnected!";
	for (auto& connection : m_connections)
	{
		if (&connection == &connected)
			continue;
		connection.pm_outgoing.Append(connectionLostPacket);
	}
}

bool TCPServer::ProcessPacket(std::shared_ptr<Packet> packet)
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
		std::cout << "Array size: " << arraySize << '\n';
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