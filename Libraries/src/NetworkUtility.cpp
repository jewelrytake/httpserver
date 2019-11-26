#include <NetworkUtility.h>
#include <iostream>


void Network::OnConnect(TCPConnection& newConnection, Network::Server& server)
{
	std::cout << newConnection.ToString() << " - New connection accepted.\n";
}

void Network::OnDisconnect(TCPConnection& lostConnection, std::string&& reason, Network::Server& server)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << ".\n";
}

void Network::CloseConnection(int connectionIndex, std::string&& reason, Network::Server& server)
{
	TCPConnection& connection = server.m_connections[connectionIndex];
	OnDisconnect(connection, std::move(reason), server);
	server.m_master_fd.erase(server.m_master_fd.begin() + (connectionIndex + 1));
	server.m_use_fd.erase(server.m_use_fd.begin() + (connectionIndex + 1));
	connection.Close();
	server.m_connections.erase(server.m_connections.begin() + connectionIndex);
}

bool Network::ProcessPacket(std::shared_ptr<Packet> packet)
{
	std::cout << "Packet received with size: " << packet->m_buffer.size() << '\n';
	return true;
}

#pragma region Read data
void ProcessPacketContent(Network::TCPConnection& connection)
{
	std::shared_ptr<Network::Packet> packet = std::make_shared<Network::Packet>();
	packet->m_buffer.resize(connection.pm_incoming.currentPacketSize);
	memcpy(&packet->m_buffer[0], connection.m_buffer, connection.pm_incoming.currentPacketSize);
	connection.pm_incoming.Append(packet);
	connection.pm_incoming.currentPacketSize = 0;
	connection.pm_incoming.currentPacketExtractionOffset = 0;
	connection.pm_incoming.m_currentTask = Network::PacketTask::ProcessPacketSize;
}

ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, int bytesReceived, int connectionIndex, Network::Server& server, ConditionStrategy cs)
{
	
	if (connection.pm_incoming.m_currentTask == Network::PacketTask::ProcessPacketSize)
	{
		if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
		{
			connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
			if (connection.pm_incoming.currentPacketSize > Network::g_maxPacketSize)
			{
				CloseConnection(connectionIndex, "Packet size too large.", server);
				if (cs == ConditionStrategy::ST_CONTINUE)
					return ConditionStrategy::ST_CONTINUE;
				else if (cs == ConditionStrategy::ST_FALSE)
					return ConditionStrategy::ST_FALSE;
			}
			connection.pm_incoming.currentPacketExtractionOffset = 0;
			connection.pm_incoming.m_currentTask = Network::PacketTask::ProcessPacketContents;
		}
	}
}
#pragma endregion End code specific to read data

